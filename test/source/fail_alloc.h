#pragma once

template <typename T>
struct fail_allocator
{
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    pointer allocate(size_type n, const_pointer hint = 0)
    {
        std::ignore = n;
        std::ignore = hint;
        throw std::bad_alloc();
    }

    void deallocate(pointer p, size_type n)
    {
        std::ignore = p;
        std::ignore = n;
    }

    template <typename Ty>
    void construct(pointer p, const_reference t)
    {
        std::ignore = p;
        std::ignore = t;
        throw std::bad_alloc();
    }

    template <typename Ty>
    void destroy(pointer p)
    {
        (reinterpret_cast<Ty*>(p))->~Ty();
    }
};