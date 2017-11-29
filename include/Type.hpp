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
  Primitive primitive(std::move(first));

  return DefinedValue{Value{primitive}, Type{0, {PropertyList::getPrimitiveProperty<std::remove_reference_t<T>>()}}};
}

inline std::ostream &operator<<(std::ostream &out, Type const &)
{
  out << "TODO: add way to print type";
  return out;
}

inline std::ostream &operator<<(std::ostream &out, UnaryOperator const &)
{
  out << "TODO: add way to print unary operator";
  return out;
}

inline std::ostream &operator<<(std::ostream &out, UnaryFunction const &)
{
  out << "TODO: add way to print unary function";
  return out;
}
