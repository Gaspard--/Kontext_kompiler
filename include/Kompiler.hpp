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
public:

private:

  Stack stack;
  std::unordered_map<std::string, UnaryOperator> prefixes;
  std::unordered_map<std::string, std::pair<Value, Type>> values;
  std::unordered_map<std::string, UnaryOperator> postfixes;
  PropertyList propertyList;

public:
  Kompiler() = default;
  ~Kompiler() = default;

  // Find a value within an expression
  template<class ConstrutiveIT, class EndIT>
  std::pair<Value, Type> getValue(ConstrutiveIT &it, EndIT const &end)
  {
    while (it != end) {
      std::cout << "Looking up value: " << it->content << std::endl;
      auto prefixIt(prefixes.find(it->content));

      if (prefixIt == prefixes.end())
	{
	  auto valueIt(values.find(it->content));

	  if (valueIt != values.end())
	    return valueIt->second;
	  return makePrimitiveValueAndType(std::make_shared<Token>(*it));
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
  std::pair<Value, Type> evaluateTokens(DestructiveIT &begin, ConstrutiveIT &it, EndIT const &end, UnaryOperator const &prevPrefix = UnaryOperator::getUnapplyable(), Value const &prevStored = {})
  {
    if (it == end)
      throw std::runtime_error("Can't evaluate nothing yet :/");
    auto value(getValue(it, end));
    std::optional<decltype(it.copy())> prefixIt;

    if (it != begin)
      --*(prefixIt = it.copy());
    ++it;
    while (prefixIt && it != end)
      {
	UnaryOperator const &unaryPrefix(prefixIt ? prevPrefix : getUnaryOperator((*prefixIt)->content, prefixes));
	UnaryOperator const &unaryPostfix((it == end) ? UnaryOperator::getUnapplyable() : getUnaryOperator(it->content, postfixes));

	bool which(0u);
	long unsigned int bestCost(PropertyList::inf);
	UnaryFunction const *bestFunc(nullptr);

	for (auto &func : unaryPrefix.data)
	  {
	    long unsigned int currentCost(getTypeCastCost(func.signature.properties, value.second.properties));

	    if (currentCost < bestCost)
	      {
		bestCost = currentCost;
		bestFunc = &func;
	      }
	  }
	for (auto &func : unaryPostfix.data)
	  {
	    long unsigned int currentCost(getTypeCastCost(func.signature.properties, value.second.properties));

	    if (currentCost < bestCost)
	      {
		bestCost = currentCost;
		bestFunc = &func;
		which = 1;
	      }
	  }
	if (!bestFunc)
	  throw std::runtime_error("No prefix or postfix to apply :(");
	std::pair<Value, std::variant<Type, UnaryOperator>> returnValueAndType{bestFunc->func(prevStored, value.first)};

	value = std::visit([this, &value, bestFunc, prevStored, &it, end, &returnValueAndType](auto const &returnType)
			   {
			     using T = std::remove_cv_t<std::remove_reference_t<decltype(returnType)>>;
			     constexpr bool isValue(std::is_same_v<T, Type>);
			     constexpr bool isPrefix(std::is_same_v<T, UnaryOperator>);

			     static_assert(isValue || isPrefix, "Unhandled type in " __FILE__  ": " STRINGIZE(__LINE__));
			     if constexpr (isValue)
			       return std::pair<Value, Type>{returnValueAndType.first, returnType};
			     else
			       {
				 auto itCopy(it.copy());

				 return evaluateTokens(itCopy, it, end, returnType, returnValueAndType.first);
			       }
			   }, returnValueAndType.second);
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
	      return value; // there was no prefix to apply
	  }
	else
	  {
	    ++it;
	    if (!prefixIt)
	      ++begin;
	  }
      }
    return value;
  }

  void parseLine(std::string const &str);
  
  void process(std::istream &);
};

#include "Lexer.hpp"
