// #include "Value.hpp"
// #include "Kompiler.hpp"

// void Value::store(std::unique_ptr<Value> &value, std::stack<std::unique_ptr<PrefixOp>> &)
// {
//   value.reset(this);
// }

// void PrefixOp::store(std::unique_ptr<Value> &, std::stack<std::unique_ptr<PrefixOp>> &prefixOpStack)
// {
//   prefixOpStack.emplace(this);
// }

// std::unique_ptr<Prefix> PrefixOp::apply(Value const &value)
// {
//   return {nullptr};
// }

// std::unique_ptr<Prefix> Suffix::apply(Value const &value)
// {
//   return {nullptr};
// }

// /**
//  * TODO: define proper operator precedence
//  */
// bool operator>(Prefix const &prefix, Suffix const &suffix)
// {
//   return true;
// }

// bool operator<(Prefix const &prefix, Suffix const &suffix)
// {
//   return !(prefix > suffix);
// }
