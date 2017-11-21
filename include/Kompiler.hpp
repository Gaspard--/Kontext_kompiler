#pragma once

#include <string>
#include <vector>
#include <memory>
#include <iostream>

#include "Stack.hpp"
#include "Lookup.hpp"
// #include "Properties.hpp"
#include "Value.hpp"

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
  std::unordered_map<std::string, UnaryFunction> prefixes;
  std::unordered_map<std::string, Value> values;
  std::unordered_map<std::string, UnaryFunction> postfixes;
  // PropertyList propertyList;

public:
  Kompiler() = default;
  ~Kompiler() = default;
  

  // Find a value within an expression
  template<class DestructiveIT, class ConstrutiveIT, class EndIT>
  Value &getValue(DestructiveIT &begin, ConstrutiveIT &it, EndIT const &end)
  {
    while (it != end) {
      auto valueIt(values.find(std::string(it->content)));

      if (valueIt != values.end())
	return valueIt->second;
      ++it;
    }
    throw std::runtime_error("No value found in expression!");
  }

  // Contract:
  // - only values between `begin` and `it` will be accessed.
  // - both will be incremented, add_assigned, or assigned to greater values
  // - both won't be incremented further than `end`
  // - 
  // - copies of `it` may be decremented
  // - all threee iterators have to be comparable.
  template<class DestructiveIT, class ConstrutiveIT, class EndIT>
  Value evaluateTokens(DestructiveIT &begin, ConstrutiveIT &it, EndIT const &end)
  {
    if (begin == end)
      throw std::runtime_error("Can't evaluate nothing yet :/");
    auto prevPrefixBegin(begin);
    auto &value(getValue(begin, it, end));
    ++it;
    auto prefixIt(it);

    if (it - 2 >= begin)
      prefixIt -= 2;
    else
      prefixIt = begin;
    
    // UnaryFunction *unaryPrefix((prefixIt <= begin) ? nullptr : getUnaryPreffix((--prefixIt)->content));
    // UnaryFunction *unaryPostfix((it == end) ? nullptr : getUnaryPostfix(it->content));
    // auto evaluateCandidateWorth([](auto &typeAndFunc){
    // 	auto &type(typeAndFunc.first.paramType);

    //   });

    // do {
    //   if (it == end)
    // 	return value;
    //   else
    // 	throw std::runtime_error("Value can't be followed by a prefix context type");
    // } while (value);
    // if (begin == end)
    //   throw std::runtime_error("Can't evaluate nothing yet :/");
  }
  
  template<class IT>
  void processTokens(IT begin, IT end)
  {
    for (auto it(begin); it != end; ++it)
      std::cout << it->content << " : " << (size_t)it->type << std::endl;
    evaluateTokens(begin, begin, end);
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
