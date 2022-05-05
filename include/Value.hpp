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

#define KOMPILER_PRIMITIVE_X_LIST				        \
  X(unsigned long int),							\
    X(double),								\
    X(void *),								\
    X(std::unique_ptr<Token>),						\
    X(std::unique_ptr<Type>),						\
    X(std::unique_ptr<UnaryOperator>),					\
    X(std::unique_ptr<UnaryFunction>)					\

#define X(a) a

using Primitive = std::variant<KOMPILER_PRIMITIVE_X_LIST>;

#undef X

using Value = std::vector<Primitive>;
