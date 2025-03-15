#include "../include/ObjectImmovable.h"

ObjectImmovable::ObjectImmovable(sf::Vector2f position, sf::Vector2f size) : daltonyx::Object(position, size)
{
    collider.SetIsMovable(false);
}
