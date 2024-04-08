#include <gtest/gtest.h>
#include <safe-containers/vector.h>

#include "fail_alloc.h"

using int_vec = safe_containers::vector<int, std::allocator<int>>;
using int_allocator = int_vec::allocator_type;

struct NotCopyable
{
    NotCopyable() = default;
    NotCopyable(NotCopyable&) = delete;
    NotCopyable(const NotCopyable&) = delete;
    NotCopyable& operator=(NotCopyable&) = delete;
    NotCopyable& operator=(const NotCopyable&) = delete;
    NotCopyable(NotCopyable&&) = default;
    NotCopyable& operator=(NotCopyable&&) = default;
    ~NotCopyable() = default;
};

using not_copyable_vec = safe_containers::vector<NotCopyable, std::allocator<NotCopyable>>;
using not_copyable_allocator = not_copyable_vec::allocator_type;

TEST(SafeVec, DefaultCtorWithStdAllocator)
{
    int_allocator alloc{};
    int_vec v{alloc};
    ASSERT_TRUE(v.empty());
    ASSERT_EQ(v.capacity(), 0);
}

TEST(SafeVec, Ctor)
{
    int_allocator alloc{};
    const int_vec v{alloc};
    ASSERT_TRUE(v.empty());
    ASSERT_EQ(v.capacity(), 0);
}

TEST(SafeVec, Create)
{
    int_allocator alloc{};
    auto result = int_vec::Create(alloc);
    ASSERT_TRUE(result.has_value());
    ASSERT_TRUE(result.value().empty());
    ASSERT_EQ(result.value().capacity(), 0);
}

TEST(SafeVec, CreateFilled)
{
    int_allocator alloc{};
    constexpr int val = 42;
    auto result = int_vec::Create(static_cast<size_t>(3), val, alloc);
    ASSERT_TRUE(result.has_value());
    ASSERT_FALSE(result.value().empty());
    ASSERT_EQ(result.value().size(), 3);
    ASSERT_EQ(result.value()[0], val);
    ASSERT_EQ(result.value()[1], val);
    ASSERT_EQ(result.value()[2], val);
}

TEST(SafeVec, CreateSizedDefault)
{
    int_allocator alloc{};
    auto result = int_vec::Create(3, alloc);
    ASSERT_TRUE(result.has_value());
    ASSERT_FALSE(result.value().empty());
    ASSERT_EQ(result.value().size(), 3);
    ASSERT_EQ(result.value()[0], 0);
    ASSERT_EQ(result.value()[1], 0);
    ASSERT_EQ(result.value()[2], 0);
}

TEST(SafeVec, CreateIterRange)
{
    int_allocator alloc{};
    std::vector<int> vec{1, 2, 3};
    auto result = int_vec::Create(vec.begin(), vec.end(), alloc);
    ASSERT_TRUE(result.has_value());
    ASSERT_FALSE(result.value().empty());
    ASSERT_EQ(result.value().size(), 3);
    ASSERT_EQ(result.value()[0], 1);
    ASSERT_EQ(result.value()[1], 2);
    ASSERT_EQ(result.value()[2], 3);
}

TEST(SafeVec, CreateInitializerList)
{
    int_allocator alloc{};
    auto result = int_vec::Create({1, 2, 3}, alloc);
    ASSERT_TRUE(result.has_value());
    ASSERT_FALSE(result.value().empty());
    ASSERT_EQ(result.value().size(), 3);
    ASSERT_EQ(result.value()[0], 1);
    ASSERT_EQ(result.value()[1], 2);
    ASSERT_EQ(result.value()[2], 3);
}

TEST(SafeVec, PushBack)
{
    {
        int_allocator alloc{};
        int_vec v{alloc};
        v.push_back(1).expect("push_back should work");
        ASSERT_EQ(v.size(), 1);
        ASSERT_EQ(v[0], 1);
        ASSERT_GE(v.capacity(), 1);
    }

    {
        not_copyable_allocator alloc{};
        not_copyable_vec v{alloc};
        v.push_back(NotCopyable()).expect("push_back should work");
        ASSERT_EQ(v.size(), 1);
        ASSERT_GE(v.capacity(), 1);
    }

    {
        not_copyable_allocator alloc{};
        not_copyable_vec v{alloc};
        NotCopyable a;
        v.push_back(std::move(a)).expect("push_back should work");
        ASSERT_EQ(v.size(), 1);
        ASSERT_GE(v.capacity(), 1);
    }
}

TEST(SafeVec, Emplace)
{
    {
        int_allocator alloc{};
        int_vec v{alloc};
        v.emplace(v.cbegin(), 1).expect("emplace should work");
        ASSERT_EQ(v.size(), 1);
        ASSERT_EQ(v[0], 1);
        ASSERT_GE(v.capacity(), 1);
    }

    {
        not_copyable_allocator alloc{};
        not_copyable_vec v{alloc};
        v.emplace(v.cbegin(), NotCopyable()).expect("emplace should work");
        ASSERT_EQ(v.size(), 1);
        ASSERT_GE(v.capacity(), 1);
    }

    {
        not_copyable_allocator alloc{};
        not_copyable_vec v{alloc};
        NotCopyable a;
        v.emplace(v.cbegin(), std::move(a)).expect("emplace should work");
        ASSERT_EQ(v.size(), 1);
        ASSERT_GE(v.capacity(), 1);
    }
}

TEST(SafeVec, EmplaceBack)
{
    {
        int_allocator alloc{};
        int_vec v{alloc};
        v.emplace_back(1).expect("emplace_back should work");
        ASSERT_EQ(v.size(), 1);
        ASSERT_EQ(v[0], 1);
        ASSERT_GE(v.capacity(), 1);
    }

    {
        not_copyable_allocator alloc{};
        not_copyable_vec v{alloc};
        v.emplace_back().expect("emplace_back should work");
        ASSERT_EQ(v.size(), 1);
        ASSERT_GE(v.capacity(), 1);
    }

    {
        not_copyable_allocator alloc{};
        not_copyable_vec v{alloc};
        NotCopyable a;
        v.emplace_back(std::move(a)).expect("emplace_back should work");
        ASSERT_EQ(v.size(), 1);
        ASSERT_GE(v.capacity(), 1);
    }
}

TEST(SafeVec, Insert)
{
    int_allocator alloc{};
    {
        int_vec v{alloc};
        constexpr int a = 1;
        v.insert(v.cbegin(), a).expect("insert should work");  // l-value
        ASSERT_EQ(v.size(), 1);
        ASSERT_EQ(v[0], 1);
        ASSERT_GE(v.capacity(), 1);
    }

    {
        int_vec v{alloc};
        v.insert(v.cbegin(), 1).expect("insert should work");  // r-value
        ASSERT_EQ(v.size(), 1);
        ASSERT_EQ(v[0], 1);
        ASSERT_GE(v.capacity(), 1);
    }

    {
        int_vec v{alloc};
        constexpr int a = 42;
        v.insert(v.cbegin(), static_cast<size_t>(3), a)
            .expect("insert should work");  // count + val
        ASSERT_EQ(v.size(), 3);
        ASSERT_EQ(v[0], 42);
        ASSERT_EQ(v[1], 42);
        ASSERT_EQ(v[2], 42);
    }

    {
        int_vec v{alloc};
        std::vector<int> vec{1, 2, 3};
        v.insert(v.cbegin(), vec.begin(), vec.end()).expect("insert should work");  // count + val
        ASSERT_EQ(v.size(), 3);
        ASSERT_EQ(v[0], 1);
        ASSERT_EQ(v[1], 2);
        ASSERT_EQ(v[2], 3);
    }

    {
        int_vec v{alloc};
        v.insert(v.begin(), {1, 2, 3}).expect("insert should work");  // initializer_list
        ASSERT_EQ(v.size(), 3);
        ASSERT_EQ(v[0], 1);
        ASSERT_EQ(v[1], 2);
        ASSERT_EQ(v[2], 3);
    }
}

TEST(SafeVec, Assign)
{
    int_allocator alloc{};
    {
        int_vec v{alloc};
        constexpr int a = 42;
        v.assign(static_cast<size_t>(3), a).expect("assign should work");  // count + val
        ASSERT_EQ(v.size(), 3);
        ASSERT_EQ(v[0], 42);
        ASSERT_EQ(v[1], 42);
        ASSERT_EQ(v[2], 42);
    }

    {
        int_vec v{alloc};
        std::vector<int> vec{1, 2, 3};
        v.assign(vec.begin(), vec.end()).expect("assign should work");  // iterator
        ASSERT_EQ(v.size(), 3);
        ASSERT_EQ(v[0], 1);
        ASSERT_EQ(v[1], 2);
        ASSERT_EQ(v[2], 3);
    }

    {
        int_vec v{alloc};
        v.assign({1, 2, 3}).expect("insert should work");  // initializer_list
        ASSERT_EQ(v.size(), 3);
        ASSERT_EQ(v[0], 1);
        ASSERT_EQ(v[1], 2);
        ASSERT_EQ(v[2], 3);
    }
}

TEST(SafeVec, Resize)
{
    int_allocator alloc{};
    {
        int_vec v{alloc};
        v.resize(static_cast<size_t>(3)).expect("resize should work");  // iterator
        ASSERT_EQ(v.size(), 3);
        ASSERT_EQ(v[0], 0);
        ASSERT_EQ(v[1], 0);
        ASSERT_EQ(v[2], 0);
    }

    {
        int_vec v{alloc};
        constexpr int a = 42;
        v.resize(static_cast<size_t>(3), a).expect("resize should work");  // iterator
        ASSERT_EQ(v.size(), 3);
        ASSERT_EQ(v[0], 42);
        ASSERT_EQ(v[1], 42);
        ASSERT_EQ(v[2], 42);
    }
}

TEST(SafeVec, Clone)
{
    int_allocator alloc{};
    int_vec v{alloc};
    v.assign({1, 2, 3}).expect("Could not assign values");

    const auto result = v.Clone();
    ASSERT_FALSE(result.has_error());
    ASSERT_EQ(result.value().size(), 3);
    ASSERT_EQ(result.value(), v);
}

TEST(SafeVec, AllocationFailuresReturnError)
{
    fail_allocator<int> alloc{};
    safe_containers::vector<int, fail_allocator<int>> v{alloc};

    {
        const auto result = v.push_back(1);
        ASSERT_TRUE(result.has_error());
    }

    {
        const auto result = v.insert(v.cbegin(), 1);
        ASSERT_TRUE(result.has_error());
    }

    {
        const auto result = v.emplace(v.cbegin(), 1);
        ASSERT_TRUE(result.has_error());
    }

    {
        const auto result = v.emplace_back(1);
        ASSERT_TRUE(result.has_error());
    }
}
