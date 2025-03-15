#pragma once
#include "Button.h"
#include "Globals.h"
#include <SFML/Graphics.hpp>
#include <string>

class DraggableWindow
{
  public:
    KeyBindings *keybinds;
    DraggableWindow(std::string title, sf::Vector2f size, sf::Font &font, KeyBindings &keybinds);

    virtual void HandleEvents(float delta, sf::Event &event, KeyBindings &keybinds);
    virtual void Update(sf::RenderWindow &window, sf::Time &elapsedTime, sf::Vector2u &windowSize, sf::Vector2i &windowPosition, sf::Vector2i &mousePosition);
    void UpdateBase(sf::RenderWindow &window, sf::Time &elapsedTime, sf::Vector2u &windowSize, sf::Vector2i &windowPosition, sf::Vector2i &mousePosition);
    void UpdateSize(sf::Vector2f size);
    virtual void Draw(sf::RenderWindow &window);
    void DrawBase(sf::RenderWindow &window);
    bool isHidden = true;
    bool isAllowedToDrag = true;
    bool isHiddenBuffer = false;
  protected:
    struct Tooltip
    {
        std::string name = "name";
        sf::Vector2f position;
        sf::Vector2f size;
        sf::RectangleShape background;
        sf::Text description;
        sf::Text textName;
        sf::RectangleShape divider;

        Tooltip() = default;

        Tooltip(sf::Font &font)
        {
            position = sf::Vector2f(0, 0);
            size = sf::Vector2f(150, 50);
            background = sf::RectangleShape(size);

            background.setFillColor(sf::Color(25, 25, 25, 255));
            background.setOutlineColor(sf::Color::White);
            background.setOutlineThickness(5);

            description.setFont(font);
            description.setCharacterSize(14);
            description.setFillColor(sf::Color::White);

            textName.setFont(font);
            textName.setCharacterSize(20);
            textName.setFillColor(sf::Color::White);

            divider.setFillColor(sf::Color(150, 150, 150));
            divider.setSize(sf::Vector2f(0, 2));
        }

        void SetInfo(std::string name, sf::Vector2f position)
        {
            this->position = position;
            this->name = name;

            textName.setString(name);
            textName.setPosition(position + sf::Vector2f(50, -10));

            std::string tooltipText;

            description.setString(tooltipText);
            description.setPosition(position + sf::Vector2f(50, 30)); // Below the name and divider

            sf::FloatRect nameBounds = textName.getLocalBounds();
            divider.setSize(sf::Vector2f(std::max(nameBounds.width, description.getLocalBounds().width) + 10, 2));
            divider.setPosition(position + sf::Vector2f(50, nameBounds.height + 10));

            sf::FloatRect textBounds = description.getLocalBounds();
            size = sf::Vector2f(std::max(nameBounds.width, textBounds.width) + 20, nameBounds.height + textBounds.height + 40);
            background.setSize(size);
            background.setPosition(position + sf::Vector2f(45, -15));
        }

        void Clear()
        {
            this->position = position;

            textName.setString("");
            description.setString("");

            sf::FloatRect textBounds = description.getLocalBounds();
            size = sf::Vector2f(0, 0);
            background.setSize(size);
            background.setPosition(position);
            divider.setSize(sf::Vector2f(0, 0));
        }

        void Draw(sf::RenderWindow &window)
        {
            window.draw(background);
            window.draw(textName);
            window.draw(divider);
            window.draw(description);
        }
    };

    sf::Keyboard::Key key = sf::Keyboard::Tab;
    sf::RectangleShape titleBacker;
    sf::RectangleShape titleFooter;
    bool isDragging = false;
    bool isHandleClicked = false;
    bool needsToClearTooltip = false;
    bool leftMouseReleased = false;
    bool tabReleased = false;

    float scrollOffset = 0.0f;
    float dt = 0;
    float ticker = 0;
    const float scrollSpeed = 20.0f;

    std::shared_ptr<daltonyx::Button> exitButton;
    Tooltip tooltip;
    sf::Vector2f size{ 0, 0 };
    sf::Font font;
    sf::Text textTitle;
    sf::RectangleShape handle;
    sf::Vector2f mouseOffsetPosition = sf::Vector2f(0, 0);
    //mainOffset is the 0,0 of the _draggableWindow_ in local, but expressed in global
    sf::Vector2f mainOffset = sf::Vector2f(300, 50);
    sf::RectangleShape outerWindowBackground;
    sf::RectangleShape innerWindowBackground;
};
