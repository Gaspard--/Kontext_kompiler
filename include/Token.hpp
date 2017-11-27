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

