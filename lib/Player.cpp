#include "../include/Player.h"
#include <SFML/Window/Keyboard.hpp>
#include <iostream>

Player::Player(sf::Vector2f position, sf::Vector2f size, float speed, const std::string& uuid, const sf::IpAddress& ip, unsigned short port, bool isHost, bool isServer) : daltonyx::Object(position, size), speed(speed), isGrounded(false), uuid(uuid), ip(ip), port(port), isHost(isHost), isServer(isServer)
{
    //collider.SetIsMovable(false);
}

void Player::SetUUID(const std::string& uuid)
{
    this->uuid = uuid;
}

void Player::SetIsHost(bool isHost)
{
    this->isHost = isHost;
}

void Player::SetIsServer(bool isServer)
{
    this->isServer = isServer;
}

void Player::Update(float dt)
{


    std::cout << "updating player " << std::endl;
    if (position.y > height)
    {
        position.y = -100;
        velocity.y = 0;
    }
    body.setFillColor(sf::Color::Green);
    // Apply friction to smooth the horizontal movement
    velocity.x *= 0.9f; // Reduced friction for more natural control

    if (isWallLeft)
    {
        velocity.y *= .8f;
        if (velocity.x < 0)
        {
            velocity.x = 0;
        }
    }
    else if (isWallRight)
    {
        velocity.y *= .8f;
        if (velocity.x > 0)
        {
            velocity.x = 0;
        }
    }

    UpdateSpatial();

    // Handling input for movement
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        velocity.x -= speed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        velocity.x += speed;
    }

    // Wall jump logic (wall left or right)
    if (!isGrounded)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        {
            if (isWallLeft) // Wall jump from the left
            {
                velocity.y = jumpForce;  // Half the jump force
                velocity.x = speed * 20; // Push right away from the wall
            }
            else if (isWallRight) // Wall jump from the right
            {
                velocity.y = jumpForce;   // Half the jump force
                velocity.x = -speed * 20; // Push left away from the wall
            }
        }
    }

    if (isGrounded && sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
    {
        velocity.y = jumpForce;
        isGrounded = false;
    }

    // Apply gravity when not grounded
    if (!isGrounded)
        velocity.y += gravity * dt;

    // Update position
    position += velocity;

    // Set the player's position and update the collider
    body.setPosition(position);
    collider.SetPosition(position);
}

void Player::HandleCollision(const std::shared_ptr<daltonyx::Object>& platform)
{
    if (!platform)
        return;

    sf::Vector2f direction = velocity;
    if (!collider.CheckCollision(platform->GetCollider(), direction, 1.0f))
        return;

    sf::Vector2f playerPos = position;
    sf::Vector2f platformPos = platform->GetCollider().GetPosition();
    sf::Vector2f playerHalfSize = collider.GetHalfSize();
    sf::Vector2f platformHalfSize = platform->GetCollider().GetHalfSize();

    float overlapX = (playerHalfSize.x + platformHalfSize.x) - std::abs(playerPos.x - platformPos.x);
    float overlapY = (playerHalfSize.y + platformHalfSize.y) - std::abs(playerPos.y - platformPos.y);

    bool isWallCollision = overlapX < overlapY;
    bool isGroundCollision = !isWallCollision && velocity.y > 0.0f;

    if (isWallCollision)
    {
        // Resolve horizontal overlap without zeroing velocity
        if (direction.x > 0.0f)
            position.x -= overlapX; // Colliding with the right side of the wall
        else if (direction.x < 0.0f)
            position.x += overlapX; // Colliding with the left side of the wall

        // Check if the player is hugging the wall (set the wall flags accordingly)
        if (velocity.x > 0.0f) // Moving towards the right wall
        {
            isWallRight = true;
            isWallLeft = false;
        }
        else if (velocity.x < 0.0f) // Moving towards the left wall
        {
            isWallLeft = true;
            isWallRight = false;
        }
    }
    else if (isGroundCollision)
    {
        // Proper ground detection (on a surface)
        position.y -= overlapY;
        velocity.y = 0.0f;
        isGrounded = true;
    }
    else
    {
        // Handling hitting the ceiling
        position.y += overlapY;
        velocity.y = 0.0f;
    }

    body.setPosition(position);
    collider.SetPosition(position);
}

void Player::Serialize(sf::Packet& packet)
{
    packet << position.x << position.y << velocity.x << velocity.y << isHost << isServer << uuid;
}

void Player::Deserialize(sf::Packet& packet)
{
    packet >> position.x >> position.y >> velocity.x >> velocity.y >> isHost >> isServer >> uuid;
}
