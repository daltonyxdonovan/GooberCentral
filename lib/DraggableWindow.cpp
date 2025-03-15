#include "../include/DraggableWindow.h"
#include <iostream>

DraggableWindow::DraggableWindow(std::string title, sf::Vector2f size, sf::Font &font, KeyBindings &keybinds)
{
    this->keybinds = &keybinds;
    this->font = font;
    this->tooltip = Tooltip(font);
    this->size = size;
    textTitle.setFont(font);
    textTitle.setPosition(sf::Vector2f(10, 10) + mainOffset);
    textTitle.setFillColor(sf::Color(255, 255, 255));
    textTitle.setOutlineColor(sf::Color::Black);
    textTitle.setOutlineThickness(1);
    textTitle.setString(title);

    outerWindowBackground.setSize(sf::Vector2f(size.x, size.y));
    outerWindowBackground.setFillColor(sf::Color(25, 25, 25));
    outerWindowBackground.setOutlineColor(sf::Color(50, 50, 50));
    outerWindowBackground.setOutlineThickness(1);
    outerWindowBackground.setPosition(mainOffset);

    exitButton = std::make_shared<daltonyx::Button>("X", mainOffset + sf::Vector2f(size.x - 70, 10), font);
    exitButton->SetSize(sf::Vector2f(45, 45));

    innerWindowBackground.setSize(sf::Vector2f(outerWindowBackground.getSize().x - 10, outerWindowBackground.getSize().y - 65));
    innerWindowBackground.setFillColor(sf::Color(0, 0, 0));
    innerWindowBackground.setOutlineColor(sf::Color(50, 50, 50));
    innerWindowBackground.setOutlineThickness(1);
    innerWindowBackground.setPosition(mainOffset + sf::Vector2f(5, 60));

    handle.setSize(sf::Vector2f(outerWindowBackground.getSize().x - 60, 50));
    handle.setFillColor(sf::Color::Transparent);
    handle.setPosition(outerWindowBackground.getPosition());

    this->titleBacker.setSize(sf::Vector2f(outerWindowBackground.getSize().x, 60));
    this->titleBacker.setFillColor(sf::Color(25, 25, 25));
    this->titleBacker.setPosition(mainOffset + sf::Vector2f(0, outerWindowBackground.getSize().y - 5));
    this->titleFooter.setSize(sf::Vector2f(outerWindowBackground.getSize().x, 60));
    this->titleFooter.setFillColor(sf::Color(25, 25, 25));
    this->titleFooter.setPosition(mainOffset + sf::Vector2f(0, outerWindowBackground.getSize().y - 5));
}

void DraggableWindow::UpdateSize(sf::Vector2f size)
{
    outerWindowBackground.setSize(sf::Vector2f(size.x, size.y));
    innerWindowBackground.setSize(sf::Vector2f(outerWindowBackground.getSize().x - 10, outerWindowBackground.getSize().y - 65));
    handle.setSize(sf::Vector2f(outerWindowBackground.getSize().x - 60, 50));
}

void DraggableWindow::UpdateBase(sf::RenderWindow &window, sf::Time &elapsedTime, sf::Vector2u &windowSize, sf::Vector2i &windowPosition, sf::Vector2i &mousePosition)
{
    dt += elapsedTime.asSeconds();
    if (dt > .5f)
    {
        if (ticker > 0)
            ticker -= 1;
        dt = 0;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
    {
        isHidden = true;
        dt = 0;
    }
    if (sf::Keyboard::isKeyPressed(key) && ticker == 0)
    {
        isHidden = !isHidden;
        ticker = 1;
        dt = 0;
        mainOffset = sf::Vector2f(0, 0);
    }


    if (isHidden != isHiddenBuffer)
    {
        mainOffset = sf::Vector2f(0, 0);
        isHiddenBuffer = isHidden;
    }

    if (isHidden)
        return;

    sf::Vector2f outerWindowBackgroundPosition = outerWindowBackground.getPosition();
    sf::Vector2f outerWindowBackgroundSize = outerWindowBackground.getSize();
    handle.setPosition(outerWindowBackgroundPosition);
    if (isAllowedToDrag && handle.getGlobalBounds().contains((sf::Vector2f)mousePosition))
    {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) || keybinds->isActionPressed(KeyBindings::Action::MouseLeftClick))
        {
            isDragging = true;
            if (!isHandleClicked)
            {
                mouseOffsetPosition = sf::Vector2f(mousePosition.x - mainOffset.x, mousePosition.y - mainOffset.y);
                isHandleClicked = true;
            }
        }
        else
            isDragging = false;
    }

    if (mainOffset.y < 53)
    {
        isDragging = false;
        mainOffset.y = 53;
    }

    if (mainOffset.y > height - size.y)
    {
        isDragging = false;
        mainOffset.y = height - size.y;
    }

    if (mainOffset.x < 4)
    {
        isDragging = false;
        mainOffset.x = 4;
    }

    if (mainOffset.x > width - size.x)
    {
        isDragging = false;
        mainOffset.x = width - size.x;
    }

    if (isDragging)
        mainOffset = sf::Vector2f(mousePosition) - mouseOffsetPosition;
    else
        isHandleClicked = false;



    outerWindowBackground.setPosition(mainOffset);
    textTitle.setPosition(mainOffset + sf::Vector2f(10, 10));
    innerWindowBackground.setPosition(mainOffset + sf::Vector2f(5, 60));

    this->titleBacker.setPosition(mainOffset);
    this->titleFooter.setPosition(mainOffset + sf::Vector2f(0, outerWindowBackground.getSize().y - 5));

    tabReleased = false;
    leftMouseReleased = false;
    exitButton->Update(mousePosition);
    exitButton->SetPosition(mainOffset + sf::Vector2f(size.x - 30, 29));
    if (exitButton->IsClicked(window) || (exitButton->state == HOVER && keybinds->isActionPressed(KeyBindings::Action::MouseLeftClick)))
        isHidden = true;
}

void DraggableWindow::Update(sf::RenderWindow &window, sf::Time &elapsedTime, sf::Vector2u &windowSize, sf::Vector2i &windowPosition, sf::Vector2i &mousePosition)
{
    dt += elapsedTime.asSeconds();
    if (dt > .5f)
    {
        if (ticker > 0)
            ticker -= 1;
        dt = 0;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
    {
        isHidden = true;
        dt = 0;
    }
    if (sf::Keyboard::isKeyPressed(key) && ticker == 0)
    {
        isHidden = !isHidden;
        ticker = 1;
        dt = 0;
    }

    if (isHidden)
        return;

    sf::Vector2f outerWindowBackgroundPosition = outerWindowBackground.getPosition();
    sf::Vector2f outerWindowBackgroundSize = outerWindowBackground.getSize();
    handle.setPosition(outerWindowBackgroundPosition);
    if (handle.getGlobalBounds().contains((sf::Vector2f)mousePosition))
    {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) || keybinds->isActionPressed(KeyBindings::Action::MouseLeftClick))
        {
            isDragging = true;
            if (!isHandleClicked)
            {
                mouseOffsetPosition = sf::Vector2f(mousePosition.x - mainOffset.x, mousePosition.y - mainOffset.y);
                isHandleClicked = true;
            }
        }
        else
            isDragging = false;
    }

    if (isDragging)
        mainOffset = sf::Vector2f(mousePosition) - mouseOffsetPosition;
    else
        isHandleClicked = false;

    if (mainOffset.y < 53)
        mainOffset.y = mainOffset.y + ((53) - mainOffset.y);

    if (mainOffset.y > ((windowSize.y + windowPosition.y)) - outerWindowBackgroundSize.y)
        mainOffset.y = ((windowSize.y + windowPosition.y)) - outerWindowBackgroundSize.y;

    if (mainOffset.x < 4)
        mainOffset.x = 4;

    if (mainOffset.x > windowSize.x - outerWindowBackgroundSize.x - 4)
        mainOffset.x = windowSize.x - outerWindowBackgroundSize.x - 4;

    outerWindowBackground.setPosition(mainOffset);
    textTitle.setPosition(mainOffset + sf::Vector2f(10, 10));
    innerWindowBackground.setPosition(mainOffset + sf::Vector2f(5, 60));

    tabReleased = false;
    leftMouseReleased = false;
}

void DraggableWindow::HandleEvents(float delta, sf::Event &event, KeyBindings &keybinds)
{

    if (isHidden)
        return;
    if (isPaused)
        return;
    if (scrollOffset < 0)
        scrollOffset = 0;


    if (keybinds.isActionPressed(KeyBindings::Action::ScrollDown))
    {
        if (scrollOffset > 0)
            scrollOffset -= scrollSpeed;
    }
    if (keybinds.isActionPressed(KeyBindings::Action::ScrollUp))
    {
        scrollOffset += scrollSpeed;
    }

    if (event.type == sf::Event::MouseWheelScrolled)
    {
        if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
        {
            if (event.mouseWheelScroll.delta > 0)
            {
                if (scrollOffset > 0)
                    scrollOffset -= scrollSpeed;
            }
            else if (event.mouseWheelScroll.delta < 0)
            {
                scrollOffset += scrollSpeed;
            }
        }
    }
    if (event.type = sf::Event::MouseButtonPressed)
    {
        if (event.mouseButton.button == sf::Mouse::Left)
            leftMouseReleased = true;
    }

    if (event.type == sf::Event::KeyReleased)
    {
        if (event.key.code == sf::Keyboard::Tab)
            tabReleased = true;
    }
}

void DraggableWindow::DrawBase(sf::RenderWindow &window)
{
    if (isHidden)
        return;

    needsToClearTooltip = true;

    window.draw(outerWindowBackground);
    window.draw(textTitle);
    window.draw(innerWindowBackground);

    if (needsToClearTooltip)
        tooltip.Clear();

    this->tooltip.Draw(window);

    this->exitButton->Draw(window);
}

void DraggableWindow::Draw(sf::RenderWindow &window)
{
    if (isHidden)
        return;

    needsToClearTooltip = true;

    window.draw(outerWindowBackground);
    window.draw(textTitle);
    window.draw(innerWindowBackground);

    if (needsToClearTooltip)
        tooltip.Clear();

    this->tooltip.Draw(window);
}
