#pragma once

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <list>

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
    prefixes["ID_DEBUG"].addFunc({{}, [](Value const &, DefinedValue const &val)
					-> std::variant<DefinedValue, std::pair<Value, UnaryOperator>>
					{
					  std::cout << "Applying prefix!\n";
					  return val;
					}});
    postfixes["ID_DEBUG"].addFunc({{}, [](Value const &, DefinedValue const &val)
					 -> std::variant<DefinedValue, std::pair<Value, UnaryOperator>>
					 {
					   std::cout << "Applying postfix!\n";
					   return val;
					 }});
    postfixes["asInt"].addFunc({{PropertyList::getPrimitiveProperty<std::shared_ptr<Token>>()},
	  [](Value const &, DefinedValue const &val)
	    -> std::variant<DefinedValue, std::pair<Value, UnaryOperator>>
	    {
	      auto const &token(std::get<std::shared_ptr<Token>>(val.value[0]));
	      long unsigned int ret(0u);

	      // TODO: wait for <char_conv> or do something serious
	      for (auto it(token->content.begin()); it != token->content.end(); ++it)
		{
		  ret *= 10;
		  ret += *it - '0';
		}

	      return makePrimitiveDefinedValue(ret);
	    }});
    prefixes["INT_ONLY"].addFunc({{PropertyList::getPrimitiveProperty<long unsigned int>()},
	  [](Value const &, DefinedValue const &val)
	    -> std::variant<DefinedValue, std::pair<Value, UnaryOperator>>
	    {
	      std::cout << "Applying INT_ONLY prefix!\n";
	      
	      return val;
	    }});
    postfixes["+"].addFunc({{PropertyList::getPrimitiveProperty<long unsigned int>()},
	  [](Value const &, DefinedValue const &val)
	    -> std::variant<DefinedValue, std::pair<Value, UnaryOperator>>
	    {
	      UnaryOperator add;

	      add.addFunc({{PropertyList::getPrimitiveProperty<long unsigned int>()},
		    [](Value const &left, DefinedValue const &right)
		      -> std::variant<DefinedValue, std::pair<Value, UnaryOperator>>
		      {
			return makePrimitiveDefinedValue(std::get<long unsigned int>(left[0]) + std::get<long unsigned int>(right.value[0]));
		      }});
	      return std::pair<Value, UnaryOperator>{val.value, add};
	    }});
  }

  ~Kompiler() = default;

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

  
  struct NoEffect
  {
    template<class T>
    NoEffect &operator=(T const)
    {
      return *this;
    };

    NoEffect &operator++()
    {
      return *this;
    };
  };

  NoEffect noEffect;

  // Contract:
  // - only values between `begin` and `it` will be accessed.
  // - both will only be incremented or assigned to greater values
  // - both won't be incremented further than `end`
  // - copies of `it` may be decremented and will always be between `begin` and `it`, and should not construct or destruct elements
  // - all threee iterators have to be comparable, and provide a copy function
  // - ConstructiveIt and DestructiveIt shall not be copied
  template<class DestructiveIT, class ConstrutiveIT, class EndIT>
  DefinedValue evaluateTokens(DestructiveIT &destroyer, ConstrutiveIT &it, EndIT const &end, UnaryOperator const &prevPrefix, Value const &prevStored = {})
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
    while (!prevPrefixApplied)
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
					  long unsigned int currentCost(getTypeCastCost(value.type.properties, func.requiredProperties));

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
	      if (!prefixIt)
		prevPrefixApplied = true;
	      else if (begin == *prefixIt)
		{
		  prefixIt.reset();
		  destroyer = it.copy();
		}
	      else
		--*prefixIt;
	    }
	  else
	    {
	      ++it;
	      if (!prefixIt)
		++destroyer;
	    }
	}
	std::variant<DefinedValue, std::pair<Value, UnaryOperator>> ret{bestFunc->func(prevStored, value)};

	value = std::visit([this, &it, end](auto const &ret)
			   {
			     using T = std::remove_cv_t<std::remove_reference_t<decltype(ret)>>;

			     if constexpr (std::is_same_v<T, DefinedValue>)
			       return ret;
			     else
			       return evaluateTokens(noEffect, it, end, ret.second, ret.first);
			   }, ret);
      }
    return value;
  }

  void parseLine(std::string const &str);
  
  void process(std::istream &);
};
