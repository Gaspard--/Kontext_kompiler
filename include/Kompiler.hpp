#pragma once

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <list>

#include "Stack.hpp"
#include "Lookup.hpp"
// #include "Properties.hpp"
#include "Value.hpp"

#define STRINGIZE_DETAIL(x) #x
#define STRINGIZE(x) STRINGIZE_DETAIL(x)

class Kompiler
{
private:
  Stack stack;
  std::unordered_map<std::string, UnaryOperator> prefixes;
  std::unordered_map<std::string, DefinedValue> values;
  std::unordered_map<std::string, UnaryOperator> postfixes;
  PropertyList propertyList;

public:
  Kompiler()
  {
    prefixes["ID_DEBUG"].addFunc({{}, [](Value const &, DefinedValue const &val)
					-> std::pair<Value, std::variant<Type, UnaryOperator>>
					{
					  std::cout << "Applying postfix!\n";
					  return {val.value, val.type};
					}});
    postfixes["ID_DEBUG"].addFunc({{}, [](Value const &, DefinedValue const &val)
					 -> std::pair<Value, std::variant<Type, UnaryOperator>>
					 {
					   std::cout << "Applying postfix!\n";
					   return {val.value, val.type};
					 }});
  }

  ~Kompiler() = default;

  // Find a value within an expression
  template<class ConstrutiveIT, class EndIT>
  DefinedValue getValue(ConstrutiveIT &it, EndIT const &end)
  {
    while (it != end) {
      // std::cout << "Looking up value: " << it->content << std::endl;
      auto prefixIt(prefixes.find(it->content));

      if (prefixIt == prefixes.end())
	{
	  auto valueIt(values.find(it->content));

	  if (valueIt != values.end())
	    return valueIt->second;
	  return makePrimitiveDefinedValue(std::make_shared<Token>(*it));
	}
      ++it;
    }
    throw std::runtime_error("No value found in expression!");
  }

  static UnaryOperator const &getUnaryOperator(std::string const &str, std::unordered_map<std::string, UnaryOperator> const &unaryOperators)
  {
    try {
      return unaryOperators.at(str);
    } catch (std::out_of_range const &) {
      return UnaryOperator::getUnapplyable();
    }
  }

  long unsigned int getTypeCastCost(PropertyList::Properties source, PropertyList::Properties const &dest)
  {
    return (propertyList.getCost(source, dest));
  }

  // Contract:
  // - only values between `begin` and `it` will be accessed.
  // - both will only be incremented or assigned to greater values
  // - both won't be incremented further than `end`
  // - copies of `it` may be decremented and will always be between `begin` and `it`, and should not construct or destruct elements
  // - all threee iterators have to be comparable, and provide a copy function
  // - ConstructiveIt and DestructiveIt shall not be copied
  template<class DestructiveIT, class ConstrutiveIT, class EndIT>
  DefinedValue evaluateTokens(DestructiveIT &begin, ConstrutiveIT &it, EndIT const &end, UnaryOperator const &prevPrefix = UnaryOperator::getUnapplyable(), Value const &prevStored = {})
  {
    if (it == end)
      throw std::runtime_error("Can't evaluate nothing yet :/");
    DefinedValue value(getValue(it, end));
    std::optional<decltype(it.copy())> prefixIt;
    bool prevPrefixApplied(false);

    if (it != begin)
      --*(prefixIt = it.copy());
    ++it;
    while (!prevPrefixApplied && (prefixIt || it != end))
      {
	UnaryFunction const *bestFunc(nullptr);
	{
	  int which(0u);
	  {
	    UnaryOperator const &unaryPrefix(!prefixIt ? prevPrefix : getUnaryOperator((*prefixIt)->content, prefixes));
	    UnaryOperator const &unaryPostfix((it == end) ? UnaryOperator::getUnapplyable() : getUnaryOperator(it->content, postfixes));

	    long unsigned int bestCost(PropertyList::inf);
	    auto checkIfBetterCandidate([this, &bestCost, &bestFunc, &value](auto &func)
					{
					  long unsigned int currentCost(getTypeCastCost(func.requiredProperties, value.type.properties));

					  if (currentCost < bestCost)
					    {
					      bestCost = currentCost;
					      bestFunc = &func;
					      return true;
					    }
					  return false;
					});

	    for (auto &func : unaryPrefix.data)
	      checkIfBetterCandidate(func);
	    for (auto &func : unaryPostfix.data)
	      which |= checkIfBetterCandidate(func);
	  }
	  if (!bestFunc)
	    throw std::runtime_error("No (Prefix or Postfix) UnaryOperator to appliable!");
	  if (!which)
	    {
	      if (begin == *prefixIt)
		{
		  prefixIt.reset();
		  begin = it.copy();
		}
	      else if (prefixIt)
		--*prefixIt;
	      else
		prevPrefixApplied = true;
	    }
	  else
	    {
	      ++it;
	      if (!prefixIt)
		++begin;
	    }
	}
	std::pair<Value, std::variant<Type, UnaryOperator>> returnValueAndType{bestFunc->func(prevStored, value)};

	value = std::visit([this, &it, end, &returnValueAndType](auto const &returnType)
			   {
			     using T = std::remove_cv_t<std::remove_reference_t<decltype(returnType)>>;
			     constexpr bool isValue(std::is_same_v<T, Type>);
			     constexpr bool isPrefix(std::is_same_v<T, UnaryOperator>);

			     static_assert(isValue || isPrefix, "Unhandled type in " __FILE__  ": " STRINGIZE(__LINE__));
			     if constexpr (isValue)
			       return DefinedValue{returnValueAndType.first, returnType};
			     else
			       {
				 auto itCopy(it.copy());

				 return evaluateTokens(itCopy, it, end, returnType, returnValueAndType.first);
			       }
			   }, returnValueAndType.second);
      }
    return value;
  }

  void parseLine(std::string const &str);
  
  void process(std::istream &);
};
