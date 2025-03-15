#include "../include/Object.h"
#include <SFML/Network.hpp>

namespace daltonyx
{
daltonyx::Object::Object(sf::Vector2f position, sf::Vector2f size)
{
    this->body = sf::RectangleShape(size);
    this->collider = Collider(position, size);
    this->size = size;
    this->position = position;

    body.setPosition(position);
    collider.SetPosition(position);

    body.setOrigin(size * .5f);
    collider.setOrigin(size * .5f);

    body.setFillColor(sf::Color::White);

    UpdateSpatial();
}


void daltonyx::Object::Draw(sf::RenderWindow& window)
{
    body.setPosition(position);
    collider.outline.setPosition(position);
    window.draw(body);
}

void Object::Update(float dt)
{
    // Update object logic (e.g., applying velocity, gravity, etc.)
    if (collider.GetIsMovable())
        velocity.y += gravity * dt; // Apply gravity
    position += velocity * dt;      // Update position with velocity
    body.setPosition(position);
    collider.SetPosition(position);
}

void Object::UpdateSpatial()
{
    spatialID = sf::Vector2i(static_cast<int>((position.x + 499) / 500), static_cast<int>((position.y + 499) / 500));
}

sf::Vector2f Object::GetPosition() const
{
    return position;
}

void Object::SetPosition(sf::Vector2f position)
{
    this->position = position;
    body.setPosition(position);
    collider.SetPosition(position);
}

Collider& Object::GetCollider()
{
    return collider;
}

void Object::SendToServer(sf::TcpSocket& socket)
{
    sf::Packet packet;
    Serialize(packet);
    socket.send(packet);
}

void Object::ReceiveFromServer(sf::TcpSocket& socket)
{
    sf::Packet packet;
    if (socket.receive(packet) == sf::Socket::Done)
    {
        Deserialize(packet);
    }
}

void Object::Serialize(sf::Packet& packet) const
{
    packet << spatialID.x << spatialID.y << position.x << position.y << velocity.x << velocity.y << size.x << size.y;
}

void Object::Deserialize(sf::Packet& packet)
{
    packet >> spatialID.x >> spatialID.y >> position.x >> position.y >> velocity.x >> velocity.y >> size.x >> size.y;
}
} // namespace daltonyx
