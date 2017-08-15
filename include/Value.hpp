#pragma once

#include <stack>
#include <memory>

/**
 * Prefix context: VALUE, PREFIX
 * Suffix context: SUFFIX
 * After VALUE: suffix context
 * After PREFIX: prefix context
 */
enum class ValueType
  {
    PREFIX,
    VALUE,
    SUFFIX,
  };

class Variable
{
public:
  Variable() = default;
  Variable(Variable const &) = delete;
  virtual ~Variable() = default;
};

class Value;
class PrefixOp;

/**
 * Can only appear in prefix context
 */
class Prefix : public Variable
{
public:
  constexpr Prefix() = default;
  virtual ~Prefix() = default;

  virtual void store(std::unique_ptr<Value> &value, std::stack<std::unique_ptr<PrefixOp>> &prefixOpStack) = 0;
  // virtual std::unique_ptr<Prefix> processNext(Kompiler const &kompiler, Token const &next) const = 0;
  // virtual std::unique_ptr<Prefix> processNext(Kompiler const &kompiler, Token const &next, Token const &lookAhead) const = 0;
};

class Value : public Prefix
{
public:
  uint64_t valueId;

  constexpr Value(uint64_t valueId)
  : valueId(valueId)
  {
  }

  virtual ~Value() = default;

  virtual void store(std::unique_ptr<Value> &value, std::stack<std::unique_ptr<PrefixOp>> &prefixOpStack) override;
};

class PrefixOp : public Prefix
{
public:
  uint64_t prefixId;

  constexpr PrefixOp(uint64_t prefixId)
  : prefixId(prefixId)
  {
  }

  virtual ~PrefixOp() = default;

  virtual void store(std::unique_ptr<Value> &value, std::stack<std::unique_ptr<PrefixOp>> &prefixOpStack) override;
  virtual std::unique_ptr<Prefix> apply(Value const &value);
};

/**
 * Can only appear in suffix context
 */
class Suffix : public Variable
{
public:
  uint64_t suffixId;

  virtual ~Suffix() = default;
  virtual std::unique_ptr<Prefix> apply(Value const &value);
};

class IntValue : public Value
{
public:
  int value;

  IntValue(int value)
    : Value(0u)
    , value(value)
  {}
};

bool operator>(Prefix const &prefix, Suffix const &suffix);
bool operator<(Prefix const &prefix, Suffix const &suffix);

constexpr bool operator==(Prefix const &, Suffix const &)
{
  return false;
}

constexpr bool operator!=(Prefix const &, Suffix const &)
{
  return true;
}
