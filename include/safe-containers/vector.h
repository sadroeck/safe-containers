#pragma once

#include <safe-containers/error.h>
#include <safe-containers/macros.h>
#include <safe-containers/result/result_ext.h>
#include <safe-containers/type_traits.h>

#include <memory>
#include <vector>

namespace safe_containers
{

// `vector` is a wrapper around `std::vector` that allows for fallible allocation
// handling using `result` types.
//
// It is intended to be used in environments where exceptions are not allowed.
// The `result` type is used to signal allocation failures, which can be handled
// at the call-site.
template <typename T, typename AllocatorType = std::allocator<T>>
class vector : public std::vector<T, AllocatorType>
{
   public:
    template <typename V>
    using result = cpp::result<V, ContainerError>;

    using inner = std::vector<T, AllocatorType>;
    using value_type = typename inner::value_type;
    using allocator_type = typename inner::allocator_type;
    using size_type = typename inner::size_type;
    using difference_type = typename inner::difference_type;
    using reference = typename inner::reference;
    using const_reference = typename inner::const_reference;
    using pointer = typename inner::pointer;
    using const_pointer = typename inner::const_pointer;
    using iterator = typename inner::iterator;
    using const_iterator = typename inner::const_iterator;
    using reverse_iterator = typename inner::reverse_iterator;
    using const_reverse_iterator = typename inner::const_reverse_iterator;

   public:
    // Disable the default constructor of the vector with a custom allocator, as it might
    // allocate memory. Considering using `Create(...)` instead.
    template <
        typename U = T,
        typename std::enable_if_t<std::is_same_v<AllocatorType, std::allocator<T>>, U> = nullptr>
    MAYBE_CONSTEXPR vector() noexcept = delete;

    // Allow default construction of the vector with std::allocator<T>.
    // As of C++17, the default constructor is noexcept, so the major STL
    // implementations shouldn't allocate memory in the default constructor.
    template <
        typename U = T,
        typename std::enable_if_t<!std::is_same_v<AllocatorType, std::allocator<T>>, U> = nullptr>
    MAYBE_CONSTEXPR vector() noexcept
        : inner{}
    {
    }

    // Copy constructor and assignment operator would both need to allocate but
    // do not offer a facility to signal OOM beyond throwing. Consider using
    // `assign()` instead.
    vector(vector const&) = delete;
    void operator=(vector const&) = delete;

    MAYBE_CONSTEXPR explicit vector(const allocator_type& alloc) noexcept
        : inner{alloc}
    {
    }

    MAYBE_CONSTEXPR explicit vector(allocator_type& alloc) noexcept
        : inner{alloc}
    {
    }

    MAYBE_CONSTEXPR explicit vector(inner&& vec) noexcept
        : inner{std::move(vec)}
    {
    }

    // Other constructor types of `std::vector` might allocate,
    // but offer no way to signal OOM beyond throwing. Consider using `Create` instead.
    template <typename... Args>
    MAYBE_CONSTEXPR vector(Args&&... args) noexcept = delete;

    MAYBE_CONSTEXPR result<vector> Clone() noexcept
    {
        return Create(inner::cbegin(), inner::cend(), inner::get_allocator());
    }

    // Utility wrapper around the plain `vector`` so `Create` can be
    // used regardless of fallibility.
    MAYBE_CONSTEXPR static result<vector> Create(const allocator_type& alloc) noexcept
    {
        SAFE_CONTAINERS_CATCH_OOM(return result<vector>(cpp::in_place, alloc));
    }

    MAYBE_CONSTEXPR static result<vector> Create(
        size_type count, const allocator_type& alloc) noexcept
    {
        SAFE_CONTAINERS_CATCH_OOM({
            inner vec(count, alloc);
            return result<vector>(cpp::in_place, std::move(vec));
        });
    }

    MAYBE_CONSTEXPR static result<vector> Create(
        size_type count, const T& value, const allocator_type& alloc) noexcept
    {
        SAFE_CONTAINERS_CATCH_OOM({
            inner vec(count, value, alloc);
            return result<vector>(cpp::in_place, std::move(vec));
        })
    }

    template <typename InputIt>
    MAYBE_CONSTEXPR static result<vector> Create(
        InputIt first, InputIt last, const allocator_type& alloc) noexcept
    {
        SAFE_CONTAINERS_CATCH_OOM({
            inner vec(first, last, alloc);
            return result<vector>(cpp::in_place, std::move(vec));
        });
    }

    MAYBE_CONSTEXPR static result<vector> Create(
        std::initializer_list<T> values, const allocator_type& alloc) noexcept
    {
        SAFE_CONTAINERS_CATCH_OOM({
            inner vec(values, alloc);
            return result<vector>(cpp::in_place, std::move(vec));
        });
    }

    template <typename... Args>
    MAYBE_CONSTEXPR result<void> push_back(Args&&... args) noexcept
    {
        static_assert(
            !contains_type<allocator_type, Args...>::value,
            "Arguments cannot contain an allocator type");
        SAFE_CONTAINERS_CATCH_OOM(inner::push_back(std::forward<Args>(args)...));
        return {};
    }

    template <typename... Args>
    MAYBE_CONSTEXPR result<iterator> emplace(Args&&... args) noexcept
    {
        static_assert(
            !contains_type<allocator_type, Args...>::value,
            "Arguments cannot contain an allocator type");
        SAFE_CONTAINERS_CATCH_OOM(return inner::emplace(std::forward<Args>(args)...));
    }

    template <typename... Args>
    MAYBE_CONSTEXPR result<reference> emplace_back(Args&&... args) noexcept
    {
        static_assert(
            !contains_type<allocator_type, Args...>::value,
            "Arguments cannot contain an allocator type");
        SAFE_CONTAINERS_CATCH_OOM(return inner::emplace_back(std::forward<Args>(args)...));
    }

    MAYBE_CONSTEXPR result<iterator> insert(const_iterator pos, value_type&& value) noexcept
    {
        SAFE_CONTAINERS_CATCH_OOM(return inner::insert(pos, value));
    }

    MAYBE_CONSTEXPR result<iterator> insert(
        const_iterator pos, size_type count, const value_type& value) noexcept
    {
        SAFE_CONTAINERS_CATCH_OOM(return inner::insert(pos, count, value));
    }

    MAYBE_CONSTEXPR result<iterator> insert(
        const_iterator pos, std::initializer_list<value_type> values) noexcept
    {
        SAFE_CONTAINERS_CATCH_OOM(return inner::insert(pos, values));
    }

    template <typename InputIt, typename = std::_RequireInputIter<InputIt>>
    result<iterator> insert(const_iterator pos, InputIt first, InputIt last) noexcept
    {
        SAFE_CONTAINERS_CATCH_OOM(return inner::insert(pos, first, last));
    }

    template <typename... Args>
    MAYBE_CONSTEXPR result<iterator> insert(Args&&... args) noexcept
    {
        static_assert(
            !contains_type<allocator_type, Args...>::value,
            "Arguments cannot contain an allocator type");
        SAFE_CONTAINERS_CATCH_OOM(return inner::insert(std::forward<Args>(args)...));
    }

    MAYBE_CONSTEXPR result<void> assign(size_type count, const T& value) noexcept
    {
        SAFE_CONTAINERS_CATCH_OOM(inner::assign(count, value));
        return {};
    }

    template <typename InputIt>
    MAYBE_CONSTEXPR result<void> assign(InputIt first, InputIt last) noexcept
    {
        SAFE_CONTAINERS_CATCH_OOM(inner::assign(first, last));
        return {};
    }

    MAYBE_CONSTEXPR result<void> assign(std::initializer_list<T> values) noexcept
    {
        SAFE_CONTAINERS_CATCH_OOM(inner::assign(values));
        return {};
    }

    MAYBE_CONSTEXPR result<void> resize(size_type count) noexcept
    {
        SAFE_CONTAINERS_CATCH_OOM(inner::resize(count));
        return {};
    }

    MAYBE_CONSTEXPR result<void> resize(size_type count, const value_type& value) noexcept
    {
        SAFE_CONTAINERS_CATCH_OOM(inner::resize(count, value));
        return {};
    }

    MAYBE_CONSTEXPR void swap(vector& other) noexcept { inner::swap(other); }
};

}  // namespace safe_containers