#pragma once

#include <stack>
#include <memory>
#include <string>

class Variable
{
public:
  uint64_t typeId;

  constexpr Variable()
  : typeId(0u)
  {}

  constexpr Variable(uint64_t typeId)
  : typeId(typeId)
  {}

  constexpr Variable(Variable const &) = default;

  ~Variable() = default;
};

class Value : public Variable
{
public:
  constexpr Value() = default;
  constexpr Value(Value const &) = default;
  ~Value() = default;
};

template<class T>
class CompilerValue : public Value
{
public:
  T data;

  CompilerValue<T>(T const &t)
    : data(t)
  {
  }

  T &operator*()
  {
    return data;
  }

  T const &operator*() const
  {
    return data;
  }

  T *operator->()
  {
    return &data;
  }

  T const *operator->() const
  {
    return &data;
  }
};

class Type
{
  
};

class Class : public Type
{
  
};

template<class T>
class CompilerType : public Class
{
};

class Pointer : public Class
{
  // ???
};

class Operator : public Type
{
public:
  constexpr Operator() = default;
  constexpr Operator(Operator const &) = default;
  ~Operator() = default;
};
