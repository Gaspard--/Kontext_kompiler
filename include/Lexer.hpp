#pragma once

#include "Token.hpp"

constexpr inline bool isWhiteSpace(char c)
{
  return c == ' ' || c == '\t' || c == '\n';
}

constexpr inline bool isDigit(char c)
{
  return (((unsigned)c - '0') <= '9' - '0');
}

constexpr inline bool isName(char c)
{
  return ((((unsigned)c - 'A') <= 'Z' - 'A')
	  || (((unsigned)c - 'a') <= 'z' - 'a')
	  || c == '_');
}

struct EndIt{};

struct DestructiveIt
{
  std::list<Token> &tokens;

  auto &operator++()
  {
    tokens.pop_front();
    return *this;
  }

  auto &operator=(std::list<Token>::iterator const &other)
  {
    tokens.erase(tokens.begin(), other);
    return *this;
  }

  bool operator==(std::list<Token>::iterator const &it) const
  {
    return tokens.begin() == it;
  }

  bool operator!=(std::list<Token>::iterator const &it) const
  {
    return !(*this == it);
  }
};

struct ConstructiveIt
{
  std::list<Token> &tokens; // TODO: replace with something better (needs to not invalidate iterators though.)
  std::string::const_iterator begin;
  std::string::const_iterator end;
  bool endReached;
  
  ConstructiveIt &operator++()
  {
    for (; begin != end && isWhiteSpace(*begin); ++begin);
    if (begin == end)
      endReached = true;
    else
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
	std::cout << "Created new token: " << tokens.back().content << std::endl;
      }
    return *this;
  }

  auto &operator*()
  {
    return tokens.back();
  }

  auto *operator->()
  {
    return &tokens.back();
  }

  auto copy()
  {
    auto tmp(tokens.end());
    return --tmp;
  }

  bool operator==(EndIt const &) const
  {
    return endReached;
  }

  bool operator!=(EndIt const &) const
  {
    return !endReached;
  }

  bool operator==(std::list<Token>::iterator const & it) const
  {
    return tokens.end() != it;
  }

  bool operator!=(std::list<Token>::iterator const& it) const
  {
    return !(*this == it);
  }

  bool operator==(DestructiveIt const &) const
  {
    return tokens.size() == 1;
  }
    
  bool operator!=(DestructiveIt const &other) const
  {
    return !(*this == other);
  }
};

inline void Kompiler::parseLine(std::string const &str)
{
  std::cout << "Parsing: " << str << std::endl;
  std::list<Token> tokens{};

  ConstructiveIt constructiveIt{tokens, str.begin(), str.end(), false};
  ++constructiveIt;
  DestructiveIt destructiveIt{tokens};
  EndIt endIt{};

  auto value(evaluateTokens(destructiveIt, constructiveIt, endIt));

  if (constructiveIt != endIt)
    throw std::runtime_error("Not all tokens where consumed!");
}
