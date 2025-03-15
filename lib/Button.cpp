#include "../include/Button.h"

namespace daltonyx
{

Button::Button()
{
}

Button::Button(std::string textToDisplay, sf::Vector2f position, sf::Font &font, bool justifyRight)
{
    this->justifyRight = justifyRight;

    if (justifyRight)
        position += sf::Vector2f(300, 0);
    this->stringText = textToDisplay;
    ticker = 1;
    rect = sf::RectangleShape();
    text = sf::Text();
    this->font = font;
    text.setFont(font);
    text.setString(stringText);
    text.setCharacterSize(15);
    text.setFillColor(sf::Color::White);
    text.setStyle(sf::Text::Bold);

    text.setOrigin(text.getLocalBounds().left + text.getLocalBounds().width / 2.0f, text.getLocalBounds().top + text.getLocalBounds().height / 2.0f);

    text.setPosition(position);

    rect.setSize(sf::Vector2f(std::max((int)text.getLocalBounds().width + 10, 200), 30));
    rect.setOrigin(rect.getLocalBounds().left + rect.getLocalBounds().width / 2.0f, rect.getLocalBounds().top + rect.getLocalBounds().height / 2.0f);
    rect.setPosition(position);

    rect.setFillColor(colorDefault);
    rect.setOutlineThickness(2);
    rect.setOutlineColor(outlineColor);

    state = DEFAULT;
    shape = sf::RectangleShape(sf::Vector2f(150, 150));
    shape.setOrigin(75, 75);
    shape.setPosition(position.x + 170, position.y);
    shape.setOutlineColor(sf::Color::White);
    shape.setOutlineThickness(2);
}

void Button::SetOutlineColor(sf::Color color)
{
    outlineColor = color;
    rect.setOutlineColor(color);
}

void Button::SetWordColor(sf::Color color)
{
    text.setFillColor(color);
}

void Button::SetWordOutlineColor(sf::Color color)
{
    text.setOutlineColor(color);
    text.setOutlineThickness(1);
}

bool Button::IsClicked(sf::RenderWindow &window)
{
    bool clicked = (state == PRESSED && ticker == 0);
    if (clicked)
    {
        state = HOVER;
        return true;
    }
    return false;
}

void Button::OverrideColor(sf::Color color)
{
    colorDefault = color;
    colorHover = sf::Color(color.r - 20, color.g - 20, color.b - 20);
    colorPressed = sf::Color(color.r - 40, color.g - 40, color.b - 40);
}

void Button::SetOutlineThickness(int size)
{
    rect.setOutlineThickness(size);
}

void Button::Update(sf::Vector2i &mousePosition)
{
    if (ticker > 0)
        ticker--;
    this->text.setString(stringText); // Update text origin and position to center it
    text.setOrigin(text.getLocalBounds().left + text.getLocalBounds().width / 2.0f, text.getLocalBounds().top + text.getLocalBounds().height / 2.0f);
    if (justifyRight)
    {
        text.setOrigin(text.getGlobalBounds().width, text.getLocalBounds().top + text.getLocalBounds().height / 2.0f);
    }
    text.setPosition(rect.getPosition().x, rect.getPosition().y);

    rect.setFillColor(invisibleBackground ? sf::Color::Transparent : (state == HOVER ? colorHover : (state == PRESSED ? colorPressed : colorDefault)));

    if (rect.getGlobalBounds().contains(mousePosition.x, mousePosition.y))
    {
        state = sf::Mouse::isButtonPressed(sf::Mouse::Left) ? PRESSED : HOVER;
    }
    else
    {
        state = DEFAULT;
    }
}

void Button::SetPosition(sf::Vector2f position)
{

    rect.setPosition(position);
    shape.setPosition(position.x + 170, position.y);
    if (justifyRight)
        position += sf::Vector2f(180, 0);
    text.setPosition(position);
}

void Button::SetIcon(sf::Texture &texture)
{
    icon = &texture;
    shape.setTexture(icon);
    hasIcon = true;
}

void Button::Draw(sf::RenderWindow &window)
{
    window.draw(rect);
    window.draw(text);
    if (hasIcon)
        window.draw(shape);
}

void Button::SetSize(sf::Vector2f size)
{
    rect.setSize(size);
    rect.setOrigin(rect.getLocalBounds().left + rect.getLocalBounds().width / 2.0f, rect.getLocalBounds().top + rect.getLocalBounds().height / 2.0f);
}

void Button::SetCharacterSize(int size)
{
    text.setCharacterSize(size);
    text.setOrigin(text.getLocalBounds().left + text.getLocalBounds().width / 2.0f, text.getLocalBounds().top + text.getLocalBounds().height / 2.0f);
    if (justifyRight)
        text.setOrigin(text.getGlobalBounds().width, text.getLocalBounds().top + text.getLocalBounds().height / 2.0f);
}

sf::Vector2f Button::GetSize()
{
    return rect.getSize();
}

} // namespace daltonyx
