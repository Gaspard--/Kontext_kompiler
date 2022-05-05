#pragma once

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <list>
#include <optional>

#include "Stack.hpp"
#include "Lookup.hpp"
#include "Type.hpp"

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
    createDefaultDefinitions();
  }

  ~Kompiler() = default;

  void createDefaultDefinitions();

  // Find a value within an expression
  template<class ConstrutiveIT, class EndIT>
  DefinedValue getValue(ConstrutiveIT &it, EndIT const &end)
  {
    while (it != end) {
      auto prefixIt(prefixes.find(it->content));

      if (prefixIt == prefixes.end())
	{
	  // std::cout << "Looking up value: " << it->content << std::endl;
	  auto valueIt(values.find(it->content));

	  if (valueIt != values.end())
	    return copy(valueIt->second);
	  return makePrimitiveDefinedValue(std::make_unique<Token>(*it));
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


  struct NoEffect
  {
    template<class T>
    NoEffect &operator=(T const)
    {
      return *this;
    }

    NoEffect &operator++()
    {
      return *this;
    }
  };

  NoEffect noEffect;

  std::pair<UnaryFunction const *, int> getBestFunction(UnaryOperator const &unaryPrefix, UnaryOperator const &unaryPostfix, Type const &type)
  {
    UnaryFunction const *bestFunc(nullptr);
    int which(false);
    long unsigned int bestCost(PropertyList::Cost::inf);
    auto checkIfBetterCandidate([this, &bestCost, &bestFunc, &type](auto &func)
				{
				  long unsigned int currentCost(getTypeCastCost(type.properties, func.requiredProperties));

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
    return {bestFunc, which};
  }

  // Contract:
  // - only values between `begin` and `it` will be accessed.
  // - both will only be incremented or assigned to greater values
  // - both won't be incremented further than `end`
  // - copies of `it` may be decremented and will always be between `begin` and `it`, and should not construct or destruct elements
  // - all threee iterators have to be comparable, and provide a copy function
  // - ConstructiveIt and DestructiveIt shall not be copied
  template<class DestructiveIT, class ConstrutiveIT, class EndIT>
  DefinedValue evaluateTokens(DestructiveIT &destroyer, ConstrutiveIT &it, EndIT const &end, UnaryOperator const &prevPrefix, Value &&prevStored)
  {
    if (it == end)
      throw std::runtime_error("Can't evaluate nothing yet :/");

    auto const begin(it.copy());
    DefinedValue value(getValue(it, end));
    std::optional<decltype(it.copy())> prefixIt;
    bool prevPrefixApplied(false);

    if (it != begin)
      --*(prefixIt = it.copy());
    ++it;
    do
      {
	UnaryFunction const *bestFunc(nullptr);
	bool which;
	std::tie(bestFunc, which) = getBestFunction(!prefixIt ? prevPrefix : getUnaryOperator((*prefixIt)->content, prefixes),
						    (it == end) ? UnaryOperator::getUnapplyable() : getUnaryOperator(it->content, postfixes),
						    value.type);
	if (!bestFunc)
	  throw std::runtime_error("No (Prefix or Postfix) UnaryOperator to appliable!");
	if (which)
	  {
	    ++it;
	    if (!prefixIt)
	      ++destroyer;
	  }
	else
	  if (!prefixIt)
	    prevPrefixApplied = true;
	  else if (begin == *prefixIt)
	    {
	      prefixIt.reset();
	      destroyer = it.copy();
	    }
	  else
	    --*prefixIt;
	value = std::visit([this, &it, end](auto &&ret) noexcept
			   -> DefinedValue
			   {
			     using T = std::remove_cv_t<std::remove_reference_t<decltype(ret)>>;

			     if constexpr (std::is_same_v<T, DefinedValue>)
			       return std::move(ret);
			     else
			       return evaluateTokens(noEffect, it, end, ret.second, std::move(ret.first));
			   }, bestFunc->func(*this, std::move(prevStored), std::move(value)));
      } while (!prevPrefixApplied);
    return value;
  }

  void parseLine(std::string const &str);

  void process(std::istream &);
};
