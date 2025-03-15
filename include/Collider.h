#pragma once
#include <SFML/Graphics.hpp>

class Collider
{
  public:
    sf::RectangleShape outline;

    Collider(sf::Vector2f position, sf::Vector2f size);
    Collider() {};

    bool CheckCollision(Collider& other, sf::Vector2f& direction, float push);
    sf::Vector2f GetPosition() const;
    void SetPosition(sf::Vector2f& position);
    void Draw(sf::RenderWindow& window);
    sf::Vector2f GetHalfSize() const;
    void Move(float dx, float dy);
    void setOrigin(const sf::Vector2f& origin);
    sf::Vector2f getOrigin() const;
    bool& GetIsMovable();
    void SetIsMovable(bool val);
    bool CheckPointCollision(const sf::Vector2f& point) const;


  private:
    sf::Vector2f position;
    sf::Vector2f size;
    sf::Vector2f origin;
    bool isMovable = true;
    sf::RectangleShape top, bottom, left, right;

    void updateBoundaries();
};
