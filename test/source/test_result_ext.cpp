#include <gtest/gtest.h>
#include <safe-containers/error.h>
#include <safe-containers/result/result_ext.h>

template <typename T>
using result = cpp::result<T, ContainerError>;

result<void> fail() { return cpp::fail(ContainerError{}); }

enum class MyResult
{
    Success,
    Failure,
};

template <>
MyResult result_ext::ConvertError(ContainerError&& error)
{
    std::ignore = error;
    return MyResult::Failure;
}

template <>
void result_ext::IntoContextError(ContainerError&& error)
{
    std::ignore = error;
}

template <typename Ret, typename F>
Ret WrapFunc(F&& f)
{
    return f();
}

TEST(ResultExt, AllocationFailuresReturnErrorWithCustomError)
{
    const MyResult res = WrapFunc<MyResult>(
        [&]() -> MyResult
        {
            TRY_CHECK(fail());
            return MyResult::Success;
        });
    ASSERT_EQ(res, MyResult::Failure);
}

TEST(ResultExt, FailuresCoerceToBool)
{
    const bool res = WrapFunc<bool>(
        [&]() -> bool
        {
            TRY_CHECK(fail());
            return true;
        });
    ASSERT_FALSE(res);
}

TEST(ResultExt, FailuresCoerceToNullptr)
{
    int a = 42;
    const int* res = WrapFunc<int*>(
        [&]() -> int*
        {
            TRY_CHECK(fail());
            return &a;
        });
    ASSERT_EQ(res, nullptr);
}

TEST(ResultExt, FailuresCoerceToErrorType)
{
    const ContainerError res = WrapFunc<ContainerError>(
        [&]() -> ContainerError
        {
            TRY_CHECK(fail());
            return {};
        });
    std::ignore = res;
}

TEST(ResultExt, FailuresCoerceToInt)
{
    const int res = WrapFunc<int>(
        [&]() -> int
        {
            TRY_CHECK(fail());
            return 42;
        });
    ASSERT_EQ(res, 0);
}
