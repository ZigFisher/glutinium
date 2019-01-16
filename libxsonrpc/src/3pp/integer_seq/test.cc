// Copyright Jonathan Wakely 2012-2013
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "integer_seq.h"

#include <type_traits>

template<std::size_t I> struct Wrap { };  // create a type from an integer

template<int N, std::size_t... Ind>
struct test
{
  typedef redi::make_index_sequence<N> test_type;
  typedef redi::index_sequence<Ind...> expected;

  static_assert(std::is_same<test_type, expected>::value, "ok");

  typedef redi::index_sequence_for<Wrap<Ind>...>        test2_type;
  static_assert(std::is_same<test2_type, expected>::value, "ok");
};

template struct test<0>;
template struct test<1, 0>;
template struct test<2, 0, 1>;
template struct test<3, 0, 1, 2>;
template struct test<4, 0, 1, 2, 3>;
template struct test<5, 0, 1, 2, 3, 4>;
template struct test<6, 0, 1, 2, 3, 4, 5>;
template struct test<7, 0, 1, 2, 3, 4, 5, 6>;

