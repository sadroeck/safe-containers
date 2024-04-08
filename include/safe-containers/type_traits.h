#pragma once

#include <type_traits>

template <class Ty, class... Args>
struct contains_type : std::disjunction<std::is_same<Ty, Args>...>
{
};
