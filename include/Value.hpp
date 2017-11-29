#pragma once

#include <stack>
#include <memory>
#include <string>
#include <string_view>
#include <map>
#include <variant>
#include <functional>
#include "Token.hpp"

struct UnaryOperator;
struct UnaryFunction;
struct Type;

#define KOMPILER_PRIMITIVE_LIST						\
  unsigned long int, double, void *,					\
    std::shared_ptr<Token>,						\
    std::shared_ptr<Type>,						\
    std::shared_ptr<UnaryOperator>,					\
    std::shared_ptr<UnaryFunction>					\



using Primitive = std::variant<KOMPILER_PRIMITIVE_LIST>;

using Value = std::vector<Primitive>;
using PrimitiveId = unsigned long int;

struct StructDefinition : public std::map<std::string, std::variant<StructDefinition, PrimitiveId>>
{
  using std::map<std::string, std::variant<StructDefinition, PrimitiveId>>::map;
};

using DataType = std::variant<StructDefinition, PrimitiveId>;

