#pragma once
#include "Collider.h"
#include <SFML/Network.hpp> // Include SFML networking

namespace daltonyx
{
class Object
{
  public:
    sf::Vector2i spatialID = sf::Vector2i(0, 0);
    Collider collider;
    sf::Vector2f size;
    sf::Vector2f velocity;

    Object(sf::Vector2f position, sf::Vector2f size);
    Object() {};
    virtual ~Object() = default;

    virtual void Draw(sf::RenderWindow& window);
    virtual void Update(float dt);
    void UpdateSpatial();
    sf::Vector2f GetPosition() const;
    void SetPosition(sf::Vector2f position);
    Collider& GetCollider();

    // Networking methods
    void SendToServer(sf::TcpSocket& socket);
    void ReceiveFromServer(sf::TcpSocket& socket);
    void Serialize(sf::Packet& packet) const;
    void Deserialize(sf::Packet& packet);

  protected:
    sf::RectangleShape body;
    sf::Vector2f position;
    const float gravity = 15.f;
};
} // namespace daltonyx
