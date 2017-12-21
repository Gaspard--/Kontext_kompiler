#pragma once

#include <unordered_set>
#include <unordered_map>

#include "Value.hpp"

class PropertyList
{
public:
  using PropertyId = unsigned long int;
  using Properties = std::unordered_set<PropertyId>;

  static constexpr unsigned long int inf = static_cast<unsigned long int>(-1);

  struct PropertyInfo
  {
    Properties requirements;
    unsigned long int cost;

    PropertyInfo(Properties requirements,
		 unsigned long int cost)
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
      createProperty({}, PropertyList::inf);
  }

  long unsigned int getCost(Properties &possessed, Properties const &required)
  {
    long unsigned int cost(0u);

    for (auto &property : required)
      if (possessed.find(property) == possessed.end())
	{
	  possessed.insert(property);

	  PropertyInfo const &info(propertyInfos[property]);
	  long unsigned int reqCost(getCost(possessed, info.requirements));

	  if (info.cost == inf || reqCost == inf)
	    return inf;
	  cost += info.cost + reqCost;
	}
    return cost;
  }

  PropertyId createProperty(Properties requirements, unsigned long int cost)
  {
    propertyInfos.try_emplace(count, requirements, cost);
    return count++;
  }
};
