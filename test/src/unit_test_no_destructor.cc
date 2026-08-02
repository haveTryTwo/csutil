// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <map>
#include <string>
#include <vector>

#include "base/no_destructor.h"

#include "test_base/include/test_base.h"

// Helper class to track constructor and destructor calls
namespace {

static uint32_t g_construct_count = 0;
static uint32_t g_destruct_count = 0;

class TrackableObject {
 public:
  TrackableObject() : value_(0) { ++g_construct_count; }
  explicit TrackableObject(int32_t value) : value_(value) { ++g_construct_count; }
  TrackableObject(int32_t a, int32_t b) : value_(a + b) { ++g_construct_count; }
  ~TrackableObject() { ++g_destruct_count; }

  int32_t GetValue() const { return value_; }
  void SetValue(int32_t value) { value_ = value; }

 private:
  int32_t value_;
};

void ResetCounters() {
  g_construct_count = 0;
  g_destruct_count = 0;
}

}  // namespace

// === Normal Tests ===

TEST_D(NoDestructor, Test_Normal_BasicConstruct, "基本构造测试-默认构造函数") { /*{{{*/
  using namespace base;
  ResetCounters();

  {
    NoDestructor<TrackableObject> obj;
    EXPECT_EQ(g_construct_count, (uint32_t)1);
    EXPECT_EQ(obj->GetValue(), (int32_t)0);
  }

  // NoDestructor 析构后，内部对象的析构函数不应被调用
  EXPECT_EQ(g_destruct_count, (uint32_t)0);
  fprintf(stderr, "construct_count:%u, destruct_count:%u\n", g_construct_count, g_destruct_count);
} /*}}}*/

TEST_D(NoDestructor, Test_Normal_ConstructWithArgs, "带参数构造测试") { /*{{{*/
  using namespace base;
  ResetCounters();

  {
    NoDestructor<TrackableObject> obj(42);
    EXPECT_EQ(g_construct_count, (uint32_t)1);
    EXPECT_EQ(obj->GetValue(), (int32_t)42);
  }

  // 析构函数不应被调用
  EXPECT_EQ(g_destruct_count, (uint32_t)0);
  fprintf(stderr, "construct_count:%u, destruct_count:%u\n", g_construct_count, g_destruct_count);
} /*}}}*/

TEST_D(NoDestructor, Test_Normal_MultiArgs, "多参数构造测试") { /*{{{*/
  using namespace base;
  ResetCounters();

  {
    NoDestructor<TrackableObject> obj(10, 20);
    EXPECT_EQ(g_construct_count, (uint32_t)1);
    EXPECT_EQ(obj->GetValue(), (int32_t)30);
  }

  EXPECT_EQ(g_destruct_count, (uint32_t)0);
  fprintf(stderr, "construct_count:%u, destruct_count:%u\n", g_construct_count, g_destruct_count);
} /*}}}*/

TEST_D(NoDestructor, Test_Normal_StringType, "包装std::string类型测试") { /*{{{*/
  using namespace base;

  NoDestructor<std::string> str("hello world");
  EXPECT_EQ(*str, std::string("hello world"));
  EXPECT_EQ(str->size(), (uint32_t)11);
  EXPECT_EQ(str->empty(), false);

  fprintf(stderr, "str value: %s, size: %u\n", str->c_str(), (uint32_t)str->size());
} /*}}}*/

TEST_D(NoDestructor, Test_Normal_VectorType, "包装std::vector类型测试") { /*{{{*/
  using namespace base;

  NoDestructor<std::vector<int32_t>> vec({1, 2, 3, 4, 5});
  EXPECT_EQ(vec->size(), (uint32_t)5);
  EXPECT_EQ((*vec)[0], (int32_t)1);
  EXPECT_EQ((*vec)[4], (int32_t)5);

  // 可以修改内部对象
  vec->push_back(6);
  EXPECT_EQ(vec->size(), (uint32_t)6);
  EXPECT_EQ((*vec)[5], (int32_t)6);

  fprintf(stderr, "vector size: %u\n", (uint32_t)vec->size());
} /*}}}*/

TEST_D(NoDestructor, Test_Normal_MapType, "包装std::map类型测试") { /*{{{*/
  using namespace base;

  NoDestructor<std::map<int32_t, std::string>> mp(
      std::map<int32_t, std::string>({{1, "one"}, {2, "two"}, {3, "three"}}));
  EXPECT_EQ(mp->size(), (uint32_t)3);
  EXPECT_EQ(mp->at(1), std::string("one"));
  EXPECT_EQ(mp->at(2), std::string("two"));
  EXPECT_EQ(mp->at(3), std::string("three"));

  fprintf(stderr, "map size: %u\n", (uint32_t)mp->size());
} /*}}}*/

TEST_D(NoDestructor, Test_Normal_AccessOperators, "访问操作符测试") { /*{{{*/
  using namespace base;

  NoDestructor<std::string> str("test");

  // operator* 返回引用
  std::string& ref = *str;
  EXPECT_EQ(ref, std::string("test"));

  // operator-> 返回指针
  EXPECT_EQ(str->size(), (uint32_t)4);

  // Get() 返回指针
  std::string* ptr = str.Get();
  EXPECT_EQ(*ptr, std::string("test"));

  // const 访问
  const NoDestructor<std::string>& const_str = str;
  const std::string& const_ref = *const_str;
  EXPECT_EQ(const_ref, std::string("test"));
  EXPECT_EQ(const_str->size(), (uint32_t)4);

  const std::string* const_ptr = const_str.Get();
  EXPECT_EQ(*const_ptr, std::string("test"));

  fprintf(stderr, "access operators all passed\n");
} /*}}}*/

TEST_D(NoDestructor, Test_Normal_Mutable, "内部对象可修改测试") { /*{{{*/
  using namespace base;

  NoDestructor<std::string> str("hello");
  EXPECT_EQ(*str, std::string("hello"));

  // 通过引用修改
  *str = "world";
  EXPECT_EQ(*str, std::string("world"));

  // 通过指针修改
  str->append("!");
  EXPECT_EQ(*str, std::string("world!"));

  // 通过 Get() 修改
  str.Get()->clear();
  EXPECT_EQ(str->empty(), true);

  fprintf(stderr, "mutable test passed\n");
} /*}}}*/

TEST_D(NoDestructor, Test_Normal_StaticLocal, "函数局部静态变量使用场景测试") { /*{{{*/
  using namespace base;

  // 模拟典型使用场景: 函数局部 static NoDestructor
  struct Singleton {
    static const std::string& GetInstance() {
      static NoDestructor<std::string> instance("singleton_value");
      return *instance;
    }
  };

  const std::string& first_call = Singleton::GetInstance();
  const std::string& second_call = Singleton::GetInstance();

  // 两次调用应返回同一个对象
  EXPECT_EQ(&first_call, &second_call);
  EXPECT_EQ(first_call, std::string("singleton_value"));

  fprintf(stderr, "static local: %s, addr: %p\n", first_call.c_str(), (const void*)&first_call);
} /*}}}*/

// === Edge Case / Exception Tests ===

TEST_D(NoDestructor, Test_Exception_DestructorNotCalled, "析构函数不被调用验证") { /*{{{*/
  using namespace base;
  ResetCounters();

  // 在内部作用域创建并销毁 NoDestructor
  {
    NoDestructor<TrackableObject> obj1(100);
    NoDestructor<TrackableObject> obj2(200);
    EXPECT_EQ(g_construct_count, (uint32_t)2);
  }

  // 两个对象的析构函数都不应被调用
  EXPECT_EQ(g_destruct_count, (uint32_t)0);
  fprintf(stderr, "construct_count:%u, destruct_count:%u (should be 0)\n", g_construct_count, g_destruct_count);
} /*}}}*/

TEST_D(NoDestructor, Test_Exception_CompareWithRawObject, "与普通对象的析构对比测试") { /*{{{*/
  using namespace base;
  ResetCounters();

  // 普通对象: 析构函数会被调用
  {
    TrackableObject raw_obj(1);
    EXPECT_EQ(g_construct_count, (uint32_t)1);
  }
  EXPECT_EQ(g_destruct_count, (uint32_t)1);

  ResetCounters();

  // NoDestructor 包装: 析构函数不会被调用
  {
    NoDestructor<TrackableObject> nd_obj(1);
    EXPECT_EQ(g_construct_count, (uint32_t)1);
  }
  EXPECT_EQ(g_destruct_count, (uint32_t)0);

  fprintf(stderr, "raw object destruct called, NoDestructor destruct NOT called - verified\n");
} /*}}}*/

TEST_D(NoDestructor, Test_Exception_EmptyString, "空字符串构造测试") { /*{{{*/
  using namespace base;

  NoDestructor<std::string> empty_str("");
  EXPECT_EQ(empty_str->empty(), true);
  EXPECT_EQ(empty_str->size(), (uint32_t)0);
  EXPECT_EQ(*empty_str, std::string(""));

  NoDestructor<std::string> default_str;
  EXPECT_EQ(default_str->empty(), true);

  fprintf(stderr, "empty string test passed\n");
} /*}}}*/

TEST_D(NoDestructor, Test_Exception_EmptyContainer, "空容器构造测试") { /*{{{*/
  using namespace base;

  NoDestructor<std::vector<int32_t>> empty_vec;
  EXPECT_EQ(empty_vec->empty(), true);
  EXPECT_EQ(empty_vec->size(), (uint32_t)0);

  // 空容器仍可正常操作
  empty_vec->push_back(42);
  EXPECT_EQ(empty_vec->size(), (uint32_t)1);
  EXPECT_EQ((*empty_vec)[0], (int32_t)42);

  fprintf(stderr, "empty container test passed\n");
} /*}}}*/

TEST_D(NoDestructor, Test_Exception_LargeObject, "大对象构造测试") { /*{{{*/
  using namespace base;

  // 测试包含大量数据的对象
  struct LargeStruct {
    char data[4096];
    uint64_t id;
    LargeStruct() : id(12345) { memset(data, 'A', sizeof(data)); }
  };

  NoDestructor<LargeStruct> large_obj;
  EXPECT_EQ(large_obj->id, (uint64_t)12345);
  EXPECT_EQ(large_obj->data[0], 'A');
  EXPECT_EQ(large_obj->data[4095], 'A');

  fprintf(stderr, "large object (size=%u bytes) test passed\n", (uint32_t)sizeof(LargeStruct));
} /*}}}*/

TEST_D(NoDestructor, Test_Exception_PodType, "POD类型测试") { /*{{{*/
  using namespace base;

  // 测试基本数据类型
  NoDestructor<int32_t> int_val(42);
  EXPECT_EQ(*int_val, (int32_t)42);

  NoDestructor<double> double_val(3.14);
  EXPECT_TRUE(*double_val > 3.13);
  EXPECT_TRUE(*double_val < 3.15);

  NoDestructor<uint64_t> uint_val(UINT64_MAX);
  EXPECT_EQ(*uint_val, UINT64_MAX);

  fprintf(stderr, "POD type test passed: int=%d, double=%f, uint64=%llu\n", *int_val, *double_val,
          (unsigned long long)*uint_val);
} /*}}}*/

// === Performance / Stress Tests ===

TEST_D(NoDestructor, Test_Press_RepeatedAccess, "重复访问压测") { /*{{{*/
  using namespace base;

  NoDestructor<std::string> str("stress_test_value");

  uint32_t iterations = 1000000;
  uint32_t check_sum = 0;
  for (uint32_t i = 0; i < iterations; ++i) {
    check_sum += (uint32_t)str->size();
  }

  EXPECT_EQ(check_sum, iterations * (uint32_t)17);
  fprintf(stderr, "repeated access %u iterations passed, check_sum:%u\n", iterations, check_sum);
} /*}}}*/

TEST_D(NoDestructor, Test_Press_ManyObjects, "大量对象创建压测") { /*{{{*/
  using namespace base;
  ResetCounters();

  const uint32_t count = 10000;
  // 使用 vector 存放指向 NoDestructor 的指针来模拟大量创建
  std::vector<NoDestructor<TrackableObject>*> objects;
  objects.reserve(count);

  for (uint32_t i = 0; i < count; ++i) {
    // 使用 placement new 在堆上创建 NoDestructor
    void* mem = ::operator new(sizeof(NoDestructor<TrackableObject>));
    auto* nd = new (mem) NoDestructor<TrackableObject>((int32_t)i);
    objects.push_back(nd);
  }

  EXPECT_EQ(g_construct_count, count);

  // 验证每个对象的值
  for (uint32_t i = 0; i < count; ++i) {
    EXPECT_EQ(objects[i]->Get()->GetValue(), (int32_t)i);
  }

  // 显式销毁 NoDestructor (但内部对象的析构不会被调用)
  for (uint32_t i = 0; i < count; ++i) {
    objects[i]->~NoDestructor();
    ::operator delete(objects[i]);
  }

  // TrackableObject 的析构函数不应被调用
  EXPECT_EQ(g_destruct_count, (uint32_t)0);
  fprintf(stderr, "created %u objects, construct_count:%u, destruct_count:%u\n", count, g_construct_count,
          g_destruct_count);
} /*}}}*/

TEST_D(NoDestructor, Test_Press_StaticSingleton, "静态单例并发安全性压测") { /*{{{*/
  using namespace base;

  // 模拟多次获取单例的场景
  struct Config {
    static const std::vector<std::string>& GetItems() {
      static NoDestructor<std::vector<std::string>> items(std::vector<std::string>{"item1", "item2", "item3"});
      return *items;
    }
  };

  uint32_t iterations = 100000;
  for (uint32_t i = 0; i < iterations; ++i) {
    const std::vector<std::string>& items = Config::GetItems();
    EXPECT_EQ(items.size(), (uint32_t)3);
    EXPECT_EQ(items[0], std::string("item1"));
  }

  fprintf(stderr, "static singleton access %u iterations passed\n", iterations);
} /*}}}*/
