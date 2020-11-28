#pragma once

#include <type_traits>
#include <tuple>
#include <cstddef>
#include <cstdint>

namespace mm::detail {
template <typename Policy, Policy val = Policy{}>
struct PolicyHolder
{
  using policy                  = Policy;
  static constexpr Policy value = val;
};

template <typename X, typename Tuple>
class FindPolicy;

template <typename X, int64_t idx_val, typename... T>
struct FindPolicyIdxDispatch
{
  using type = std::tuple_element_t<static_cast<size_t>(idx_val), std::tuple<T...>>;
};

template <typename X, typename... T>
struct FindPolicyIdxDispatch<X, -1, T...>
{
  using type = PolicyHolder<X>;
};

template <typename X, typename... T>
class FindPolicy<X, std::tuple<T...>>
{
  template <size_t... idx>
  static constexpr int64_t find_idx(std::index_sequence<idx...>)
  {
    return -1 + ((std::is_same_v<X, typename T::policy> ? idx + 1 : 0) + ...);
  }
  static constexpr int64_t idx = find_idx(std::index_sequence_for<T...>{});
  using holder                 = typename FindPolicyIdxDispatch<X, idx, T...>::type;

public:
  static constexpr X value = holder::value;
};

template <typename PolicyValues, typename... T>
struct PoliciesGetter
{
  using policies_holders = PolicyValues;
  using allowed_types    = std::tuple<T...>;

  template <typename EnumT>
  constexpr static EnumT get()
  {
    return FindPolicy<EnumT, policies_holders>::value;
  }
};

template <typename Values, typename... T>
struct WithChain
{
  template <typename EnumT, EnumT, typename = void>
  struct with
  {
    static_assert(!std::is_same_v<EnumT, EnumT>, "type is invalid or has been used already!");
  };

  template <typename EnumT, EnumT val>
  struct with<EnumT, val, std::enable_if_t<(std::is_same_v<EnumT, T> || ...)>>
  {
  private:
    using policy_values =
      decltype(std::tuple_cat(std::declval<Values>(), std::declval<std::tuple<PolicyHolder<EnumT, val>>>()));

  public:
    template <typename NextEnumT, NextEnumT next_val>
    using and_ = typename WithChain<policy_values, std::conditional_t<std::is_same_v<EnumT, T>, void, T>...>::
      template with<NextEnumT, next_val>;
    using build = PoliciesGetter<policy_values, T...>;
  };
};
}