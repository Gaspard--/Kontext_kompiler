#pragma once

#include <memory>
#include <vector>

class Stack
{
private:
  std::vector<char> data;
  std::vector<std::function<void(Stack &)>> destructors;

  void unwind(size_t id);
public:
  Stack() = default;
  ~Stack();

  class Handle
  {
    friend class Stack;
  protected:
    size_t offset;
    size_t id;
  public:
    Handle(int blob);
    
    constexpr Handle(size_t offset, size_t id)
    : offset(offset), id(id)
    {}
  };

  template<class T>
  class TypedHandle : Handle
  {
  public:
    using Handle::Handle;

    T &operator[](Stack &stack)
    {
      return stack[*this];
    }

    Handle &operator++()
    {
      offset += sizeof(T);
      id += 1;
      return *this;
    }

    Handle operator++(int)
    {
      return Handle(offset + sizeof(T), id + 1);
    }
  };

  template<class Cons, class Dest>
  Handle	push(Cons &&cons, Dest &&dest)
  {
    size_t	offset(data.size());

    cons();
    destructors.emplace_back([&dest](Stack &)
			     {
			       dest();
			     });
    return Handle(offset, destructors.size() - 1);
  }

  template<class T, class... Args>
  TypedHandle<T>	push(Args &&... args)
  {
    size_t		offset(data.size());

    data.resize(offset + sizeof(T));
    new (&data[offset])T(std::forward<Args>(args)...);
    destructors.emplace_back([](Stack &stack)
			     {
			       reinterpret_cast<T *>(&stack.data[stack.data.size() - sizeof(T)])->~T();
			       stack.data.resize(stack.data.size() - sizeof(T));
			     });
    return TypedHandle<T>(offset, destructors.size() - 1);
  }

  template<class T>
  T &operator[](TypedHandle<T> const &handle)
  {
    return *reinterpret_cast<T *>(&data[handle->offset]);
  }

  void unwind(Handle const &handle);
};
