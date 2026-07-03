// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sock/demo_book/agent/tool_executor.h"

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include <fstream>

#include "rapidjson/document.h"

#include "sock/demo_book/agent/skill_registry.h"

namespace book_agent {

namespace {

const int kProcTimeoutMs = 6000;         // 子进程（grep）最长运行时间
const size_t kSearchMaxBytes = 6000;     // search_code 结果字节上限
const char kSearchMaxPerFile[] = "50";   // grep 每文件最多命中数
const uint32_t kReadMaxLines = 240;      // read_file 最多返回行数
const size_t kReadMaxBytes = 8000;       // read_file 结果字节上限
const size_t kResultMaxBytes = 8000;     // 任意工具结果的最终字节上限

// grep 可执行文件候选路径（网关进程 PATH 可能不含 rg，用绝对路径的系统 grep 最稳）
const char *kGrepCandidates[] = {"/usr/bin/grep", "/bin/grep", NULL};

const char kToolsSchema[] =
    "[{\"type\":\"function\",\"function\":{\"name\":\"search_code\","
    "\"description\":\"在 csutil 代码仓库根目录下检索关键字或正则，返回匹配的文件:行号:内容。用于为回答找到真实代码证据。\","
    "\"parameters\":{\"type\":\"object\",\"properties\":{"
    "\"query\":{\"type\":\"string\",\"description\":\"检索关键字或扩展正则（ERE）\"},"
    "\"glob\":{\"type\":\"string\",\"description\":\"可选，文件名通配限定，如 *.h\"}},"
    "\"required\":[\"query\"]}}},"
    "{\"type\":\"function\",\"function\":{\"name\":\"read_file\","
    "\"description\":\"读取 csutil 仓库根目录下某个源码/文档文件的片段（带行号）。仅限仓库内路径。\","
    "\"parameters\":{\"type\":\"object\",\"properties\":{"
    "\"path\":{\"type\":\"string\",\"description\":\"相对仓库根的路径，如 mystl/my_map.h\"},"
    "\"start\":{\"type\":\"integer\",\"description\":\"可选，起始行(1 基)\"},"
    "\"end\":{\"type\":\"integer\",\"description\":\"可选，结束行(含)\"}},"
    "\"required\":[\"path\"]}}},"
    "{\"type\":\"function\",\"function\":{\"name\":\"list_books\","
    "\"description\":\"列出 demo 业务库中的图书（可按前缀过滤）。\","
    "\"parameters\":{\"type\":\"object\",\"properties\":{"
    "\"prefix\":{\"type\":\"string\",\"description\":\"可选，book_id 前缀\"},"
    "\"limit\":{\"type\":\"integer\",\"description\":\"可选，最大条数\"}}}}},"
    "{\"type\":\"function\",\"function\":{\"name\":\"get_book\","
    "\"description\":\"按 book_id 获取一本图书的详情。\","
    "\"parameters\":{\"type\":\"object\",\"properties\":{"
    "\"book_id\":{\"type\":\"string\",\"description\":\"图书主键\"}},\"required\":[\"book_id\"]}}},"
    "{\"type\":\"function\",\"function\":{\"name\":\"list_skills\","
    "\"description\":\"列出当前可用的领域 Agent（名称与描述）。\","
    "\"parameters\":{\"type\":\"object\",\"properties\":{}}}}]";

/**
 * @brief 取 JSON 对象里的整型字段（兼容模型误传的字符串数字）
 */
int GetJsonInt(const rapidjson::Value &obj, const char *key, int def) { /*{{{*/
  if (!obj.HasMember(key)) return def;
  const rapidjson::Value &v = obj[key];
  if (v.IsInt()) return v.GetInt();
  if (v.IsString()) return atoi(v.GetString());
  return def;
} /*}}}*/

/**
 * @brief 取 JSON 对象里的字符串字段
 */
std::string GetJsonStr(const rapidjson::Value &obj, const char *key) { /*{{{*/
  if (!obj.HasMember(key) || !obj[key].IsString()) return "";
  return std::string(obj[key].GetString(), obj[key].GetStringLength());
} /*}}}*/

std::string FindGrep() { /*{{{*/
  for (uint32_t i = 0; kGrepCandidates[i] != NULL; ++i) {
    struct stat st;
    if (stat(kGrepCandidates[i], &st) == 0 && (st.st_mode & S_IXUSR)) return kGrepCandidates[i];
  }
  return "";
} /*}}}*/

/**
 * @brief fork+exec 运行一个进程，捕获其 stdout（无 shell，防注入），带超时与字节上限
 * @param argv 参数向量（argv[0] 为可执行文件绝对路径）
 * @param cwd 子进程工作目录（限定检索根）
 * @param timeout_ms 超时毫秒；超时则 SIGKILL
 * @param max_bytes stdout 累积上限
 * @param out 输出捕获内容
 * @return kOk 成功（含无匹配）；kForkFailed/kPipeFailed 启动失败
 */
base::Code RunProcessCapture(const std::vector<std::string> &argv, const std::string &cwd, int timeout_ms,
                             size_t max_bytes, std::string *out) { /*{{{*/
  int pipefd[2];
  if (pipe(pipefd) != 0) return base::kPipeFailed;

  pid_t pid = fork();
  if (pid < 0) {
    close(pipefd[0]);
    close(pipefd[1]);
    return base::kForkFailed;
  }
  if (pid == 0) {
    dup2(pipefd[1], STDOUT_FILENO);
    int devnull = open("/dev/null", O_WRONLY);
    if (devnull >= 0) dup2(devnull, STDERR_FILENO);
    close(pipefd[0]);
    close(pipefd[1]);
    if (!cwd.empty() && chdir(cwd.c_str()) != 0) _exit(127);
    std::vector<char *> cargv;
    cargv.reserve(argv.size() + 1);
    for (uint32_t i = 0; i < argv.size(); ++i) cargv.push_back(const_cast<char *>(argv[i].c_str()));
    cargv.push_back(NULL);
    execv(cargv[0], cargv.data());
    _exit(127);
  }

  close(pipefd[1]);
  fcntl(pipefd[0], F_SETFL, O_NONBLOCK);
  bool timed_out = false;
  while (out->size() < max_bytes) {
    struct pollfd pfd;
    pfd.fd = pipefd[0];
    pfd.events = POLLIN;
    int pr = poll(&pfd, 1, timeout_ms);
    if (pr == 0) {
      timed_out = true;
      break;
    }
    if (pr < 0) {
      if (errno == EINTR) continue;
      break;
    }
    char buf[4096];
    ssize_t n = read(pipefd[0], buf, sizeof(buf));
    if (n > 0) {
      out->append(buf, static_cast<size_t>(n));
    } else if (n == 0) {
      break;  // EOF
    } else {
      if (errno == EAGAIN || errno == EWOULDBLOCK) continue;
      break;
    }
  }

  if (timed_out) kill(pid, SIGKILL);
  close(pipefd[0]);
  int status = 0;
  waitpid(pid, &status, 0);
  return base::kOk;
} /*}}}*/

/**
 * @brief 把结果截断到字节上限（UTF-8 安全，避免切碎多字节字符），超限追加提示
 */
std::string Truncate(const std::string &in, size_t max_bytes) { /*{{{*/
  if (in.size() <= max_bytes) return in;
  size_t cut = max_bytes;
  while (cut > 0 && (static_cast<unsigned char>(in[cut]) & 0xC0) == 0x80) --cut;
  return in.substr(0, cut) + "\n...[结果过长已截断]";
} /*}}}*/

}  // namespace

ToolExecutor::ToolExecutor() : skills_(NULL) {}

ToolExecutor::~ToolExecutor() {}

void ToolExecutor::Init(const std::string &root_dir, const SkillRegistry *skills) { /*{{{*/
  skills_ = skills;
  schema_ = kToolsSchema;
  root_real_.clear();
  if (!root_dir.empty()) {
    char buf[PATH_MAX] = {0};
    if (realpath(root_dir.c_str(), buf) != NULL) {
      root_real_ = buf;
      while (!root_real_.empty() && root_real_[root_real_.size() - 1] == '/') root_real_.resize(root_real_.size() - 1);
    }
  }
} /*}}}*/

void ToolExecutor::SetBookToolFn(const BookToolFn &fn) { book_fn_ = fn; }

bool ToolExecutor::Enabled() const { return !root_real_.empty(); }

const std::string &ToolExecutor::ToolsSchemaJson() const { return schema_; }

bool ToolExecutor::ResolveInRoot(const std::string &path, std::string *real_out) { /*{{{*/
  if (root_real_.empty() || path.empty()) return false;
  std::string candidate = (path[0] == '/') ? path : (root_real_ + "/" + path);
  char buf[PATH_MAX] = {0};
  if (realpath(candidate.c_str(), buf) == NULL) return false;
  std::string real = buf;
  // 必须严格位于 root 之内（realpath 已展开 ../软链，故前缀校验即可防越权）
  if (real.size() <= root_real_.size() || real.compare(0, root_real_.size(), root_real_) != 0 ||
      real[root_real_.size()] != '/') {
    return false;
  }
  *real_out = real;
  return true;
} /*}}}*/

std::string ToolExecutor::RunSearchCode(const std::string &args_json) { /*{{{*/
  if (!Enabled()) return "search_code 不可用：根目录未配置";
  rapidjson::Document doc;
  doc.Parse(args_json.c_str(), args_json.size());
  if (doc.HasParseError() || !doc.IsObject()) return "search_code 参数非法";
  std::string query = GetJsonStr(doc, "query");
  std::string glob = GetJsonStr(doc, "glob");
  if (query.empty()) return "search_code 缺少 query";

  std::string grep = FindGrep();
  if (grep.empty()) return "search_code 不可用：系统未找到 grep";

  std::vector<std::string> argv;
  argv.push_back(grep);
  argv.push_back("-rInE");  // 递归/忽略二进制/行号/扩展正则
  argv.push_back("-m");
  argv.push_back(kSearchMaxPerFile);
  if (!glob.empty()) argv.push_back("--include=" + glob);
  argv.push_back("-e");
  argv.push_back(query);
  argv.push_back(".");

  std::string raw;
  base::Code ret = RunProcessCapture(argv, root_real_, kProcTimeoutMs, kSearchMaxBytes, &raw);
  if (ret != base::kOk) return "search_code 执行失败";
  if (raw.empty()) return "（无匹配）query=" + query + (glob.empty() ? "" : (" glob=" + glob));
  return Truncate("检索命中（相对仓库根，格式 文件:行号:内容）：\n" + raw, kResultMaxBytes);
} /*}}}*/

std::string ToolExecutor::RunReadFile(const std::string &args_json) { /*{{{*/
  if (!Enabled()) return "read_file 不可用：根目录未配置";
  rapidjson::Document doc;
  doc.Parse(args_json.c_str(), args_json.size());
  if (doc.HasParseError() || !doc.IsObject()) return "read_file 参数非法";
  std::string path = GetJsonStr(doc, "path");
  if (path.empty()) return "read_file 缺少 path";
  int start = GetJsonInt(doc, "start", 1);
  int end = GetJsonInt(doc, "end", 0);  // 0 表示到文件末尾（受行数上限约束）
  if (start < 1) start = 1;

  std::string real;
  if (!ResolveInRoot(path, &real)) return "read_file 拒绝：路径越权或不存在（仅允许仓库根目录内）path=" + path;

  std::ifstream ifs(real.c_str());
  if (!ifs.is_open()) return "read_file 打不开：" + path;

  std::string out = "文件 " + path + "（行号:内容）：\n";
  std::string line;
  int cur = 0;
  uint32_t emitted = 0;
  while (std::getline(ifs, line)) {
    ++cur;
    if (cur < start) continue;
    if (end > 0 && cur > end) break;
    if (emitted >= kReadMaxLines) {
      out += "...[超过行数上限已截断]\n";
      break;
    }
    char num[16] = {0};
    snprintf(num, sizeof(num), "%d| ", cur);
    out += num;
    out += line;
    out += "\n";
    ++emitted;
    if (out.size() >= kReadMaxBytes) {
      out += "...[超过字节上限已截断]\n";
      break;
    }
  }
  if (emitted == 0) return "read_file 指定范围无内容：" + path;
  return Truncate(out, kResultMaxBytes);
} /*}}}*/

std::string ToolExecutor::RunListSkills() { /*{{{*/
  if (skills_ == NULL) return "list_skills 不可用";
  std::vector<const Skill *> list;
  skills_->List(&list);
  if (list.empty()) return "（无已加载领域）";
  std::string out = "可用领域 Agent：\n";
  for (uint32_t i = 0; i < list.size(); ++i) {
    out += "- " + list[i]->name + "：" + list[i]->description + "\n";
  }
  return Truncate(out, kResultMaxBytes);
} /*}}}*/

std::string ToolExecutor::Run(const std::string &name, const std::string &args_json) { /*{{{*/
  if (name == "search_code") return RunSearchCode(args_json);
  if (name == "read_file") return RunReadFile(args_json);
  if (name == "list_skills") return RunListSkills();
  if (name == "list_books" || name == "get_book") {
    if (!book_fn_) return name + " 不可用：未注入业务库回调";
    std::string r = book_fn_(name, args_json);
    return Truncate(r, kResultMaxBytes);
  }
  return "未知工具：" + name;
} /*}}}*/

}  // namespace book_agent
