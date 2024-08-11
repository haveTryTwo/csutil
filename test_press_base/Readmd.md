# 背景
当业务完成服务开发测试后，期望获取服务的性能，则除了构建服务自身的压测代码，还需要构建压测框架，成本很高。

# 目标
提供一套通用的压测框架，方便对需要的服务进行压测

# 使用
## 1、压测代码
首先，创建一个继承 `PressObject`的类，在这个类中即实现服务的压测代码；
```C++
class PressXXXObject : public test::PressObject {
 public:
  PressXXXObject(const std::string &test_name) : test::PressObject(test_name) {}

  virtual ~PressXXXObject() {}

 public:
  virtual test::PressObject *Create();

  virtual base::Code Init(const std::string &dst_ip_port_protos);
  virtual base::Code ExecBody();
  virtual bool IsOver();
};

// NOTE: this is vary import! So register a press object
Register(PressXXX, PressXXXObject);

test::PressObject *PressXXXObject::Create() {
  return new PressXXXObject(*this); // 可以不用调整
}

base::Code PressXXXObject::Init(const std::string &dst_ip_port_protos) { // 可不用调整
  base::Code ret = test::PressObject::Init(dst_ip_port_protos);
  if (ret != base::kOk) return ret;
  return base::kOk;
}

bool PressXXXObject::IsOver() { 
  // 实现压测代结束逻辑 
}

base::Code PressXXXObject::ExecBody() { /*{{{*/
  if (IsOver()) {
    return base::kExitOk;
  }

  test::BusiClient *busi_client = GetBusiClient(test::kHttpStr);
  if (busi_client == NULL) return base::kInvalidParam;
  std::string resp;
  std::string relative_url = "/";
  base::Code ret = busi_client->SendAndRecv(relative_url, "", &resp);

  return ret;
} /*}}}*/
```
自定义类中，重点实现`ExecBody()`，即实际的压测逻辑，其他代码只要按样例模板复制即可。
说明：IsOver()是判断压测是否结束的逻辑，可以简单实用计数实现；
关于调用的协议，目前有`Http/rpc`可以直接使用，如果有其他协议，可以继承BusiClient实现即可。

## 2、执行
完成压测方法的实现后，即可以执行，例如样例：
```shell
./press_test -p '127.0.0.1:80:http,127.0.0.1:9090:rpc' -n 10 -m 1000 -x 0 -c PressXXX
```
