#include <unordered_map>
#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>
#include <boost/functional/hash.hpp>
#include "../include/Object.h"

namespace boost
{
template <> struct hash<sf::Vector2i>
{
    std::size_t operator()(const sf::Vector2i& v) const noexcept
    {
        std::size_t seed = 0;
        boost::hash_combine(seed, v.x);
        boost::hash_combine(seed, v.y);
        return seed;
    }
};
} // namespace boost

class World
{
  public:
    std::unordered_map<sf::Vector2i, std::vector<std::shared_ptr<daltonyx::Object>>, boost::hash<sf::Vector2i>> spatialMap;
    void AddObject(std::shared_ptr<daltonyx::Object> object);
    std::vector<std::shared_ptr<daltonyx::Object>> GetAllPlatforms() const;
};
