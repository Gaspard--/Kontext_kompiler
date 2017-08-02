#include "Stack.hpp"

void Stack::unwind(size_t id)
{
  for (auto it = destructors.rbegin(); it < destructors.rend() - id; ++it)
    (*it)(*this);
}

void Stack::unwind(Stack::Handle const &handle)
{
  unwind(handle.id);
}

Stack::~Stack()
{
  unwind(0u);
}
