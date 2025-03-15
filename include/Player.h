#pragma once
#include "Object.h"
#include "Globals.h"

class Player : public daltonyx::Object
{
  public:
    bool isGrounded = false;
    bool isWallLeft = false;
    bool isWallRight = false;
    bool isHost = false;   // Track if the player is the host
    bool isServer = false; // Track if the player is the server
    std::string uuid;      // Unique identifier for the player
    sf::IpAddress ip;      // Player's IP address (for networking)
    unsigned short port;   // Player's port (for networking)

    void SetUUID(const std::string& uuid);
    void SetIsHost(bool isHost);     // Setter for isHost
    void SetIsServer(bool isServer); // Setter for isServer

    Player() {};
    Player(sf::Vector2f position, sf::Vector2f size, float speed, const std::string& uuid, const sf::IpAddress& ip, unsigned short port, bool isHost = false, bool isServer = false);
    void Update(float dt) override;
    void HandleCollision(const std::shared_ptr<daltonyx::Object>& platform);
    void Serialize(sf::Packet& packet);
    void Deserialize(sf::Packet& packet);

  private:
    float speed;
    float friction;
    float maxSpeed = 10;
    float maxFallSpeed = 1;
    const float jumpForce = -9.8f;
};
