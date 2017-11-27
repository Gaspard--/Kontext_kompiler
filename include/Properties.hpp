#pragma once

#include <unordered_set>
#include <unordered_map>

class PropertyList
{
public:
  using PropertyId = unsigned long int;
  using Properties = std::unordered_set<PropertyId>;

  static constexpr PropertyId inf = static_cast<PropertyId>(-1);

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
  std::unordered_map<unsigned long int, PropertyInfo> propertyInfos;

public:
  PropertyList()
    : count(0), propertyInfos{}
  {
  }

  long unsigned int getCost(Properties &possessed, Properties const &required)
  {
    long unsigned int cost(0u);

    for (auto &property : required)
      {
	if (possessed.find(property) != possessed.end())
	  continue;
	possessed.insert(property);

	PropertyInfo const &info(propertyInfos[property]);
	long unsigned int reqCost(getCost(possessed, info.requirements));

	if (info.cost == inf || reqCost == inf)
	  return inf;
	cost += info.cost + reqCost;
      }
    return cost;
  }

  long unsigned int createProperty(Properties requirements, unsigned long int cost)
  {
    propertyInfos.try_emplace(count, requirements, cost);
    return count++;
  }
};
