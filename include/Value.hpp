#pragma once

#include <stack>
#include <memory>
#include <string>
#include <string_view>
#include <map>
#include <variant>
#include <functional>
// #include "Properties.hpp"

struct UnaryFunction;

#define KOMPILER_PRIMITIVE_LIST			\
  signed long int, unsigned long int, double	\

// std::shared_ptr<TypeDefinition>,		
// std::shared_ptr<UnaryFunction>      	

// TODO: reflection

using Primitive = std::variant<KOMPILER_PRIMITIVE_LIST>;
using Value = std::vector<Primitive>;


using PrimitiveId = unsigned long int; // TODO: possibly use an enum?

struct StructDefinition : public std::map<std::string, std::variant<StructDefinition, PrimitiveId>>
{
  using std::map<std::string, std::variant<StructDefinition, PrimitiveId>>::map;
};

using DataType = std::variant<StructDefinition, PrimitiveId>;


class Type
{
  DataType dataType;
  // PropertyList::Properties properties;
};

struct UnaryFunction
{
  using FunctionPtr = Value(Value const &stored, Value const &param);

  struct FuncSignature
  {
    Type paramType;
    Type returnType;
    bool returnsPrefix;
  };

  FuncSignature signature;
  FunctionPtr func;
};

struct UnaryOperator
{
  StructDefinition storedValueType;
  std::vector<UnaryFunction> data;
};
