#ifndef KEYBINDINGS_H
#define KEYBINDINGS_H
#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include "nlohmann/json.hpp"
#include <fstream>

class KeyBindings
{
  public:
    //should equal -1 if no controller
    int currentDeviceIndex = -1;
    bool isTerminalOpen = false;
    enum class Action
    {
        MoveUp,
        MoveDown,
        MoveLeft,
        MoveRight,
        Interact,
        Interact2,
        OpenWindowCrafting,
        OpenTerminal,
        ScrollUp,
        ScrollDown,
        MouseLeftClick,
        MouseRightClick,
        MouseXPositive,
        MouseXNegative,
        MouseYPositive,
        MouseYNegative,
        Run,
        OpenWindowMap,
        OpenWindowInventory,
        None,
    };

    struct Binding
    {
        enum class InputType
        {
            Keyboard,
            MouseButton,
            JoystickButton,
            JoystickAxisPositive,
            JoystickAxisNegative,
            MouseXPositive,
            MouseXNegative,
            MouseYPositive,
            MouseYNegative
        };

        InputType type;

        union {
            sf::Keyboard::Key key;
            unsigned int joystickButton;
            sf::Joystick::Axis joystickAxis;
            sf::Vector2i mouseMove;
        };

        Binding(sf::Keyboard::Key k) : type(InputType::Keyboard), key(k)
        {
        }
        Binding(unsigned int button) : type(InputType::JoystickButton), joystickButton(button)
        {
        }
        Binding(sf::Joystick::Axis axis, bool isPositive)
        {
            if (isPositive)
                type = InputType::JoystickAxisPositive;
            else
                type = InputType::JoystickAxisNegative;

            joystickAxis = axis;
        }
        Binding(InputType type, sf::Joystick::Axis axis, bool isPositive)
        {
            this->type = type;
            if (isPositive)
                type = InputType::JoystickAxisPositive;
            else
                type = InputType::JoystickAxisNegative;

            joystickAxis = axis;
        }
    };

    KeyBindings();
    std::string getKeyBindingActionString(Action action);
    std::string getKeyBindingKeyString(KeyBindings::Action action);
    std::string getKeyName(KeyBindings::Action action, int currentDeviceIndex);
    void LoadProfiles();
    void loadProfileFromFile(const std::string &filename);
    void saveProfileToFile(const std::string &filename);
    void processJoystickInput(Action action, unsigned int axis, Binding::InputType inputType);
    std::string getKeysName(sf::Keyboard::Key key);
    void addBinding(Action action, sf::Keyboard::Key key);
    void addBinding(Action action, sf::Mouse::Button mouseButton);
    void replaceBindings(Action action, sf::Keyboard::Key key);
    bool isActionPressed(Action action);
    std::string getActionName(Action action);
    void addMouseMoveBinding(Action action);
    void updateCursorPosition(float x, float y);

    void drawCursor(sf::RenderWindow &window);
    void replaceBindings(Action action, unsigned int joystickButton);

    void replaceBindings(Action action, Binding::InputType inputType, unsigned int axis);

    void addBinding(Action action, unsigned int joystickButton);

    void addBinding(Action action, int axis, bool isPositive);

    Action mouseMoveAction;
    int deadzone = 35;
    sf::RectangleShape cursor;
    sf::Texture cursorTexture;

  private:
    // Map of actions to their bindings (each action can have multiple bindings)
    std::unordered_map<Action, std::vector<Binding>> keyBindings;
};

#endif // KEYBINDINGS_H
