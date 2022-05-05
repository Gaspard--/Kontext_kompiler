#pragma once

#include <unordered_set>
#include <unordered_map>

#include "Value.hpp"

class PropertyList
{
public:
  using PropertyId = uint32_t;
  using Properties = std::unordered_set<PropertyId>;

  struct Cost
  {
    static constexpr unsigned long int inf = static_cast<unsigned long int>(-1);
    unsigned long int val;

    Cost(unsigned long int val) : val(val) {}

    auto &operator+=(Cost const &other)
    {
      if (val != inf)
	{
	  if (other.val == inf)
	    val = inf;
	  else
	    val += other.val;
	}
      return *this;
    }

    auto operator+(Cost const &other) const
    {
      auto copy(other);

      copy += *this;
      return copy;
    }

    bool isInf() const
    {
      return val == inf;
    }

    operator long unsigned int()
    {
      return val;
    }
  };

  struct PropertyInfo
  {
    Properties requirements;
    Cost cost;

    PropertyInfo(Properties requirements,
		 Cost cost)
      : requirements(requirements),
	cost(cost)
    {
    }

    PropertyInfo() = default;
    PropertyInfo(PropertyInfo const &) = default;
  };

private:
  long unsigned int count;
  std::unordered_map<PropertyId, PropertyInfo> propertyInfos;

  template<class T, class... Types, size_t... Indices>
  static constexpr PropertyId getPrimitivePropertyImpl(std::variant<Types...>, std::index_sequence<Indices...>)
  {
    static_assert((std::is_same_v<T, Types> || ...), "Invalid type passed to getPrimitiveProperty<...>()");
    return ((std::is_same_v<T, Types> * Indices) + ...);
  }

public:
  template<class T>
  static constexpr PropertyId getPrimitiveProperty()
  {
    return getPrimitivePropertyImpl<T>(Primitive{}, std::make_index_sequence<std::variant_size_v<Primitive>>());
  }

  PropertyList()
    : count(), propertyInfos{}
  {
    while (count < std::variant_size_v<Primitive>)
      createProperty({}, PropertyList::Cost::inf);
  }

  Cost getCost(Properties &possessed, Properties const &required)
  {
    Cost cost(0u);

    for (auto &property : required)
      if (possessed.find(property) == possessed.end())
	{
	  possessed.insert(property);
	  PropertyInfo const &info(propertyInfos.at(property));

	  cost += info.cost + getCost(possessed, info.requirements);
	}
    return cost;
  }

  PropertyId createProperty(Properties requirements, unsigned long int cost)
  {
    propertyInfos.try_emplace(count, requirements, cost);
    return count++;
  }
};
