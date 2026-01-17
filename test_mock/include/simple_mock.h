// Copyright (c) 2025 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Simple Mock Framework - A lightweight C++ mocking library
// Inspired by Google Mock but completely independent implementation
//
// Supported features:
// - MOCK_METHOD: Define mock methods
// - EXPECT_CALL: Set expectations for method calls
// - WillOnce/WillRepeatedly: Set return values
// - Return: Specify return value
// - Times: Verify call count
// - Parameter matching: _ (wildcard) and exact match

#ifndef TEST_MOCK_INCLUDE_SIMPLE_MOCK_H_
#define TEST_MOCK_INCLUDE_SIMPLE_MOCK_H_

#include <cstdint>
#include <functional>
#include <memory>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace simple_mock {

// ============================================================================
// Forward declarations
// ============================================================================
template <typename R, typename... Args>
class MockFunction;

template <typename R, typename... Args>
class MockExpectation;

// ============================================================================
// Cardinality - 调用次数控制
// ============================================================================

/**
 * @brief Cardinality 表示期望的调用次数范围
 */
class Cardinality {
 public:
  /**
   * @brief 构造精确次数的 Cardinality
   * @param exact 精确的调用次数
   */
  explicit Cardinality(int32_t exact) : min_call_(exact), max_call_(exact) {}

  /**
   * @brief 构造范围次数的 Cardinality
   * @param min 最小调用次数
   * @param max 最大调用次数，-1 表示无限制
   */
  Cardinality(int32_t min, int32_t max) : min_call_(min), max_call_(max) {}

  /**
   * @brief 检查调用次数是否满足期望
   * @param call_count 实际调用次数
   * @return 是否满足期望
   */
  bool IsSatisfied(int32_t call_count) const {
    return call_count >= min_call_ && (max_call_ == -1 || call_count <= max_call_);
  }

  /**
   * @brief 检查调用次数是否已达到上限
   * @param call_count 实际调用次数
   * @return 是否已饱和
   */
  /**
   * @brief 检查调用次数是否已达到上限
   * @param call_count 实际调用次数
   * @return 是否已饱和
   * 
   * Note: Returns true only if call_count EXCEEDS max_call_, not when it equals max_call_.
   * If max_call_ is 1 and call_count is 1, that's the expected call, not exceeding.
   */
  bool IsSaturated(int32_t call_count) const {
    return max_call_ != -1 && call_count > max_call_;
  }

  int32_t GetMin() const { return min_call_; }
  int32_t GetMax() const { return max_call_; }

 private:
  int32_t min_call_;
  int32_t max_call_;
};

// Cardinality helper functions
inline Cardinality Times(int32_t n) { return Cardinality(n); }
inline Cardinality AtLeast(int32_t n) { return Cardinality(n, -1); }
inline Cardinality AtMost(int32_t n) { return Cardinality(0, n); }
inline Cardinality AnyNumber() { return Cardinality(0, -1); }

// ============================================================================
// Matcher - 参数匹配器
// ============================================================================

// Forward declaration
template <typename T>
class Matcher;

/**
 * @brief AnythingMatcher 匹配任意值（通配符 _）
 * Can be implicitly converted to Matcher<T> for any T
 */
struct AnythingMatcher {
  // Allow implicit conversion to Matcher<T> for any type T
  template <typename T>
  operator Matcher<T>() const {
    return Matcher<T>();  // Default constructor creates wildcard matcher
  }
};

// 全局通配符对象
static const AnythingMatcher _ = AnythingMatcher();

/**
 * @brief EqMatcher 精确匹配指定值
 */
template <typename T>
class EqMatcher {
 public:
  explicit EqMatcher(const T& expected) : expected_(expected) {}

  bool Matches(const T& actual) const { return actual == expected_; }

 private:
  T expected_;
};

/**
 * @brief Matcher 通用匹配器包装类
 * 
 * Note: Uses std::remove_reference to store value types, not references
 */
template <typename T>
class Matcher {
 public:
  using ValueType = typename std::remove_const<typename std::remove_reference<T>::type>::type;

  // Constructor for anything matcher
  Matcher() : is_anything_(true), expected_() {}

  // Constructor for exact match
  explicit Matcher(const T& value) : is_anything_(false), expected_(value) {}

  bool Matches(const T& value) const {
    if (is_anything_) {
      return true;
    }
    return expected_ == value;
  }

 private:
  bool is_anything_;
  ValueType expected_;
};

// ============================================================================
// Action - 行为定义
// ============================================================================

/**
 * @brief ActionInterface Action 的基类接口
 */
template <typename R>
class ActionInterface {
 public:
  virtual ~ActionInterface() = default;
  virtual R Perform() const = 0;
};

/**
 * @brief ReturnAction 返回指定值的 Action
 */
template <typename R>
class ReturnAction : public ActionInterface<R> {
 public:
  explicit ReturnAction(const R& value) : value_(value) {}

  R Perform() const override { return value_; }

 private:
  R value_;
};

/**
 * @brief void 返回类型的特化
 */
template <>
class ReturnAction<void> : public ActionInterface<void> {
 public:
  ReturnAction() = default;

  void Perform() const override {}
};

/**
 * @brief Action 包装类
 */
template <typename R>
class Action {
 public:
  Action() = default;

  explicit Action(std::shared_ptr<ActionInterface<R>> impl) : impl_(impl) {}

  R Perform() const {
    if (impl_) {
      return impl_->Perform();
    }
    return R{};
  }

 private:
  std::shared_ptr<ActionInterface<R>> impl_;
};

/**
 * @brief void 类型的 Action 特化
 */
template <>
class Action<void> {
 public:
  Action() = default;

  explicit Action(std::shared_ptr<ActionInterface<void>> impl) : impl_(impl) {}

  void Perform() const {
    if (impl_) {
      impl_->Perform();
    }
  }

 private:
  std::shared_ptr<ActionInterface<void>> impl_;
};

// Return helper function
template <typename R>
Action<R> Return(const R& value) {
  return Action<R>(std::make_shared<ReturnAction<R>>(value));
}

inline Action<void> Return() { return Action<void>(std::make_shared<ReturnAction<void>>()); }

// ============================================================================
// MockExpectation - 期望调用管理
// ============================================================================

/**
 * @brief MockExpectation 管理单个期望调用的所有信息
 */
template <typename R, typename... Args>
class MockExpectation {
 public:
  MockExpectation()
      : has_matchers_(false), repeated_action_set_(false), call_count_(0), cardinality_(1), 
        times_explicitly_set_(false) {}

  /**
   * @brief 设置期望的参数匹配器
   */
  MockExpectation& WithArgs(const Matcher<Args>&... matchers) {
    matchers_ = std::make_tuple(matchers...);
    has_matchers_ = true;
    return *this;
  }

  /**
   * @brief 设置期望的参数匹配器（接受通用参数类型）
   * This overload allows passing AnythingMatcher (_) directly
   */
  template <typename... MatcherArgs>
  MockExpectation& WithArgsHelper(MatcherArgs&&... args) {
    CreateMatchers(std::index_sequence_for<Args...>{}, std::forward<MatcherArgs>(args)...);
    has_matchers_ = true;
    return *this;
  }

 private:
  // Helper to create Matchers from arguments (handles both regular values and AnythingMatcher)
  template <std::size_t... Is, typename... MatcherArgs>
  void CreateMatchers(std::index_sequence<Is...>, MatcherArgs&&... args) {
    matchers_ = std::make_tuple(CreateMatcherForIndex<Is>(std::forward<MatcherArgs>(args))...);
  }

  // Helper to create Matcher for a specific index
  template <std::size_t I, typename ArgType>
  auto CreateMatcherForIndex(ArgType&& arg) {
    using TargetType = std::tuple_element_t<I, std::tuple<Args...>>;
    if constexpr (std::is_same_v<std::decay_t<ArgType>, AnythingMatcher>) {
      return Matcher<TargetType>();  // Wildcard matcher
    } else {
      return Matcher<TargetType>(std::forward<ArgType>(arg));  // Exact match
    }
  }

 public:

  /**
   * @brief 设置单次调用的返回行为
   */
  MockExpectation& WillOnce(const Action<R>& action) {
    actions_.push_back(action);
    // Update cardinality to match the number of WillOnce calls
    // Only if user hasn't explicitly set Times()
    // If user calls WillOnce multiple times, they expect that many calls
    if (!times_explicitly_set_) {
      int32_t expected_calls = static_cast<int32_t>(actions_.size());
      cardinality_ = Cardinality(expected_calls);
    }
    return *this;
  }

  /**
   * @brief 设置重复调用的返回行为
   */
  MockExpectation& WillRepeatedly(const Action<R>& action) {
    repeated_action_ = action;
    repeated_action_set_ = true;
    return *this;
  }

  /**
   * @brief 设置期望的调用次数
   */
  MockExpectation& Times(const Cardinality& cardinality) {
    cardinality_ = cardinality;
    times_explicitly_set_ = true;  // Mark that user explicitly set Times
    return *this;
  }

  /**
   * @brief 检查参数是否匹配
   */
  bool Matches(const Args&... args) const {
    if (!has_matchers_) {
      return true;  // 没有设置匹配器，匹配所有调用
    }
    return MatchesImpl(std::index_sequence_for<Args...>{}, args...);
  }

  /**
   * @brief 获取下一个要执行的 Action
   */
  Action<R> GetNextAction() {
    if (!actions_.empty()) {
      Action<R> action = actions_.front();
      actions_.erase(actions_.begin());
      return action;
    }
    if (repeated_action_set_) {
      return repeated_action_;
    }
    return Action<R>();  // 返回默认 Action
  }

  /**
   * @brief 增加调用计数
   */
  void IncrementCallCount() { ++call_count_; }

  /**
   * @brief 获取调用计数
   */
  int32_t GetCallCount() const { return call_count_; }

  /**
   * @brief 检查是否已达到调用次数上限
   */
  bool IsSaturated() const { return cardinality_.IsSaturated(call_count_); }

  /**
   * @brief 验证调用次数是否满足期望
   */
  bool Verify() const { return cardinality_.IsSatisfied(call_count_); }

  /**
   * @brief 获取 Cardinality 用于错误报告
   */
  const Cardinality& GetCardinality() const { return cardinality_; }

 private:
  template <std::size_t... Is>
  bool MatchesImpl(std::index_sequence<Is...>, const Args&... args) const {
    std::tuple<Args...> args_tuple(args...);
    // 使用 fold expression (C++17)
    return (std::get<Is>(matchers_).Matches(std::get<Is>(args_tuple)) && ...);
  }

  std::tuple<Matcher<Args>...> matchers_;  // 参数匹配器
  bool has_matchers_;  // 是否设置了参数匹配器

  std::vector<Action<R>> actions_;  // 行为队列
  Action<R> repeated_action_;  // 重复行为
  bool repeated_action_set_;  // 是否设置了重复行为

  int32_t call_count_;  // 调用计数
  Cardinality cardinality_;  // 调用次数控制
  bool times_explicitly_set_;  // Track if user explicitly called Times()
};

// ============================================================================
// MockFunction - Mock 函数对象
// ============================================================================

/**
 * @brief MockFunction 管理某个函数的所有期望调用
 */
template <typename R, typename... Args>
class MockFunction {
 public:
  MockFunction() : current_expectation_(nullptr), method_name_("unknown") {}

  /**
   * @brief 设置方法名（用于错误报告）
   */
  void SetMethodName(const std::string& name) { method_name_ = name; }

  /**
   * @brief 添加一个新的期望调用
   */
  MockExpectation<R, Args...>& AddExpectation() {
    auto exp = std::make_shared<MockExpectation<R, Args...>>();
    expectations_.push_back(exp);
    current_expectation_ = exp.get();
    return *exp;
  }

  /**
   * @brief 实际调用入口
   */
  R Invoke(const Args&... args) {
    // 查找匹配的 expectation
    for (std::shared_ptr<MockExpectation<R, Args...>>& exp : expectations_) {
      if (exp->Matches(args...)) {
        exp->IncrementCallCount();

        // 检查是否超过调用次数
        if (exp->IsSaturated()) {
          ReportError("Mock function called more times than expected");
        }

        // 获取并执行 action
        Action<R> action = exp->GetNextAction();
        return action.Perform();
      }
    }

    // 没有找到匹配的 expectation
    ReportError("Unexpected call to mock function");
    return R{};
  }

  /**
   * @brief 析构时验证所有期望是否满足
   */
  ~MockFunction() { VerifyExpectations(); }

  /**
   * @brief 手动验证所有期望
   */
  void VerifyExpectations() {
    for (const std::shared_ptr<MockExpectation<R, Args...>>& exp : expectations_) {
      if (!exp->Verify()) {
        std::ostringstream oss;
        oss << "Mock function '" << method_name_ << "' expected to be called ";
        const Cardinality& card = exp->GetCardinality();
        if (card.GetMin() == card.GetMax()) {
          oss << card.GetMin();
        } else if (card.GetMax() == -1) {
          oss << "at least " << card.GetMin();
        } else {
          oss << "between " << card.GetMin() << " and " << card.GetMax();
        }
        oss << " times, but called " << exp->GetCallCount() << " times";
        ReportError(oss.str());
      }
    }
  }

 private:
  void ReportError(const std::string& message) const {
    fprintf(stderr, "[MOCK ERROR] %s (function: %s)\n", message.c_str(), method_name_.c_str());
  }

  std::vector<std::shared_ptr<MockExpectation<R, Args...>>> expectations_;  // 期望队列
  MockExpectation<R, Args...>* current_expectation_;  // 当前期望
  std::string method_name_;  // 方法名
};

// ============================================================================
// void 返回类型的特化
// ============================================================================

template <typename... Args>
class MockFunction<void, Args...> {
 public:
  MockFunction() : current_expectation_(nullptr), method_name_("unknown") {}

  void SetMethodName(const std::string& name) { method_name_ = name; }

  MockExpectation<void, Args...>& AddExpectation() {
    auto exp = std::make_shared<MockExpectation<void, Args...>>();
    expectations_.push_back(exp);
    current_expectation_ = exp.get();
    return *exp;
  }

  void Invoke(const Args&... args) {
    for (std::shared_ptr<MockExpectation<void, Args...>>& exp : expectations_) {
      if (exp->Matches(args...)) {
        exp->IncrementCallCount();

        if (exp->IsSaturated()) {
          ReportError("Mock function called more times than expected");
        }

        Action<void> action = exp->GetNextAction();
        action.Perform();
        return;
      }
    }

    ReportError("Unexpected call to mock function");
  }

  ~MockFunction() { VerifyExpectations(); }

  void VerifyExpectations() {
    for (const std::shared_ptr<MockExpectation<void, Args...>>& exp : expectations_) {
      if (!exp->Verify()) {  // 验证期望是否满足预期
        std::ostringstream oss;
        oss << "Mock function '" << method_name_ << "' expected to be called ";
        const Cardinality& card = exp->GetCardinality();
        if (card.GetMin() == card.GetMax()) {
          oss << card.GetMin();
        } else if (card.GetMax() == -1) {
          oss << "at least " << card.GetMin();
        } else {
          oss << "between " << card.GetMin() << " and " << card.GetMax();
        }
        oss << " times, but called " << exp->GetCallCount() << " times";
        ReportError(oss.str());
      }
    }
  }

 private:
  void ReportError(const std::string& message) const {
    fprintf(stderr, "[MOCK ERROR] %s (function: %s)\n", message.c_str(), method_name_.c_str());
  }

  std::vector<std::shared_ptr<MockExpectation<void, Args...>>> expectations_;  // 期望队列
  MockExpectation<void, Args...>* current_expectation_;  // 当前期望
  std::string method_name_;  // 方法名
};

// ============================================================================
// Internal helpers for macro implementation
// ============================================================================

namespace internal {

// Helper to extract parameter types from function signature
template <typename T>
struct ArgumentTuple;  // 参数类型提取

template <typename R, typename... Args>
struct ArgumentTuple<R(Args...)> {
  using type = std::tuple<Args...>;  // 参数类型
};

// Helper for creating matchers - exact match
template <typename T>
Matcher<T> MakeMatcher(const T& value) {
  return Matcher<T>(value);  // 创建精确匹配的匹配器
}

// Special overload for AnythingMatcher - creates wildcard matcher
// This overload is selected when _ (AnythingMatcher) is passed
// We need to explicitly specify the target type T
template <typename T>
Matcher<T> MakeMatcher(const AnythingMatcher&) {
  return Matcher<T>();  // Default constructor creates wildcard matcher
}


}  // namespace internal

// ============================================================================
// Macros for defining mock methods
// ============================================================================

// MOCK_METHOD0-4 macros for different number of arguments
// Usage: MOCK_METHOD2(bool, connect, const std::string&, int)

// 0 arguments
#define MOCK_METHOD0(return_type, method_name)                   \
  return_type method_name() override {                           \
    return gmock_##method_name.Invoke();                         \
  }                                                              \
  simple_mock::MockFunction<return_type> gmock_##method_name

// 1 argument
#define MOCK_METHOD1(return_type, method_name, arg1_type)         \
  return_type method_name(arg1_type arg1) override {              \
    return gmock_##method_name.Invoke(arg1);                      \
  }                                                               \
  simple_mock::MockFunction<return_type, arg1_type> gmock_##method_name

// 2 arguments
#define MOCK_METHOD2(return_type, method_name, arg1_type, arg2_type) \
  return_type method_name(arg1_type arg1, arg2_type arg2) override { \
    return gmock_##method_name.Invoke(arg1, arg2);                   \
  }                                                                  \
  simple_mock::MockFunction<return_type, arg1_type, arg2_type> gmock_##method_name

// 3 arguments
#define MOCK_METHOD3(return_type, method_name, arg1_type, arg2_type, arg3_type)   \
  return_type method_name(arg1_type arg1, arg2_type arg2, arg3_type arg3) override { \
    return gmock_##method_name.Invoke(arg1, arg2, arg3);                             \
  }                                                                                  \
  simple_mock::MockFunction<return_type, arg1_type, arg2_type, arg3_type> gmock_##method_name

// 4 arguments
#define MOCK_METHOD4(return_type, method_name, arg1_type, arg2_type, arg3_type, arg4_type) \
  return_type method_name(arg1_type arg1, arg2_type arg2, arg3_type arg3,                  \
                          arg4_type arg4) override {                                        \
    return gmock_##method_name.Invoke(arg1, arg2, arg3, arg4);                             \
  }                                                                                         \
  simple_mock::MockFunction<return_type, arg1_type, arg2_type, arg3_type, arg4_type>      \
      gmock_##method_name

// For compatibility with Google Mock style
// Usage: MOCK_METHOD(ReturnType, MethodName, (Arg1Type, Arg2Type), (override))
// Note: This is more complex and uses simpler numbered macros internally
#define MOCK_METHOD(...) MOCK_METHOD_NOT_IMPLEMENTED_USE_MOCK_METHOD0_TO_4

// ============================================================================
// EXPECT_CALL macro
// ============================================================================

// EXPECT_CALL macros - separate versions for different argument counts
// Usage: EXPECT_CALL0(mock, method_name).WillOnce(Return(value))
//        EXPECT_CALL1(mock, method_name, arg1).WillOnce(Return(value))
//        EXPECT_CALL2(mock, method_name, arg1, arg2).WillOnce(Return(value))

#define EXPECT_CALL0(mock_ptr, method_name)                                           \
  mock_ptr->gmock_##method_name.SetMethodName(#method_name),                          \
      mock_ptr->gmock_##method_name.AddExpectation()

#define EXPECT_CALL1(mock_ptr, method_name, a1)                                       \
  mock_ptr->gmock_##method_name.SetMethodName(#method_name),                          \
      mock_ptr->gmock_##method_name.AddExpectation().WithArgsHelper(GMOCK_MAKE_MATCHER_1(a1))

#define EXPECT_CALL2(mock_ptr, method_name, a1, a2)                                   \
  mock_ptr->gmock_##method_name.SetMethodName(#method_name),                          \
      mock_ptr->gmock_##method_name.AddExpectation().WithArgsHelper(GMOCK_MAKE_MATCHER_2(a1, a2))

#define EXPECT_CALL3(mock_ptr, method_name, a1, a2, a3)                               \
  mock_ptr->gmock_##method_name.SetMethodName(#method_name),                          \
      mock_ptr->gmock_##method_name.AddExpectation().WithArgsHelper(GMOCK_MAKE_MATCHER_3(a1, a2, a3))

#define EXPECT_CALL4(mock_ptr, method_name, a1, a2, a3, a4)                           \
  mock_ptr->gmock_##method_name.SetMethodName(#method_name),                          \
      mock_ptr->gmock_##method_name.AddExpectation().WithArgsHelper(GMOCK_MAKE_MATCHER_4(a1, a2, a3, a4))

// Default EXPECT_CALL (for backward compatibility, requires manual WithArgs)
#define EXPECT_CALL(mock_ptr, method_name)                                            \
  mock_ptr->gmock_##method_name.SetMethodName(#method_name),                          \
      mock_ptr->gmock_##method_name.AddExpectation()

// Helper to create matchers from arguments
// Use WithArgsHelper which can handle both regular values and AnythingMatcher
#define GMOCK_MAKE_MATCHER_1(a1) a1
#define GMOCK_MAKE_MATCHER_2(a1, a2) a1, a2
#define GMOCK_MAKE_MATCHER_3(a1, a2, a3) a1, a2, a3
#define GMOCK_MAKE_MATCHER_4(a1, a2, a3, a4) a1, a2, a3, a4

}  // namespace simple_mock

#endif  // TEST_MOCK_INCLUDE_SIMPLE_MOCK_H_

