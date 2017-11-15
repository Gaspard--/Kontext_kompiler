#pragma once

#include <stack>
#include <memory>
#include <string>
#include <string_view>
#include <map>
#include <variant>
#include <functional>

struct Value;
struct Function;

using TypeDefinition = std::variant<std::map<std::string, uint64_t>, uint64_t>;
// using TypeDefinition = std::variant<std::map<std::string, uint64_t>, Function, uint64_t>;
using TypeList = std::unordered_map<uint64_t, TypeDefinition>;
using FunctionList = std::unordered_map<uint64_t, Function>;

struct FunctionOrTypeIndex
{
  bool isFunction;
  uint64_t id;
};

struct Function
{
  using FunctionPtr = Value(TypeList const &, Value const &stored, Value const &param);

  uint64_t storedValueType;
  uint64_t paramType;
  uint64_t returnType;
  bool returnsPrefix;
  double priority;

  // TypeList is necessary to be able to disambiguate type.
  FunctionPtr data;

  bool operator>(Function const &other) const
  {
    return priority > other.priority;
  }
};

#define KOMPILER_PRIMITIVE_LIST			\
  signed char, unsigned char,			\
    signed short, unsigned short,		\
    signed int, unsigned int,			\
    signed long int, unsigned long int,		\
    float, double, bool,			\
    std::shared_ptr<TypeDefinition>,		\
    std::shared_ptr<Function>			\

using Primitive = std::variant<KOMPILER_PRIMITIVE_LIST>;

struct Value
{
  uint64_t type;
  /// TODO: replace with something more optimal
  std::vector<Primitive> flatFields; // can be empty
};

inline bool operator>=(TypeDefinition const &container, TypeDefinition const &contained)
{
  return std::visit([](auto const &container, auto const &contained)
		    {
		      if constexpr (std::is_same_v<decltype(container), decltype(contained)>) {
			  if constexpr (std::is_same_v<decltype(container), uint64_t const &>) {
			      return container == contained;
			    }
			  else
			    return std::includes(container.begin(), container.end(),
						 contained.begin(), contained.end());
			}
		      return false;
		    }, container, contained);
}
