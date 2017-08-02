#pragma once

#include <algorithm>
#include <map>

template<class IT>
struct TokenRange
{
  IT begin;
  IT end;
  size_t index;

  constexpr TokenRange(IT begin, IT end, size_t index = 0u)
  : begin(begin), end(end), index(index)
  {};

  constexpr auto operator[](char c) const
  {
    auto lower(std::lower_bound(begin, end, c, [this](auto const &pair, char c)
				{
				  return (pair.first.length() >= index && pair.first[index] < c);
				}));
    auto upper(lower);

    while (upper != end && upper->first.length() >= index && upper->first[index] == c)
      ++upper;
    return TokenRange<IT>{lower, upper, index + 1};
  }

  auto operator[](std::string const &str) const
  {
    auto lower(std::lower_bound(begin, end, str, [this](auto const &pair, std::string const &str)
				{
				  return (pair.first.length() >= index + str.length() &&
					  std::lexicographical_compare(pair.first.cbegin() + index, pair.first.cbegin() + index + str.length(),
								       str.cbegin(), str.cend()));
				}));
    auto upper(lower);

    while (upper != end && upper->first.length() >= index + str.length()
	   && std::equal(str.cbegin(), str.cend(), upper->first.cbegin() + index, upper->first.cbegin() + index + str.length()))
      ++upper;
    return TokenRange<IT>{lower, upper, index + str.length()};
  }

  constexpr bool hasExactMatch() const
  {
    return !isEmpty()&& begin->first.length() == index;
  }

  constexpr bool isEmpty() const
  {
    return begin == end;
  }
};

template<class IT>
constexpr auto makeTokenRange(IT begin, IT end)
{
  return TokenRange<IT>(begin, end);
}

template<class MAP>
constexpr auto makeTokenRange(MAP map)
{
  return makeTokenRange(map.begin(), map.end());
}

template<class Token>
class Lookup
{
private:
  std::map<std::string, Token> tokenMap;

public:
  Lookup() = default;

  auto getRootTokenRange()
  {
    return makeTokenRange(tokenMap.begin(), tokenMap.end());
  }

  template<class... PARAMS>
  auto emplaceToken(std::string const &key, PARAMS &&... params)
  {
    return tokenMap.emplace(key, std::forward<PARAMS>(params)...);
  }
};
