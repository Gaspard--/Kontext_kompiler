#pragma once

#include <string>
#include <vector>
#include <memory>
#include <iostream>

#include "Stack.hpp"
#include "Lookup.hpp"
#include "Value.hpp"

class Kompiler
{
private:
  enum class TokenType
    {
      CONSTANT,
      NAME,
      OPERATOR,
    };

  struct Token
  {
    std::string content;
    TokenType type;
  };

  Stack stack;
  Lookup<Variable> prefixes;
  Lookup<Operator> suffixes;

public:
  Kompiler() = default;
  ~Kompiler() = default;

  auto interpretConstant(std::string const &str)
  {
    CompilerValue<int> result(0u);

    for (char const c : str)
      {
	if ((unsigned)c - '0' >= '9')
	  throw std::runtime_error("Non numeric character in numeric constant (Not supported yet).\n");
	*result *= 10u;
	*result += (unsigned)c - '0';
      }
    return result;
  }

  Variable evaluatePrefixVariable(Token const &token)
  {
    switch (token.type)
      {
      case TokenType::CONSTANT:
	return (interpretConstant(token.content));
      default:
	try {
	  return prefixes[token.content];
	} catch (std::out_of_range const &) {
	  return {nullptr};
	}
      }
  }

  // TODO
  auto lookupSuffix(Token const &token)
  {
    try {
      return suffixes[token.content];
    } catch (std::out_of_range const &) {
      return {nullptr};
    }
  }

  template<class IT>
  std::unique_ptr<Value> evaluateTokens(IT begin, IT end)
  {
    if (begin == end)
      throw std::runtime_error("Can't evaluate nothing yet :/");
    std::stack<std::unique_ptr<PrefixOp>> prefixOpStack;
    std::unique_ptr<Value> value;

    do {
      evaluatePrefixVariable(*begin).release()->store(value, prefixOpStack);
      ++begin;
      if (begin == end)
	throw std::runtime_error("Can't evaluate nothing yet :/");
      if (value)
	{
	  do {
	    auto suffix(lookupSuffix(*begin));

	    if (!prefixOpStack.empty() && (!suffix || *prefixOpStack.top() > *suffix))
	      {
		prefixOpStack.top()->apply(*value).release()->store(value, prefixOpStack);
		prefixOpStack.pop();
	      }
	    else if (suffix)
	      {
		suffix->apply(*value).release()->store(value, prefixOpStack);
		++begin;
	      }
	    else if (begin == end)
	      return value;
	    else
	      throw std::runtime_error("Value can't be followed by a prefix context type");
	  } while (value);
	}
    } while (true);
  }

  template<class IT>
  void processTokens(IT begin, IT end)
  {
    for (auto it(begin); it != end; ++it)
      std::cout << it->content << " : " << (size_t)it->type << std::endl;
    evaluateTokens(begin, end);
  }

  template<class IT>
  void processLine(IT begin, IT end)
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
