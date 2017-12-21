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
    std::unique_ptr<Token>,						\
    std::unique_ptr<Type>,						\
    std::unique_ptr<UnaryOperator>,					\
    std::unique_ptr<UnaryFunction>					\

using Primitive = std::variant<KOMPILER_PRIMITIVE_LIST>;

using Value = std::vector<Primitive>;

using PrimitiveId = unsigned long int;

struct StructDefinition : public std::map<std::string, std::variant<StructDefinition, PrimitiveId>>
{
  using std::map<std::string, std::variant<StructDefinition, PrimitiveId>>::map;
};

using DataType = std::variant<StructDefinition, PrimitiveId>;
