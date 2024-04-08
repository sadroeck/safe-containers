#pragma once

/** Extension utilities for working with the `result` library. **/

#include <safe-containers/result/result.h>

namespace result_ext
{
// A prototype function that sets the given error in the caller context.
// Should be specialized for any error `E` that can be converted into a context
// error.
template <typename E>
void IntoContextError(E&& error);

// A prototype function that converts the given error into another error type.
// Should be specialized for any error `InputError` that can be converted into another
// error type.
template <typename InputError, typename OutputError>
OutputError ConvertError(InputError&& error);

// Functor to convert a `cpp::result<T, E>` containing an error into another
// valid propagated error type. This can be used to convert between the
// cpp::result error types & bool/nullptr/Option types, converting to a context
// error in the process.
//
// All lossy conversions set the appropriate context error, so a specialization
// for `IntoContextError<E>` is required the `E` type. Conversions to a
// different cpp::result type do not set the context error, as the error is
// propagated.
template <typename Result>
struct [[nodiscard]] LossyErrorConverter
{
    using T = typename Result::value_type;
    using E = typename Result::error_type;

    explicit LossyErrorConverter(Result&& res)
        : result(std::move(res))
    {
    }

    // Consume a cpp::result<T, E> containing an error into a false value.
    // Note: This sets the appropriate context error.
    [[nodiscard]] operator bool() const
    {
        IntoContextError(result.error());
        return false;
    }

    // Consume a cpp::result<T, E> containing an error into a nullptr value.
    // Note: This sets the appropriate context error.
    template <typename V>
    [[nodiscard]] operator V*() const
    {
        IntoContextError(result.error());
        return nullptr;
    }

    // Consume a cpp::result<T, E> containing an error into the error (E) value.
    // Note: This sets the appropriate context error.
    [[nodiscard]] operator E() const
    {
        IntoContextError(result.error());
        return result.error();
    }

    // Consumes a cpp::result<T, E> containing an error into a 0 value.
    // Note: This sets the appropriate context error.
    [[nodiscard]] operator int() const
    {
        IntoContextError(result.error());
        return 0;
    }

    // Consume a cpp::result<T, E> containing an error into a different error type.
    // The target error requires a specialization for `ConvertError<E, OutputError>`.
    template <typename OutputError>
    [[nodiscard]] operator OutputError() const
    {
        return ConvertError<E, OutputError>(result.error());
    }

    Result&& result;
};
}  // namespace result_ext

// Utility function to convert a `cpp::result<T, E>` universal reference into a
// context error. Note: This requires a specialization for `IntoContextError<E>`
// to be defined for the `E` type.
template <typename T, typename E>
[[nodiscard]] bool CheckErrorResult(cpp::result<T, E>&& result) noexcept
{
    if (result.has_error())
    {
        result_ext::IntoContextError(result.error());
        return false;
    }
    return true;
}

template <typename Res>
inline auto ValueOrVoid(Res&& res) noexcept
{
    using R = std::decay_t<Res>;
    if constexpr (std::is_same_v<typename R::value_type, void>)
    {
        // Just return a dummy bool value, as a cpp::result<void, E> cannot move
        // its value type.
        return;
    }
    else
    {
        return std::forward(res).value();
    }
}

// Utility macros to convert statements of the form
//    cpp::result<T, E> result = my_fallible_function()
//    if (result.has_error())
//    {
//        return RET;
//    }
//    value = result.value();
// into a single line statement:
//    auto value = TRY(result);
//    or
//    auto value = TRY_CHECK(result);
//
// The `TRY` macro provides an early-return of the error contained in the result
// variable. For these non-lossy conversions, the error is simply propagated.
// The `TRY_CHECK` automatically coerces the error into the required error
// return type. For lossy conversions, i.e. those eliminating the error type,
// the appropriate context error will be set.
//
// Note: These macros use the `Statement expression` extension in clang/gcc.
#define TRY(result)                                                \
    __extension__({                                                \
        auto&& x = result;                                         \
        if (x.has_error()) return cpp::fail(std::move(x).error()); \
        ValueOrVoid(x);                                            \
    })

#define TRY_CHECK(result)                                                          \
    __extension__({                                                                \
        auto&& x = result;                                                         \
        if (x.has_error()) return ::result_ext::LossyErrorConverter(std::move(x)); \
        ValueOrVoid(x);                                                            \
    })
