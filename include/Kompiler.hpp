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
    std::string_view content;
    TokenType type;
  };

  Stack stack;
  std::unordered_map<std::string, std::vector<Value>> prefixes;
  std::unordered_map<std::string, Value> prefixes;
  std::unordered_map<std::string, std::vector<Value>> suffixes;
  TypeList typeList;

public:
  Kompiler() = default;
  ~Kompiler() = default;


    // struct Possibillity
    // {
    //   std::stack<Value *> prefixStack;
    //   std::optional<Value *> value;

    //   void processValue(auto &token, auto &suffixes, auto &prefixes, auto &typeList)
    //   {
    // 	if (!value)
    // 	  {
    // 	    auto &prefix(prefixes[token.content]);

    // 	    std::visit([this, &value, &prefix](auto &&prefix)
    // 		       {
    // 			 if constexpr (std::is_same_v<std::remove_cv_t<std::remove_reference_t<decltype(tokenType)>>, Value>)
    // 			   {
    // 			     value = prefix;
    // 			   }
    // 			 else
    // 			   ;
    // 		       }, std::move(prefix));

    // 	    std::visit([this, &value, &prefix](auto &&tokenType)
    // 		       {
    // 			 if constexpr (std::is_same_v<std::remove_cv_t<std::remove_reference_t<decltype(tokenType)>>, Function>)
    // 			   {
    // 			     prefixStack.push_back(prefix);
    // 			   }
    // 			 else
    // 			   value = prefix;
    // 		       }, typeList[prefix.type]);
    // 	  }
    // 	else
    // 	  {
    // 	    auto &suffix(suffixes[token.content]);

    // 	    if (std::get<Function>(typeList[suffix.type]) > std::get<Function>(typeList[prefixStack.type]))
    // 	      ;
    // 	  }
    // 	std::visit([this, value](auto &&lastType)
    // 		   {
    // 		     if constexpr (std::is_same_v<std::remove_cv_t<std::remove_reference_t<decltype(lastValueType)>>, Function>)
    // 		       {
    // 			 for (auto const &prefix : prefixes[std::string(begin->content)])
    // 			   if (lastValueType <= typeList[std::get<Function>(typeList[prefix.type]).returnType])
    // 			     addNode(prefix, false);
    // 		       }
    // 		     else
    // 		       {
    // 			 for (unsigned int j(0u);)
    // 			   if (lastWasSuffix)
    // 			     {
    // 			       for (auto const &suffix : suffixes[std::string(begin->content)])
    // 				 if (typeList[std::get<Function>(typeList[suffix.type]).paramType] <= lastValueType)
    // 				   addNode(suffix);
    // 			     }
    // 			   else
    // 			     for (auto const &suffix : suffixes[std::string(begin->content)])
    // 			       {
    // 			       }
    // 		       }
    // 		   }, typeList[lastNode.value.type]);
    //   }
    // };


  template<class IT>
  Value evaluateTokens(IT begin, IT end)
  {
    if (begin == end)
      throw std::runtime_error("Can't evaluate nothing yet :/");
    while (begin != end)
      {
	if (values.find(begin->content) != values.end())
	  {
	    // yay!
	    goto value_found_label
	  }
      }
    std::deque<std::shared_ptr<Function>> prefixOpStack

  value_found_label:
      
    
    // do {
    //   auto &prev(typeStack.back());
    //   typeStack.push_back({});
    //   auto &current(typeStack.back());

    //   for (unsigned int i(0u); i != prev.size(); ++i)
    // 	{
    // 	  auto const &lastNode(prev[i]);
    // 	  auto const addNode([&current, i](Value &value)
    // 			     {
    // 			       current.emplace(current.size(), {i, {}, suffix});
    // 			       lastNode.children.push_back(current.size() - 1);
    // 			     });

    // 	  std::visit([this, &typeStack, &lastNode, begin, addNode](auto &&lastType)
    // 		     {
    // 		       if constexpr (std::is_same_v<std::remove_cv_t<std::remove_reference_t<decltype(lastValueType)>>, Function>)
    // 			 {
    // 			   for (auto const &prefix : prefixes[std::string(begin->content)])
    // 			     if (lastValueType <= typeList[std::get<Function>(typeList[prefix.type]).returnType])
    // 			       addNode(prefix, false);
    // 			 }
    // 		       else
    // 			 {
    // 			   for (unsigned int j(0u);)
    // 			     if (lastWasSuffix)
    // 			       {
    // 				 for (auto const &suffix : suffixes[std::string(begin->content))
    // 				   if (typeList[std::get<Function>(typeList[suffix.type]).paramType] <= lastValueType)
    // 				     addNode(suffix);
    // 			       }
    // 			     else
    // 			       for (auto const &suffix : suffixes[std::string(begin->content)])
    // 				 {
    // 				 }
    // 			 }
    // 		     }, typeList[lastNode.value.type]);
    // 	}
    //   ++begin;
      // std::copy_if(.begin(), .end(), typeStack.back().begin(),
      // 		   [](auto const &value)
      // 		   {
      // 		     return value.type;
      // 		   });
      // if (std::visit([&prefixOpStack, &begin, &value](auto &&prefix)
      // 		     {
      // 		       using T = std::remove_cv_t<std::remove_reference_t<decltype(prefix)>>;

      // 		       ++begin;
      // 		       if constexpr (std::is_same_v<T, Function>)
      // 			 {
      // 			   prefixOpStack.push_back(prefix);
      // 			   return false;
      // 			 }
      // 		       else
      // 			 {
      // 			   value = prefix;
      // 			   return true;
      // 			 }
      // 		     }, typeList[prefixes[std::string(begin->content)].back().type]))
      // 	{
      // do {
      // auto suffix(lookupSuffix(*begin));

      // if (!prefixOpStack.empty() && (!suffix || prefixOpStack.back() > *suffix))
      //   {
      // 	prefixOpStack.top()->apply(*value).release()->store(value, prefixOpStack);
      // 	prefixOpStack.pop();
      //   }
      // else if (suffix)
      //   {
      // 	suffix->apply(*value).release()->store(value, prefixOpStack);
      // 	++begin;
      //   }
      // else if (begin == end)
      //   return value;
      // else
      //   throw std::runtime_error("Value can't be followed by a prefix context type");
      // } while (value);
      // }
    //   if (begin == end)
    // 	throw std::runtime_error("Can't evaluate nothing yet :/");
    // } while (true);
  }
  
  template<class IT>
  void processTokens(IT begin, IT end)
  {
    for (auto it(begin); it != end; ++it)
      std::cout << it->content << " : " << (size_t)it->type << std::endl;
    evaluateTokens(begin, end);
  }

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
