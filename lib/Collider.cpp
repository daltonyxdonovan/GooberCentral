#include "../include/Collider.h"
#include "../include/Globals.h"
#include <cmath>
#include <iostream>

Collider::Collider(sf::Vector2f position, sf::Vector2f size) : position(position), size(size)
{
    outline.setSize(size);
    outline.setPosition(position - size / 2.f);
    outline.setFillColor(sf::Color::Transparent);
    outline.setOutlineColor(sf::Color::Red);
    outline.setOutlineThickness(2);

    top.setSize(sf::Vector2f(size.x, 1));
    bottom.setSize(sf::Vector2f(size.x, 1));
    left.setSize(sf::Vector2f(1, size.y));
    right.setSize(sf::Vector2f(1, size.y));

    updateBoundaries();
}

void Collider::updateBoundaries()
{
    top.setPosition(position.x - size.x / 2, position.y - size.y / 2 - 0.5f);
    bottom.setPosition(position.x - size.x / 2, position.y + size.y / 2 - 0.5f);
    left.setPosition(position.x - size.x / 2 - 0.5f, position.y - size.y / 2);
    right.setPosition(position.x + size.x / 2 - 0.5f, position.y - size.y / 2);
}

bool Collider::CheckCollision(Collider& other, sf::Vector2f& direction, float push)
{
    sf::Vector2f otherPosition = other.GetPosition();
    sf::Vector2f otherHalfSize = other.GetHalfSize();
    sf::Vector2f thisHalfSize = this->GetHalfSize();

    bool collisionX = position.x + thisHalfSize.x > otherPosition.x - otherHalfSize.x && position.x - thisHalfSize.x < otherPosition.x + otherHalfSize.x;
    bool collisionY = position.y + thisHalfSize.y > otherPosition.y - otherHalfSize.y && position.y - thisHalfSize.y < otherPosition.y + otherHalfSize.y;

    if (collisionX && collisionY)
    {
        float overlapX = thisHalfSize.x + otherHalfSize.x - std::abs(position.x - otherPosition.x);
        float overlapY = thisHalfSize.y + otherHalfSize.y - std::abs(position.y - otherPosition.y);

        if (overlapX < overlapY)
        {
            if (position.x < otherPosition.x)
            {
                Move(-overlapX - push, 0); // Move left
            }
            else
            {
                Move(overlapX + push, 0); // Move right
            }
        }
        else if (direction.y >= 0)
        {
            if (position.y < otherPosition.y)
            {
                Move(0, -overlapY - push); // Move up
            }
            else
            {
                Move(0, overlapY + push); // Move down
            }
        }

        return true;
    }

    return false;
}

sf::Vector2f Collider::GetPosition() const
{
    return position;
}

void Collider::SetPosition(sf::Vector2f& position)
{
    this->position = position;
    outline.setPosition(position - size / 2.f);
    updateBoundaries();
}

void Collider::Draw(sf::RenderWindow& window)
{
    window.draw(outline);
    if (isDebug)
    {
        window.draw(top);
        window.draw(bottom);
        window.draw(left);
        window.draw(right);
    }
}

sf::Vector2f Collider::GetHalfSize() const
{
    return size / 2.f;
}

void Collider::Move(float dx, float dy)
{
    position.x += dx;
    position.y += dy;
    outline.setPosition(position - size / 2.f);
    updateBoundaries();
}

void Collider::setOrigin(const sf::Vector2f& origin)
{
    this->origin = origin;
    outline.setOrigin(origin);
}

sf::Vector2f Collider::getOrigin() const
{
    return origin;
}

bool& Collider::GetIsMovable()
{
    return isMovable;
}

void Collider::SetIsMovable(bool val)
{
    isMovable = val;
}

bool Collider::CheckPointCollision(const sf::Vector2f& point) const
{
    sf::Vector2f halfSize = GetHalfSize();

    float left = position.x - halfSize.x;
    float right = position.x + halfSize.x;
    float top = position.y - halfSize.y;
    float bottom = position.y + halfSize.y;

    return (point.x >= left && point.x <= right && point.y >= top && point.y <= bottom);
}
