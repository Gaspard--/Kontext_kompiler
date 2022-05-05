#pragma once

#include "Value.hpp"
#include "Properties.hpp"

#include <cassert>

struct Kompiler;
struct Type;

struct StructDefinition
{
  std::vector<std::pair<std::string, Type>> fields;
};

struct Type
{
  std::variant<StructDefinition, PropertyList::PropertyId> dataType;
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
  DefinedValue result{{}, Type{PropertyList::getPrimitiveProperty<std::remove_reference_t<T>>(), {PropertyList::getPrimitiveProperty<std::remove_reference_t<T>>()}}};

  result.value.emplace_back(std::move(first));
  return result;
}

inline std::ostream &operator<<(std::ostream &out, Type const &type);

inline std::ostream &operator<<(std::ostream &out, StructDefinition const &type)
{
  for (auto const &[name, value] : type.fields)
    {
      out << name << ": " << value << " - ";
    }
  return out;
}

inline std::ostream &operator<<(std::ostream &out, Type const &type)
{
  if (auto *structDefinition = std::get_if<StructDefinition>(&type.dataType))
    {
      out << structDefinition;
    }
  else
    {
      unsigned int primitiveId = std::get<unsigned int>(type.dataType);
#define X(a) #a
      char const *names[] = {KOMPILER_PRIMITIVE_X_LIST};
#undef X

      assert(primitiveId < sizeof(names));
      if (primitiveId < sizeof(names))
	{
	  out << names[primitiveId];
	}
      else
	{
	  out << "<Unknown type>";
	}
    }
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

inline Value copy(Value const &v)
{
  Value result;

  for (auto const &p : v)
    std::visit([&result](auto const &p)
	       {
           if constexpr (requires {*p;})
             result.emplace_back(std::remove_const_t<std::remove_reference_t<decltype(p)>>(new std::remove_reference_t<decltype(*p)>(*p)));
           else
             result.emplace_back(p);
	       }, p);
  return result;
}

inline DefinedValue copy(DefinedValue const &v)
{
  return DefinedValue{copy(v.value), v.type};
}
