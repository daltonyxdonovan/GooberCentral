#pragma once
#include "Object.h"

class ObjectImmovable : public daltonyx::Object
{
  public:
    ObjectImmovable(sf::Vector2f position, sf::Vector2f size);
};
