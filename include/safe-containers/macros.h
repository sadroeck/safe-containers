#pragma once

/** Collection of configuration & utility macros **/

#if __cplusplus >= 202002L
#define MAYBE_CONSTEXPR constexpr
#else
#define MAYBE_CONSTEXPR
#endif

// Configure a custom hook to be called before the container `potentially`
// allocates memory. E.g. this cqn be useful for testing/debugging purposes.
#ifndef SAFE_CONTAINERS_PRE_ALLOC_HOOK
#define SAFE_CONTAINERS_PRE_ALLOC_HOOK
#endif  // SAFE_CONTAINERS_PRE_ALLOC_HOOK

// Configure a set of extra catches to be executed after the bad_alloc catch.
// E.g. this can be useful for logging or debugging purposes.
#ifndef SAFE_CONTAINERS_EXTRA_CATCHES
#define SAFE_CONTAINERS_EXTRA_CATCHES       \
    catch (const std::exception& ex)        \
    {                                       \
        return cpp::fail(ContainerError{}); \
    }
#endif  // SAFE_CONTAINERS_EXTRA_CATCHES

// Configure a custom hook to be called after the container catches a bad_alloc
// exception. E.g. this can be useful for logging or debugging purposes.
#ifndef SAFE_CONTAINERS_POST_BAD_ALLOC_HOOK
#define SAFE_CONTAINERS_POST_BAD_ALLOC_HOOK
#endif  // SAFE_CONTAINERS_POST_BAD_ALLOC_HOOK

#define SAFE_CONTAINERS_CATCH_OOM(F)            \
    {                                           \
        SAFE_CONTAINERS_PRE_ALLOC_HOOK          \
        try                                     \
        {                                       \
            F;                                  \
        }                                       \
        catch (const std::bad_alloc&)           \
        {                                       \
            SAFE_CONTAINERS_POST_BAD_ALLOC_HOOK \
            return cpp::fail(ContainerError{}); \
        }                                       \
        SAFE_CONTAINERS_EXTRA_CATCHES           \
    }
