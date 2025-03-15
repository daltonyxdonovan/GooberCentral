#pragma once

#include "ButtonState.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>

namespace daltonyx
{

class Button
{
  public:
    Button();
    Button(std::string textToDisplay, sf::Vector2f position, sf::Font &font, bool justifyRight = false);

    void SetOutlineColor(sf::Color color);
    void SetWordColor(sf::Color color);
    void SetWordOutlineColor(sf::Color color);
    bool IsClicked(sf::RenderWindow &window);
    void OverrideColor(sf::Color color);
    void SetOutlineThickness(int size);
    void Update(sf::Vector2i &mousePosition);
    void SetPosition(sf::Vector2f position);
    void SetIcon(sf::Texture &texture);
    void Draw(sf::RenderWindow &window);
    void SetSize(sf::Vector2f size);
    void SetCharacterSize(int size);
    sf::Vector2f GetSize();

    sf::RectangleShape rect;
    sf::Text text;
    sf::Font font;
    ButtonState state;
    bool justifyRight = false;
    sf::RectangleShape shape;
    std::string stringText = "";

  private:
    int ticker = 1;
    sf::Color colorDefault = sf::Color(25, 25, 25);
    sf::Color colorHover = sf::Color(30, 30, 30);
    sf::Color colorPressed = sf::Color(40, 40, 40);
    sf::Color outlineColor = sf::Color(50, 50, 50);
    bool invisibleBackground = false;
    bool hasIcon = false;
    sf::Texture *icon = nullptr;
    bool hasOutline = true;
    std::shared_ptr<sf::Texture> texture;
};

} // namespace daltonyx
