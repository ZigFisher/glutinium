#ifndef REDI_INTEGER_SEQ_UTIL_H
#define REDI_INTEGER_SEQ_UTIL_H

// Copyright Jonathan Wakely 2012-2013
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "integer_seq.h"

#include <utility>

namespace redi
{
  template<int N, typename T, T... I>
    constexpr integer_sequence<T, N+I...>
    add(integer_sequence<T, I...>) noexcept
    { return {}; }

  template<int N, typename T, T... I>
    constexpr integer_sequence<T, N*I...>
    multiply(integer_sequence<T, I...>) noexcept
    { return {}; }

  template<typename T, T I0, T... I>
    constexpr integer_sequence<T, I0>
    car(integer_sequence<T, I0, I...>) noexcept
    { return {}; }

  template<typename T, T I0, T... I>
    constexpr integer_sequence<T, I...>
    cdr(integer_sequence<T, I0, I...>) noexcept
    { return {}; }

  template<typename F, typename Tuple, std::size_t... I>
    auto
    apply_(F&& f, Tuple&& args, index_sequence<I...>)
    noexcept(noexcept(std::forward<F>(f)(std::get<I>(std::forward<Tuple>(args))...)))
    -> decltype(std::forward<F>(f)(std::get<I>(std::forward<Tuple>(args))...))
    {
      return std::forward<F>(f)(std::get<I>(std::forward<Tuple>(args))...);
    }

  template<typename F, typename Tuple, typename Indices
           = make_index_sequence<std::tuple_size<typename std::decay<Tuple>::type>::value>>
    auto
    apply(F&& f, Tuple&& args)
    noexcept(noexcept(apply_(std::forward<F>(f), std::forward<Tuple>(args), Indices())))
    -> decltype(apply_(std::forward<F>(f), std::forward<Tuple>(args), Indices()))
    {
      return apply_(std::forward<F>(f), std::forward<Tuple>(args), Indices());
    }

}  // namespace redi

#endif  // REDI_INTEGER_SEQ_UTIL_H

// vi: set ft=cpp sw=2:
