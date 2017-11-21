#pragma once

#include <string>
#include <vector>
#include <memory>
#include <iostream>

#include "Stack.hpp"
#include "Lookup.hpp"
// #include "Properties.hpp"
#include "Value.hpp"

#define STRINGIZE_DETAIL(x) #x
#define STRINGIZE(x) STRINGIZE_DETAIL(x)

class Kompiler
{
private:
  enum class TokenType
    {
      CONSTANT,
      NAME,
      OPERATOR
    };

  struct Token
  {
    std::string_view content;
    TokenType type;
  };

  Stack stack;
  std::unordered_map<std::string, UnaryOperator> prefixes;
  std::unordered_map<std::string, std::pair<Value, Type>> values;
  std::unordered_map<std::string, UnaryOperator> postfixes;
  PropertyList propertyList;

public:
  Kompiler() = default;
  ~Kompiler() = default;
  

  // Find a value within an expression
  template<class DestructiveIT, class ConstrutiveIT, class EndIT>
  std::pair<Value, Type> const &getValue(DestructiveIT &begin, ConstrutiveIT &it, EndIT const &end)
  {
    while (it != end) {
      auto valueIt(values.find(std::string(it->content)));

      if (valueIt != values.end())
	return valueIt->second;
      ++it;
    }
    throw std::runtime_error("No value found in expression!");
  }
  
  static UnaryOperator const &getUnaryOperator(std::string_view str, std::unordered_map<std::string, UnaryOperator> const &unaryOperators)
  {
    try {
      return unaryOperators.at(std::string(str));
    } catch (std::out_of_range const &) {
      return UnaryOperator::getUnapplyable();
    }
  }
  
  long unsigned int getTypeCastCost(Type const &source, Type const &dest)
  {
    auto possessed(source.properties);

    return (propertyList.getCost(possessed, dest.properties));
  }


  // Contract:
  // - only values between `begin` and `it` will be accessed.
  // - both will be incremented, add_assigned, or assigned to greater values
  // - both won't be incremented further than `end`
  // - copies of `it` may be decremented and will always be between `begin` and `it`, and should not construct or destruct elements
  // - all threee iterators have to be comparable.
  // - ConstructiveIt and DestructiveIt shall not be copied
  template<class DestructiveIT, class ConstrutiveIT, class EndIT, class NormalItCaster>
  std::pair<Value, Type> evaluateTokens(DestructiveIT &begin, ConstrutiveIT &it, EndIT const &end, NormalItCaster const &getNormalIt, UnaryOperator const &prevPrefix = UnaryOperator::getUnapplyable(), Value const &prevStored = {})
  {
    if (begin == end)
      throw std::runtime_error("Can't evaluate nothing yet :/");
    auto prevPrefixBegin(begin);
    auto value(getValue(begin, it, end));
    auto prefixIt((it >= begin + 1) ?
		  getNormalIt(it - 1) :
		  getNormalIt(end));

    ++it;
    while (prefixIt != end && it != end)
      {
	UnaryOperator const &unaryPrefix((prefixIt == end) ? prevPrefix : getUnaryOperator(prefixIt->content, prefixes));
	UnaryOperator const &unaryPostfix((it == end) ? UnaryOperator::getUnapplyable() : getUnaryOperator(it->content, postfixes));

	bool which(0u);
	long unsigned int bestCost(~0u);
	UnaryFunction const *bestFunc(nullptr);

	for (auto &func : unaryPrefix.data)
	  {
	    long unsigned int currentCost(getTypeCastCost(func.signature.paramType, value.second));

	    if (currentCost < bestCost)
	      {
		bestCost = currentCost;
		bestFunc = &func;
	      }
	  }
	for (auto &func : unaryPostfix.data)
	  {
	    long unsigned int currentCost(getTypeCastCost(func.signature.paramType, value.second));

	    if (currentCost < bestCost)
	      {
		bestCost = currentCost;
		bestFunc = &func;
		which = 1;
	      }
	  }
	value = std::visit([this, &value, bestFunc, prevStored, getNormalIt, &it, end](auto const &returnType)
			   {
			     using T = std::remove_cv_t<std::remove_reference_t<decltype(returnType)>>;
			     constexpr bool isValue(std::is_same_v<T, Type>);
			     constexpr bool isPrefix(std::is_same_v<T, UnaryOperator>);

			     static_assert(isValue || isPrefix, "Unhandled type in " __FILE__  ": " STRINGIZE(__LINE__));
			     if constexpr (isValue)
			     {
			       return std::pair<Value, Type>{bestFunc->func(prevStored, value.first), returnType};
			     }
			     else
			     {
			       auto itCopy(getNormalIt(it));
			       return evaluateTokens(itCopy, it, end, getNormalIt, returnType, bestFunc->func(prevStored, value.first));
			     }
			   }, bestFunc->signature.returnType);
	if (!which)
	  {
	    if (prefixIt == begin)
	      {
		prefixIt = end;
		begin = it;
	      }
	    else if (prefixIt == end)
	      return value; // there was a prefix to apply
	    else
	      --prefixIt;
	  }
	else
	  ++it;
      }
  }
  
  template<class IT>
  void processTokens(IT begin, IT end)
  {
    IT beginCopy(begin);
    for (auto it(begin); it != end; ++it)
      std::cout << it->content << " : " << (size_t)it->type << std::endl;
    evaluateTokens(begin, beginCopy, end, [](auto const &a)
		   {
		     return static_cast<std::remove_cv_t<std::remove_reference_t<decltype(a)>>>(a);
		   });
  }

  // TODO: make process more lazy
  void processLine(std::string_view line)
  {
    auto isDigit([](char c)
		 {
		   return (((unsigned)c - '0') <= '9' - '0');
		 });
    auto isName([](char c)
		{
		  return ((((unsigned)c - 'A') <= 'Z' - 'A')
			  || (((unsigned)c - 'a') <= 'z' - 'a')
			  || c == '_');
		});
    auto isWhiteSpace([](char c)
		      {
			return c == ' ' || c == '\t' || c == '\n';
		      });
    std::vector<Token> tokens;
    auto begin(line.begin());
    auto end(line.end());
    
    for (;begin != end && isWhiteSpace(*begin); ++begin);
    if (begin == end)
      {
	std::cout << "line empty" << std::endl;
	return ;
      }
    while (begin != end)
      {
	if (isDigit(*begin))
	  {
	    auto it(begin);

	    for (; it != end && (isDigit(*it) || isName(*it)); ++it);
	    tokens.push_back({std::string(begin, it), TokenType::CONSTANT});
	    begin = it;
	  }
	else if (isName(*begin))
	  {
	    auto it(begin);

	    for (; it != end && (isDigit(*it) || isName(*it)); ++it);
	    tokens.push_back({std::string(begin, it), TokenType::NAME});
	    begin = it;
	  }
	else
	  {
	    auto it(begin);

	    for (; it != end && !isDigit(*it) && !isName(*it) && !isWhiteSpace(*it); ++it);
	    tokens.push_back({std::string(begin, it), TokenType::OPERATOR});
	    begin = it;
	  }
	for (; begin != end && isWhiteSpace(*begin); ++begin);
      }
    return processTokens(tokens.begin(), tokens.end());
  }

  void process(std::istream &);
};
