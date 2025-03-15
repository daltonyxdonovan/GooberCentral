#include "../include/WindowKeybinds.h"

WindowKeybinds::WindowKeybinds(std::string title, sf::Vector2f size, sf::RenderWindow &window, int &width, int &height, KeyBindings &keybinds, sf::Font &font) : font(font), DraggableWindow(title, size, font, keybinds)
{
    this->size = size;
    this->key = sf::Keyboard::Key::F10;

    // Initialize mapping between button names and actions
    actionMap = { { "MoveUp", KeyBindings::Action::MoveUp }, { "MoveDown", KeyBindings::Action::MoveDown }, { "MoveLeft", KeyBindings::Action::MoveLeft }, { "MoveRight", KeyBindings::Action::MoveRight }, { "Interact", KeyBindings::Action::Interact }, { "Interact2", KeyBindings::Action::Interact2 }, { "OpenWindowCrafting", KeyBindings::Action::OpenWindowCrafting }, { "OpenTerminal", KeyBindings::Action::OpenTerminal },
        { "MouseLeftClick", KeyBindings::Action::MouseLeftClick }, { "MouseRightClick", KeyBindings::Action::MouseRightClick }, { "MouseXNegative", KeyBindings::Action::MouseXNegative }, { "MouseXPositive", KeyBindings::Action::MouseXPositive }, { "MouseYNegative", KeyBindings::Action::MouseYNegative }, { "MouseYPositive", KeyBindings::Action::MouseYPositive }, { "ScrollUp", KeyBindings::Action::ScrollUp },
        { "ScrollDown", KeyBindings::Action::ScrollDown }, { "Run", KeyBindings::Action::Run }, { "OpenWindowInventory", KeyBindings::Action::OpenWindowInventory }, { "OpenWindowMap", KeyBindings::Action::OpenWindowMap } };

    InitButtons(keybinds);

    shapeKeysInfo.setSize(sf::Vector2f(208, 100));
    shapeKeysInfo.setOrigin(104, 100);

    texts = { sf::Text("[Q]: DROP HOVERED ITEM", font, 9), sf::Text("[E]: PICK UP DROPPED ITEM", font, 9), sf::Text("[F]: INTERACT", font, 9), sf::Text("[RMB]: BUILD PLACEABLE / OPEN CHEST", font, 9), sf::Text("[LMB]: SWING ITEM", font, 9) };
    if (currentDeviceIndex == -1)
    {
        for (unsigned int i = 0; i < sf::Joystick::Count; i++)
        {
            if (sf::Joystick::isConnected(i))
            {
                currentDeviceIndex = i;
                return;
            }
        }
    }
    currentDevice.setFont(this->font);
    currentDevice.setFillColor(sf::Color::White);
    currentDevice.setCharacterSize(16);
    currentDevice.setOutlineColor(sf::Color::Red);
    currentDevice.setPosition(sf::Vector2f(500, 500));

    currentDeviceHeader.setFont(this->font);
    currentDeviceHeader.setFillColor(sf::Color::White);
    currentDeviceHeader.setCharacterSize(16);
    currentDeviceHeader.setOutlineColor(sf::Color::Red);
    currentDeviceHeader.setPosition(sf::Vector2f(500, 500));
}

void WindowKeybinds::InitButtons(KeyBindings &keybinds)
{
    sf::Vector2f offset(80, 80);
    for (const auto &[name, action] : actionMap)
    {
        std::string keyBindingString = keybinds.getKeyBindingActionString(action);
        buttons.emplace_back(std::make_unique<daltonyx::Button>(keyBindingString, mainOffset + offset, font, true));

        sf::Text label;
        label.setFont(font);
        label.setOutlineColor(sf::Color::Black);
        label.setFillColor(sf::Color::White);
        label.setOutlineThickness(1);
        label.setCharacterSize(14);
        names.push_back(label);
    }
}

void WindowKeybinds::HandleJoystickInput(KeyBindings &keybinds)
{
    if (!waitingForKeyPress)
        return;

    for (unsigned int joystickId = 0; joystickId < sf::Joystick::Count; ++joystickId)
    {
        if (!sf::Joystick::isConnected(joystickId))
            continue;

        for (unsigned int axis = 0; axis < sf::Joystick::AxisCount; ++axis)
        {
            float axisPosition = sf::Joystick::getAxisPosition(joystickId, static_cast<sf::Joystick::Axis>(axis));

            if (std::abs(axisPosition) > 50)
            {
                keybinds.replaceBindings(currentActionToRebind, (axisPosition > 0) ? KeyBindings::Binding::InputType::JoystickAxisPositive : KeyBindings::Binding::InputType::JoystickAxisNegative, axis);
                waitingForKeyPress = false;
                return;
            }
        }

        for (unsigned int button = 0; button < sf::Joystick::ButtonCount; ++button)
        {
            if (sf::Joystick::isButtonPressed(joystickId, button))
            {
                keybinds.replaceBindings(currentActionToRebind, button);
                waitingForKeyPress = false;
                return;
            }
        }
    }
}

void WindowKeybinds::Update(sf::RenderWindow &window, sf::Time &elapsedTime, sf::Vector2u &windowSize, sf::Vector2i &windowPosition, sf::Vector2i &mousePosition, sf::Vector2f playerPositionInWorld, KeyBindings &keybinds, sf::Event &event)
{
    keybinds.currentDeviceIndex = currentDeviceIndex;
    shapeKeysInfo.setPosition(windowSize.x - 110, windowSize.y - 10);

    DraggableWindow::UpdateBase(window, elapsedTime, windowSize, windowPosition, mousePosition);
    if (isHidden)
        return;

    for (size_t i = 0; i < buttons.size(); i++)
    {
        buttons[i]->Update(mousePosition);

        // Default position
        sf::Vector2f positionOffset = sf::Vector2f(200, 80 + (35 * i));

        // Shift buttons after the first 15
        if (i >= 14)
        {
            positionOffset.x += 400; // Shift right
            positionOffset.y -= 385; // Shift up
        }

        buttons[i]->SetPosition(mainOffset + positionOffset);
        buttons[i]->SetSize(sf::Vector2f(380, 30));

        if (buttons[i]->IsClicked(window) || (buttons[i]->state == HOVER && keybinds.isActionPressed(KeyBindings::Action::MouseLeftClick)))
        {
            WaitForKeyPress(actionMap[buttons[i]->stringText], keybinds);
        }

        buttons[i]->stringText = keybinds.getKeyBindingActionString(actionMap[buttons[i]->stringText]);

        sf::Vector2f namePosition = sf::Vector2f(20, 70 + (35 * i));

        // Apply the same shift to names
        if (i >= 14)
        {
            namePosition.x += 400;
            namePosition.y -= 385;
        }

        names[i].setString(keybinds.getKeyName(actionMap[buttons[i]->stringText], currentDeviceIndex));
        names[i].setPosition(mainOffset + namePosition);
    }


    DetectConnectedJoystick();

    if (deviceCooldown == 0 && sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        currentDeviceIndex++;
        deviceCooldown = 100;
        if (currentDeviceIndex > sf::Joystick::Count)
            currentDeviceIndex = 0;
    }
    if (deviceCooldown == 0 && sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        currentDeviceIndex--;
        deviceCooldown = 100;
        if (currentDeviceIndex < 0)
            currentDeviceIndex = sf::Joystick::Count;
    }

    if (deviceCooldown > 0)
        deviceCooldown--;

    HandleJoystickInput(keybinds);
}

void WindowKeybinds::WaitForKeyPress(KeyBindings::Action action, KeyBindings &keybinds)
{

    currentActionToRebind = action;
    waitingForKeyPress = true;
}

void WindowKeybinds::DrawKeyInfo(sf::RenderWindow &window)
{
    if (!isDrawingKeybindsInfo)
        return;
    window.draw(shapeKeysInfo);
    for (size_t i = 0; i < texts.size(); i++)
    {
        texts[i].setPosition((int)shapeKeysInfo.getPosition().x, (int)(shapeKeysInfo.getPosition().y - (i * 18) - 25));
        texts[i].setOrigin((int)(texts[i].getGlobalBounds().width / 2), 0);
        window.draw(texts[i]);
    }
}

void WindowKeybinds::Draw(sf::RenderWindow &window, const std::string &gameState)
{
    if (gameState == "game")
        DrawKeyInfo(window);

    if (isHidden)
        return;

    DraggableWindow::DrawBase(window);
    for (auto &button : buttons)
        button->Draw(window);
    for (auto &name : names)
        window.draw(name);

    window.draw(currentDevice);
    window.draw(currentDeviceHeader);
    this->exitButton->Draw(window);
    window.draw(textTitle);
}


void WindowKeybinds::DetectConnectedJoystick()
{
    if (currentDeviceIndex == -1)
    {
        for (unsigned int i = 0; i < sf::Joystick::Count; i++)
        {
            if (sf::Joystick::isConnected(i))
            {
                currentDeviceIndex = i;
                return;
            }
        }
    }
    currentDevice.setFont(this->font);
    currentDevice.setFillColor(sf::Color::White);
    currentDevice.setCharacterSize(16);
    currentDevice.setPosition(mainOffset + sf::Vector2f(600, 80 + (35 * 1)));

    currentDeviceHeader.setFont(this->font);
    currentDeviceHeader.setFillColor(sf::Color::White);
    currentDeviceHeader.setCharacterSize(16);
    currentDeviceHeader.setPosition(mainOffset + sf::Vector2f(600, 80 + (35 * 0)));

    if (currentDeviceIndex != -1)
    {
        sf::Joystick::Identification id = sf::Joystick::getIdentification(currentDeviceIndex);
        currentDevice.setString(id.name);
    }
    else
    {
        currentDevice.setString("No Joystick found!");
    }

    currentDeviceHeader.setString("CURRENT CONTROLLER:");

    currentDevice.setOrigin(sf::Vector2f(currentDevice.getGlobalBounds().width / 2, 0));
    currentDeviceHeader.setOrigin(sf::Vector2f(currentDeviceHeader.getGlobalBounds().width / 2, 0));
}
