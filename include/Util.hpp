#pragma once

template<class Func>
class Proxy
{
  Func func;

  Proxy(Func func)
    : func(func)
  {}

  template<class... PARAMS>
  constexpr auto operator[](PARAMS &&... params)
  {
    return func(std::forward<PARAMS>(params)...);
  }
};

