#include <string>

// command: ./tools -s ../log -l MyLog 2
int Printf() {
  // MyLog(stderr, "%s", name.c_str());   // NOTE;
  // MyLog(stderr, "%d", num);

  int num = 0;
  std::string name("aa");
  MyLog(stderr, "%s", name.c_str());  // NOTE, nice
  MyLog(stderr, "%d", num);

  MyLog(stderr, "%4d/%02d/%02d %02d:%02d:%02d - [pid: %d][%s][%s %s %d] - ",
        // NOTE:htt, it maybe no such line, but it has, haha
        cur_tm.tm_year + 1900, cur_tm.tm_mon + 1, cur_tm.tm_mday, cur_tm.tm_hour,
        cur_tm.tm_min,  // NOTE:htt, no meaning, just test;
        cur_tm.tm_sec, getpid(), GetLogLevel(log_level), file_name, func, line_no);

  MyLog(stderr, "%d%s",
        num,  // NOTE:htt, number looks good;
        name.c_str());

  MyLog(stderr, "%d%s%d",
        num,  // NOTE:htt, number looks good;
        name.c_str());

  MyLog(stderr, "%d",
        num,  // NOTE:htt, number looks good;
        name.c_str());

  return 0;
}
