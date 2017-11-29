#pragma once

#include "Value.hpp"
#include "Properties.hpp"

struct Type
{
  DataType dataType;
  PropertyList::Properties properties;
};

struct DefinedValue
{
  Value value;
  Type type;
};

struct UnaryOperator
{
  std::vector<UnaryFunction> data;

  static UnaryOperator const &getUnapplyable()
  {
    static UnaryOperator unapplyable{};

    return unapplyable;
  }

  void addFunc(UnaryFunction &&func)
  {
    data.push_back(func);
  }
};

struct UnaryFunction
{
  using FunctionPtr = std::variant<DefinedValue, std::pair<Value, UnaryOperator>>(Value const &stored, DefinedValue const &param);

  PropertyList::Properties requiredProperties;
  FunctionPtr *func;
};

template<class T>
DefinedValue makePrimitiveDefinedValue(T &&first)
{
  Primitive primitive(first);

  return DefinedValue{Value{primitive}, Type{primitive.index(), {PropertyList::getPrimitiveProperty<T>()}}};
}

