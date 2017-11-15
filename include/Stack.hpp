#pragma once

#include <memory>
#include <deque>

#include "Value.hpp"

class Stack : public std::deque<std::unique_ptr<Value>>
{
public:
  Stack() = default;
  Stack(Stack const &) = delete;
  Stack(Stack &&) = delete;
  ~Stack() = default;

  /// I didn't find anything about stack resize destrcution order so using pop instead
  void unwind(Value *until)
  {
    while (back().get() != until)
      pop_back();
    pop_back();
  }
};
