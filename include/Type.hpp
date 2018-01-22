#pragma once

#include "Value.hpp"
#include "Properties.hpp"

struct Kompiler;

struct Type
{
  DataType dataType;
  PropertyList::Properties properties;
};

struct DefinedValue
{
  Value value;
  Type type;
  DefinedValue(DefinedValue const &) = delete;
  DefinedValue(DefinedValue &&) = default;
  DefinedValue &operator=(DefinedValue &&) = default;
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

  template<DefinedValue(*func)(Kompiler &kompiler, Value &&stored, DefinedValue &&param)>
  void addFunc(PropertyList::Properties requiredProperties)
  {
    addFunc(UnaryFunction{requiredProperties,
			  [](Kompiler &kompiler, Value &&stored, DefinedValue &&param)
			  -> std::variant<DefinedValue, std::pair<Value, UnaryOperator>>
			  {
			    return {func(kompiler, std::move(stored), std::move(param))};
			  }});
  }
};

struct UnaryFunction
{
  using FunctionPtr = std::variant<DefinedValue, std::pair<Value, UnaryOperator>>(Kompiler &kompiler, Value &&stored, DefinedValue &&param);

  PropertyList::Properties requiredProperties;
  FunctionPtr *func;
};

template<class T>
DefinedValue makePrimitiveDefinedValue(T &&first)
{
  DefinedValue result{{}, Type{0, {PropertyList::getPrimitiveProperty<std::remove_reference_t<T>>()}}};

  // according c++11 std::forward
  result.value.emplace_back(std::forward<T>(first));
  return result;
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

template<class T>
constexpr bool isUniquePtr(std::unique_ptr<T> const &)
{
  return true;
}

template<class T>
constexpr bool isUniquePtr(T const &)
{
  return false;
}

inline Value copy(Value const &v)
{
  Value result;

  for (auto const &p : v)
    std::visit([&result](auto const &p)
	       {
		 if constexpr (isUniquePtr(p))
				result.emplace_back(new std::remove_reference_t<decltype(*p)>(*p));
		 else
		   result.emplace_back(p);
	       }, p);
  return std::move(result);
}

inline DefinedValue copy(DefinedValue const &v)
{
  return DefinedValue{copy(v.value), v.type};
}

