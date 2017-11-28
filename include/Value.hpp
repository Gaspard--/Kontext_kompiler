#pragma once

#include <stack>
#include <memory>
#include <string>
#include <string_view>
#include <map>
#include <variant>
#include <functional>
#include "Properties.hpp"
#include "Token.hpp"

struct UnaryFunction;

#define KOMPILER_PRIMITIVE_LIST						\
  unsigned long int, double,						\
    std::shared_ptr<Token>						\

// std::shared_ptr<TypeDefinition>,
// std::shared_ptr<UnaryFunction>

// TODO: reflection

using Primitive = std::variant<KOMPILER_PRIMITIVE_LIST>;
using Value = std::vector<Primitive>;


using PrimitiveId = unsigned long int;

struct StructDefinition : public std::map<std::string, std::variant<StructDefinition, PrimitiveId>>
{
  using std::map<std::string, std::variant<StructDefinition, PrimitiveId>>::map;
};

using DataType = std::variant<StructDefinition, PrimitiveId>;

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
  using FunctionPtr = std::pair<Value, std::variant<Type, UnaryOperator>>(Value const &stored, DefinedValue const &param);

  PropertyList::Properties requiredProperties;
  FunctionPtr *func;
};

template<class T>
DefinedValue makePrimitiveDefinedValue(T &&first)
{
  Primitive primitive(first);

  return DefinedValue{Value{primitive}, Type{static_cast<PrimitiveId>(primitive.index()), {}}};
}
