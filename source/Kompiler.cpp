#include <iostream>
#include <charconv>
#include "Kompiler.hpp"
#include "Lexer.hpp"

static DefinedValue idDebugPrefix(Kompiler &, Value &&, DefinedValue &&val)
{
  std::cout << "Applying prefix!\n";
  return std::move(val);
}

static DefinedValue idDebugPostfix(Kompiler &, Value &&, DefinedValue &&val)
{
  std::cout << "Applying prefix!\n";
  return std::move(val);
}

static DefinedValue asInt(Kompiler &, Value &&, DefinedValue &&val)
{
  auto const &token(std::get<std::unique_ptr<Token>>(val.value[0]));
  long unsigned int ret(0u);

  // TODO: wait for <char_conv> or do something serious
  for (auto it(token->content.begin()); it != token->content.end(); ++it)
    {
      ret *= 10;
      ret += *it - '0';
    }

  return makePrimitiveDefinedValue(ret);
}

static DefinedValue asDouble(Kompiler &, Value &&, DefinedValue &&val)
{
  auto const &token(std::get<std::unique_ptr<Token>>(val.value[0]));
  double ret(0u);

  // TODO: wait for <char_conv> or do something serious
  auto it(token->content.begin());
  for (; it != token->content.end(); ++it)
    {
      if (*it == '.')
        {
          double mult = 0.1;
          for (; it != token->content.end(); ++it)
            {
              ret += double(*it - '0') * mult;
              mult *= 0.1;
            }
          break;
        }
      ret *= 10.0;
      ret += double(*it - '0');
    }
  return makePrimitiveDefinedValue(ret);
}

static DefinedValue intOnly(Kompiler &, Value &&, DefinedValue &&val)
{
  std::cout << "Applying INT_ONLY!\n";
  return std::move(val);
}

static DefinedValue typeOnly(Kompiler &, Value &&, DefinedValue &&val)
{
  std::cout << "Applying TYPE_ONLY!\n";
  return std::move(val);
}

void Kompiler::createDefaultDefinitions()
{
  prefixes["ID_DEBUG"].addFunc<&idDebugPrefix>({});
  postfixes["ID_DEBUG"].addFunc<&idDebugPostfix>({});
  prefixes["asInt"].addFunc<&asInt>({PropertyList::getPrimitiveProperty<std::unique_ptr<Token>>()});
  prefixes["asDouble"].addFunc<&asDouble>({PropertyList::getPrimitiveProperty<std::unique_ptr<Token>>()});
  prefixes["INT_ONLY"].addFunc<&intOnly>({PropertyList::getPrimitiveProperty<long unsigned int>()});
  prefixes["TYPE_ONLY"].addFunc<&typeOnly>({PropertyList::getPrimitiveProperty<std::unique_ptr<Type>>()});
  postfixes["+"].addFunc({{PropertyList::getPrimitiveProperty<long unsigned int>()},
			  [](Kompiler &, Value &&, DefinedValue &&val)
			  -> std::variant<DefinedValue, std::pair<Value, UnaryOperator>>
			  {
			   UnaryOperator add;

			   add.addFunc({{PropertyList::getPrimitiveProperty<long unsigned int>()},
					[](Kompiler &, Value &&left, DefinedValue &&right)
					-> std::variant<DefinedValue, std::pair<Value, UnaryOperator>>
					{
					 return {std::move(makePrimitiveDefinedValue(std::get<long unsigned int>(left[0]) + std::get<long unsigned int>(right.value[0])))};
					}});
			   return {std::move(std::pair<Value, UnaryOperator>{std::move(val.value), add})};
			  }});

  postfixes["="].addFunc({{PropertyList::getPrimitiveProperty<std::unique_ptr<Token>>()},
			  [](Kompiler &, Value &&, DefinedValue &&val)
			  -> std::variant<DefinedValue, std::pair<Value, UnaryOperator>>
			  {
			   UnaryOperator set;

			   set.addFunc({{PropertyList::getPrimitiveProperty<long unsigned int>()},
					[](Kompiler &kompiler, Value &&left, DefinedValue &&right)
					-> std::variant<DefinedValue, std::pair<Value, UnaryOperator>>
					{
					 kompiler.values.insert_or_assign(std::get<std::unique_ptr<Token>>(left[0])->content, copy(right));
					 return {std::move(right)};
					}});
			   return {std::move(std::pair<Value, UnaryOperator>{std::move(val.value), set})};
			  }});
  values.insert_or_assign("int", makePrimitiveDefinedValue(std::make_unique<Type>(Type{PropertyList::getPrimitiveProperty<std::remove_reference_t<long unsigned int>>(), {PropertyList::getPrimitiveProperty<std::remove_reference_t<long unsigned int>>()}})));
  values.insert_or_assign("double", makePrimitiveDefinedValue(std::make_unique<Type>(Type{PropertyList::getPrimitiveProperty<std::remove_reference_t<double>>(), {PropertyList::getPrimitiveProperty<std::remove_reference_t<long unsigned int>>()}})));

  postfixes["as"].addFunc({{PropertyList::getPrimitiveProperty<std::unique_ptr<Token>>()},
      [](Kompiler &, Value &&, DefinedValue &&val)
      -> std::variant<DefinedValue, std::pair<Value, UnaryOperator>>
      {
        UnaryOperator as;

        as.addFunc({{PropertyList::getPrimitiveProperty<std::unique_ptr<Type>>()},
            [](Kompiler &kompiler, Value &&left, DefinedValue &&right)
            -> std::variant<DefinedValue, std::pair<Value, UnaryOperator>>
            {
              assert(std::holds_alternative<std::unique_ptr<Type>>(right.value[0]));
              auto const &type = *std::get<std::unique_ptr<Type>>(right.value[0]);
              if (auto *primitiveType = std::get_if<PropertyList::PropertyId>(&type.dataType))
                {
                  if (*primitiveType == PropertyList::getPrimitiveProperty<long unsigned int>())
                    {
                      auto const &token(std::get<std::unique_ptr<Token>>(left[0]));
                      long unsigned int ret(0u);
                      std::from_chars(token->content.data(), token->content.data() + token->content.size(), ret);
                      return makePrimitiveDefinedValue(ret);
                    }
                  if (*primitiveType == PropertyList::getPrimitiveProperty<double>())
                    {
                      auto const &token(std::get<std::unique_ptr<Token>>(left[0]));
                      double ret(0u);
                      std::from_chars(token->content.data(), token->content.data() + token->content.size(), ret);
                      return makePrimitiveDefinedValue(ret);
                    }
                  assert("TODO");
                }
            }});
        return {std::move(std::pair<Value, UnaryOperator>{std::move(val.value), as})};
      }});

}

void Kompiler::process(std::istream &stream)
{
  auto isEndOfLine([](char c)
		   {
		     return c == '\n' || c == ';';
		   });
  /**
   * Note that this iterator is single pass (this explains the code below)
   */
  std::istreambuf_iterator<char> begin(stream);

  std::cout << "> ";
  while (begin != std::istreambuf_iterator<char>{})
    {
      std::string line{};

      for (; !isEndOfLine(*begin); ++begin)
	line += *begin;
      parseLine(line);
      for (; isEndOfLine(*begin); ++begin)
	std::cout << "> ";
    }
}
