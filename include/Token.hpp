#pragma once

enum class TokenType
  {
    CONSTANT,
    NAME,
    OPERATOR
  };

struct Token
{
  std::string content;
  TokenType type;
};

inline std::ostream &operator<<(std::ostream &out, Token const &token)
{
  out << "Token[" << token.content << "]";
  return out;
}
