#pragma once

#include <unordered_set>
#include <unordered_map>

class PropertyList
{
public:
  using Properties = std::unordered_set<unsigned long int>;

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

	cost += info.cost + getCost(possessed, info.requirements);
      }
    return cost;
  }

  long unsigned int createProperty(Properties requirements, unsigned long int cost)
  {
    propertyInfos.try_emplace(count, requirements, cost);
    return count++;
  }
};
