#pragma once

#include <meta_mate/config_builder.hpp>

// Policies
enum class P1 { v1, v2 };
enum class P2 { zero, one };
enum class P3 { p3_def, other, another };

void config_builder_tests()
{
  // Builder for a class which could be customized by P1-P3 policies.
  using builder = mm::ConfigBuilder<P1, P2, P3>;

  using config1 = builder::default_;
  static_assert(config1::get<P1>() == P1::v1);
  static_assert(config1::get<P2>() == P2::zero);
  static_assert(config1::get<P3>() == P3::p3_def);

  using config2 = builder::with<P1, P1::v2>::build;
  static_assert(config2::get<P1>() == P1::v2);
  static_assert(config2::get<P2>() == P2::zero);
  static_assert(config2::get<P3>() == P3::p3_def);

  using config3 = builder::with<P3, P3::other>::and_<P1, P1::v2>::and_<P2, P2::one>::build;
  static_assert(config3::get<P1>() == P1::v2);
  static_assert(config3::get<P2>() == P2::one);
  static_assert(config3::get<P3>() == P3::other);

  using cant_build =
    builder::with<P1, P1::v2>::and_<P3, P3::other>::and_<P2, P2::one>::and_<P1, P1::v1>; //::build;
}

int main()
{
  config_builder_tests();

  return 0;
}