#pragma once

#include "detail/config_builder.hpp"

namespace mm {

// Compile-time config builder helper for classes with policy-based design.
template <typename... T>
struct ConfigBuilder : detail::WithChain<std::tuple<>, T...>
{
  using default_ = detail::PoliciesGetter<std::tuple<detail::PolicyHolder<T>...>, T...>;
};
}
