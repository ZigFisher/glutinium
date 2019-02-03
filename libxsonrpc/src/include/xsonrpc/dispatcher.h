// This file is part of xsonrpc, an XML/JSON RPC library.
// Copyright (C) 2015 Erik Johansson <erik@ejohansson.se>
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by the
// Free Software Foundation; either version 2.1 of the License, or (at your
// option) any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
// for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

#ifndef XSONRPC_DISPATCHER_H
#define XSONRPC_DISPATCHER_H

#include "fault.h"
#include "request.h"
#include "response.h"
#include "value.h"

#if __cplusplus <= 201103L
#include "integer_seq.h"
namespace std {
using redi::index_sequence;
using redi::index_sequence_for;
} // namespace std
#endif

#include <functional>
#include <utility>
#include <vector>

namespace xsonrpc {

class MethodWrapper
{
public:
  typedef std::function<Value(const Request::Parameters&)> Method;

  explicit MethodWrapper(Method method) : myMethod(method) {}

  MethodWrapper(const MethodWrapper&) = delete;
  MethodWrapper& operator=(const MethodWrapper&) = delete;

  bool IsHidden() const { return myIsHidden; }
  void SetHidden(bool hidden = true) { myIsHidden = hidden; }

  MethodWrapper& SetHelpText(std::string help);
  const std::string& GetHelpText() const { return myHelpText; }

  template<typename... ParameterTypes>
  MethodWrapper& AddSignature(Value::Type returnType,
                              ParameterTypes... parameterTypes)
  {
    mySignatures.emplace_back(
      std::initializer_list<Value::Type>{returnType, parameterTypes...});
    return *this;
  }

  const std::vector<std::vector<Value::Type>>&
  GetSignatures() const { return mySignatures; }

  Value operator()(const Request::Parameters& params) const
  {
    return myMethod(params);
  }

private:
  Method myMethod;
  bool myIsHidden = false;
  std::string myHelpText;
  std::vector<std::vector<Value::Type>> mySignatures;
};

template<typename> struct ToStdFunction;

template<typename ReturnType, typename... ParameterTypes>
struct ToStdFunction<ReturnType(*)(ParameterTypes...)>
{
  typedef std::function<ReturnType(ParameterTypes...)> Type;
};

template<typename ReturnType, typename T, typename... ParameterTypes>
struct ToStdFunction<ReturnType(T::*)(ParameterTypes...)>
{
  typedef std::function<ReturnType(ParameterTypes...)> Type;
};

template<typename ReturnType, typename T, typename... ParameterTypes>
struct ToStdFunction<ReturnType(T::*)(ParameterTypes...) const>
{
  typedef std::function<ReturnType(ParameterTypes...)> Type;
};

template<typename MethodType, bool isClass>
struct StdFunction {};

template<typename MethodType>
struct StdFunction<MethodType, false>
{
  typedef typename ToStdFunction<MethodType>::Type Type;
};

template<typename MethodType>
struct StdFunction<MethodType, true>
{
  typedef typename ToStdFunction<
    decltype(&MethodType::operator())>::Type Type;
};

class Dispatcher
{
public:
  std::vector<std::string> GetMethodNames(bool includeHidden = false) const;
  MethodWrapper& GetMethod(const std::string& name);

  MethodWrapper& AddMethod(
    std::string name, MethodWrapper::Method method);

  template<typename MethodType>
  typename std::enable_if<
    !std::is_convertible<MethodType, MethodWrapper::Method>::value
    && !std::is_member_pointer<MethodType>::value,
    MethodWrapper>::type&
  AddMethod(std::string name, MethodType method)
  {
    static_assert(!std::is_bind_expression<MethodType>::value,
                  "Use AddMethod with 3 arguments to add member method");
    typename StdFunction<MethodType, std::is_class<MethodType>::value>::Type
      function(std::move(method));
    return AddMethodInternal(std::move(name), std::move(function));
  }

  template<typename T>
  MethodWrapper& AddMethod(std::string name,
                           Value(T::*method)(const Request::Parameters&),
                           T& instance)
  {
    return AddMethod(std::move(name),
                     std::bind(method, &instance, std::placeholders::_1));
  }

  template<typename T>
  MethodWrapper& AddMethod(std::string name,
                           Value(T::*method)(const Request::Parameters&) const,
                           T& instance)
  {
    return AddMethod(std::move(name),
                     std::bind(method, &instance, std::placeholders::_1));
  }

  template<typename ReturnType, typename T, typename... ParameterTypes>
  MethodWrapper& AddMethod(std::string name,
                           ReturnType(T::*method)(ParameterTypes...),
                           T& instance)
  {
    std::function<ReturnType(ParameterTypes...)> function =
      [&instance,method] (ParameterTypes&&... params) -> ReturnType
      {
        return (instance.*method)(std::forward<ParameterTypes>(params)...);
      };
    return AddMethodInternal(std::move(name), std::move(function));
  }

  template<typename ReturnType, typename T, typename... ParameterTypes>
  MethodWrapper& AddMethod(std::string name,
                           ReturnType(T::*method)(ParameterTypes...) const,
                           T& instance)
  {
    std::function<ReturnType(ParameterTypes...)> function =
      [&instance,method] (ParameterTypes&&... params) -> ReturnType
      {
        return (instance.*method)(std::forward<ParameterTypes>(params)...);
      };
    return AddMethodInternal(std::move(name), std::move(function));
  }

  void RemoveMethod(const std::string& name);

  Response Invoke(const std::string& name,
                  const Request::Parameters& parameters,
                  const Value& id) const;

private:
  template<typename ReturnType, typename... ParameterTypes>
  MethodWrapper& AddMethodInternal(
    std::string name,
    std::function<ReturnType(ParameterTypes...)> method)
  {
    return AddMethodInternal(std::move(name), std::move(method),
                             std::index_sequence_for<ParameterTypes...>{});
  }

  template<typename... ParameterTypes>
  MethodWrapper& AddMethodInternal(
    std::string name,
    std::function<void(ParameterTypes...)> method)
  {
    std::function<Value(ParameterTypes...)> returnMethod =
      [method] (ParameterTypes&&... params) -> Value
      {
        method(std::forward<ParameterTypes>(params)...);
        return Value();
      };
    return AddMethodInternal(
      std::move(name), std::move(returnMethod),
      std::index_sequence_for<ParameterTypes...>{});
  }

  template<typename ReturnType, typename... ParameterTypes,
           std::size_t... index>
  MethodWrapper& AddMethodInternal(
    std::string name,
    std::function<ReturnType(ParameterTypes...)> method,
    std::index_sequence<index...>)
  {
    MethodWrapper::Method realMethod =
      [method] (const Request::Parameters& params) -> Value
      {
        if (params.size() != sizeof...(ParameterTypes)) {
          throw InvalidParametersFault();
        }
        return method(
          params[index]
          .AsType<typename std::decay<ParameterTypes>::type>()...);
      };
    return AddMethod(std::move(name), std::move(realMethod));
  }

  std::map<std::string, MethodWrapper> myMethods;
};

} // namespace xsonrpc

#endif
