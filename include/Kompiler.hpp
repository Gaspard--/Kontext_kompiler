#pragma once

#include "Stack.hpp"
#include "Lookup.hpp"

class Kompiler
{
private:
  Stack stack;
  Lookup<int> test;
public:
  Kompiler() = default;
  ~Kompiler() = default;

  void process(std::istream &);
};
