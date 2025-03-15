#include "../include/World.h"
#include <iostream>


void World::AddObject(std::shared_ptr<daltonyx::Object> object)
{

    spatialMap[object->spatialID].push_back(object);
}

std::vector<std::shared_ptr<daltonyx::Object>> World::GetAllPlatforms() const
{
    std::vector<std::shared_ptr<daltonyx::Object>> platforms;

    for (const auto& [spatialID, objects] : spatialMap)
    {
        for (const auto& object : objects)
        {
            if (auto platform = std::dynamic_pointer_cast<daltonyx::Object>(object))
            {
                platforms.push_back(platform);
            }
        }
    }
    return platforms;
}
