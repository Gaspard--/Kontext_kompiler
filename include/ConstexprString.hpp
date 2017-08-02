#pragma once

template<size_t len>
class ConstexprString
{
  char data[len];

public:
  constexpr ConstexprString(char data[len])
  {
  }
};

template<size_t len>
constexpr makeConstexprString(char data[len])
{
  return ConstexprString<len>(data);
}
