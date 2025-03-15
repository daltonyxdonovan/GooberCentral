#include "../include/KeyBindings.h"

KeyBindings::KeyBindings()
{
    addBinding(Action::MoveUp, sf::Keyboard::W);
    addBinding(Action::MoveDown, sf::Keyboard::S);
    addBinding(Action::MoveLeft, sf::Keyboard::A);
    addBinding(Action::MoveRight, sf::Keyboard::D);
    addBinding(Action::Interact, sf::Keyboard::E);
    addBinding(Action::Interact2, sf::Keyboard::F);
    addBinding(Action::OpenWindowCrafting, sf::Keyboard::Tab);
    addBinding(Action::OpenTerminal, sf::Keyboard::F1);
    addBinding(Action::Run, sf::Keyboard::LShift);
    addBinding(Action::OpenWindowInventory, sf::Keyboard::I);
    addBinding(Action::OpenWindowMap, sf::Keyboard::M);

    cursorTexture.loadFromFile("RES/textures/cursor.png");
    cursor = sf::RectangleShape(sf::Vector2f(30, 30));
    cursor.setTexture(&cursorTexture);
}

std::string KeyBindings::getKeyBindingActionString(KeyBindings::Action action)
{
    return getActionName(action);
}

void KeyBindings::saveProfileToFile(const std::string &filename)
{
    nlohmann::json j;

    for (const auto &[action, bindings] : keyBindings)
    {
        std::vector<nlohmann::json> bindingStrings;

        for (const auto &binding : bindings)
        {
            nlohmann::json bindingJson;
            bindingJson["type"] = static_cast<int>(binding.type);

            switch (binding.type)
            {
            case Binding::InputType::Keyboard:
                bindingJson["value"] = static_cast<int>(binding.key);
                break;
            case Binding::InputType::MouseButton:
                bindingJson["value"] = static_cast<int>(binding.joystickButton); // Mouse buttons stored as buttons
                break;
            case Binding::InputType::JoystickButton:
                bindingJson["value"] = static_cast<int>(binding.joystickButton);
                break;
            case Binding::InputType::JoystickAxisPositive:
            case Binding::InputType::JoystickAxisNegative:
                bindingJson["value"] = static_cast<int>(binding.joystickAxis);
                break;
            default:
                break;
            }

            bindingStrings.push_back(bindingJson);
        }

        j[std::to_string(static_cast<int>(action))] = bindingStrings;
    }

    std::ofstream outFile(filename);
    outFile << j.dump(4); // Pretty-printing
}

void KeyBindings::LoadProfiles()
{
    if (sf::Joystick::isConnected(0)) // Check if at least one joystick is connected
    {
        std::string joystickName = sf::Joystick::getIdentification(0).name;

        // Convert name to lowercase for easier comparison
        std::transform(joystickName.begin(), joystickName.end(), joystickName.begin(), ::tolower);

        if (joystickName.find("xbox") != std::string::npos)
        {
            loadProfileFromFile("RES/keymaps/XBOX.json");
        }
        else if (joystickName.find("playstation") != std::string::npos || joystickName.find("dualshock") != std::string::npos)
        {
            loadProfileFromFile("RES/keymaps/PS.json");
        }
        else if (joystickName.find("nintendo") != std::string::npos || joystickName.find("switch") != std::string::npos)
        {
            loadProfileFromFile("RES/keymaps/SWITCH.json");
        }
        else
        {
            loadProfileFromFile("RES/keymaps/DEFAULT.json"); // Fallback profile
        }
    }
}

void KeyBindings::loadProfileFromFile(const std::string &filename)
{

    std::ifstream inFile(filename);
    if (!inFile.is_open())
    {
        std::cerr << "Failed to open the file: " << filename << std::endl;
        return;
    }

    nlohmann::json j;
    inFile >> j;

    //keyBindings.clear(); // Clear previous bindings to avoid conflicts

    for (auto &[actionStr, bindings] : j.items())
    {
        Action action;
        try
        {
            action = static_cast<Action>(std::stoi(actionStr));
        }
        catch (const std::exception &e)
        {
            std::cerr << "Invalid action ID in JSON: " << actionStr << " (" << e.what() << ")\n";
            continue;
        }

        for (auto &bindingJson : bindings)
        {
            if (!bindingJson.contains("type") || !bindingJson.contains("value"))
            {
                std::cerr << "Skipping invalid binding entry (missing 'type' or 'value')\n";
                continue;
            }

            int typeInt, valueInt;
            try
            {
                typeInt = bindingJson["type"].get<int>();
                valueInt = bindingJson["value"].get<int>();
            }
            catch (const std::exception &e)
            {
                std::cerr << "Invalid 'type' or 'value' in JSON (" << e.what() << ")\n";
                continue;
            }

            Binding::InputType inputType = static_cast<Binding::InputType>(typeInt);

            switch (inputType)
            {
            case Binding::InputType::Keyboard:
                addBinding(action, static_cast<sf::Keyboard::Key>(valueInt));
                break;
            case Binding::InputType::MouseButton:
                addBinding(action, static_cast<sf::Mouse::Button>(valueInt));
                break;
            case Binding::InputType::JoystickButton:
                addBinding(action, static_cast<unsigned int>(valueInt));
                break;
            case Binding::InputType::JoystickAxisPositive:
                keyBindings[action].emplace_back(Binding(static_cast<sf::Joystick::Axis>(valueInt), true));
                break;
            case Binding::InputType::JoystickAxisNegative:
                keyBindings[action].emplace_back(Binding(static_cast<sf::Joystick::Axis>(valueInt), false));
                break;
            default:
                std::cerr << "Unknown input type: " << typeInt << std::endl;
                break;
            }
        }
    }

    std::cout << "Profile [" + filename + "] loaded successfully!" << std::endl;
}

void KeyBindings::addMouseMoveBinding(Action action)
{
    mouseMoveAction = action;
}

void KeyBindings::updateCursorPosition(float x, float y)
{
    cursor.setPosition(x, y);
}

void KeyBindings::drawCursor(sf::RenderWindow &window)
{
    window.draw(cursor);
}

std::string KeyBindings::getKeyBindingKeyString(KeyBindings::Action action)
{
    if (currentDeviceIndex == -1)
    {
        if (action == KeyBindings::Action::MouseLeftClick)
        {
            return "LMB";
        }
        else if (action == KeyBindings::Action::MouseRightClick)
        {
            return "RMB";
        }
        else if (action == KeyBindings::Action::ScrollUp)
        {
            return "Scroll+";
        }
        else if (action == KeyBindings::Action::ScrollDown)
        {
            return "Scroll-";
        }
    }
    return getKeyName(action, currentDeviceIndex);
}

void KeyBindings::addBinding(Action action, sf::Keyboard::Key key)
{
    keyBindings[action].emplace_back(Binding(key));
}

void KeyBindings::addBinding(Action action, unsigned int joystickButton)
{
    keyBindings[action].emplace_back(Binding(joystickButton));
}

void KeyBindings::addBinding(Action action, int axis, bool isPositive)
{
    if (isPositive)
    {
        keyBindings[action].emplace_back(axis);
    }
    else
    {
        keyBindings[action].emplace_back(axis);
    }
}

void KeyBindings::addBinding(Action action, sf::Mouse::Button mouseButton)
{
    keyBindings[action].emplace_back(Binding(mouseButton));
}

void KeyBindings::replaceBindings(Action action, sf::Keyboard::Key key)
{
    keyBindings[action].clear();
    addBinding(action, key);
}

void KeyBindings::replaceBindings(Action action, unsigned int joystickButton)
{
    keyBindings[action].clear();
    addBinding(action, joystickButton);
}

void KeyBindings::replaceBindings(Action action, Binding::InputType inputType, unsigned int axis)
{
    Binding newBinding(axis);
    newBinding.type = inputType;

    keyBindings[action].clear();
    keyBindings[action].push_back(newBinding);
}

bool KeyBindings::isActionPressed(Action action)
{
    if (isTerminalOpen)
        return false;
    for (const auto &binding : keyBindings[action])
    {
        if (binding.type == Binding::InputType::Keyboard)
        {
            if (sf::Keyboard::isKeyPressed(binding.key))
            {
                return true;
            }
        }
        else if (binding.type == Binding::InputType::JoystickButton)
        {
            if (sf::Joystick::isButtonPressed(0, binding.joystickButton))
            {
                return true;
            }
        }
        else if (binding.type == Binding::InputType::JoystickAxisPositive)
        {
            float axisPosition = sf::Joystick::getAxisPosition(0, binding.joystickAxis);
            if (axisPosition > deadzone)
            {
                return true;
            }
        }
        else if (binding.type == Binding::InputType::JoystickAxisNegative)
        {
            float axisPosition = sf::Joystick::getAxisPosition(0, binding.joystickAxis);
            if (axisPosition < -deadzone)
            {
                return true;
            }
        }
        else if (binding.type == Binding::InputType::MouseXPositive || binding.type == Binding::InputType::MouseXNegative)
        {
            return true;
        }
        else if (binding.type == Binding::InputType::MouseYPositive || binding.type == Binding::InputType::MouseYNegative)
        {
            return true;
        }
    }
    return false;
}

std::string KeyBindings::getKeyName(KeyBindings::Action action, int currentDeviceIndex)
{
    auto it = keyBindings.find(action);
    if (it == keyBindings.end())
    {
        return "Unknown Action";
    }

    std::string bindingNames;

    for (const auto &binding : it->second)
    {
        if (binding.type == Binding::InputType::JoystickAxisPositive)
        {
            if (currentDeviceIndex != -1)
            {
                if (binding.joystickAxis == sf::Joystick::X)
                {
                    bindingNames += "Left (Right)\n";
                }
                else if (binding.joystickAxis == sf::Joystick::Y)
                {
                    bindingNames += "Left (Down)\n";
                }
                else if (binding.joystickAxis == sf::Joystick::U)
                {
                    bindingNames += "Right (Right)\n";
                }
                else if (binding.joystickAxis == sf::Joystick::V)
                {
                    bindingNames += "Right (Down)\n";
                }
                else if (binding.joystickAxis == sf::Joystick::Z)
                {
                    bindingNames += "R2\n";
                }
                else if (binding.joystickAxis == sf::Joystick::R)
                {
                    bindingNames += "L2\n";
                }
                else if (binding.joystickAxis == sf::Joystick::PovX)
                {
                    bindingNames += "D-Pad (Right)\n";
                }
                else if (binding.joystickAxis == sf::Joystick::PovY)
                {
                    bindingNames += "D-Pad (Up)\n";
                }
            }
        }
        else if (binding.type == Binding::InputType::JoystickAxisNegative)
        {
            if (currentDeviceIndex != -1)
            {
                if (binding.joystickAxis == sf::Joystick::X)
                {
                    bindingNames += "Left (Left)\n";
                }
                else if (binding.joystickAxis == sf::Joystick::Y)
                {
                    bindingNames += "Left (Up)\n";
                }
                else if (binding.joystickAxis == sf::Joystick::U)
                {
                    bindingNames += "Right (Left)\n";
                }
                else if (binding.joystickAxis == sf::Joystick::V)
                {
                    bindingNames += "Right (Up)\n";
                }
                else if (binding.joystickAxis == sf::Joystick::Z)
                {
                    bindingNames += "R2\n";
                }
                else if (binding.joystickAxis == sf::Joystick::R)
                {
                    bindingNames += "L2\n";
                }
                else if (binding.joystickAxis == sf::Joystick::PovX)
                {
                    bindingNames += "D-Pad (Left)\n";
                }
                else if (binding.joystickAxis == sf::Joystick::PovY)
                {
                    bindingNames += "D-Pad (Down)\n";
                }
            }
        }
        else if (binding.type == Binding::InputType::Keyboard)
        {
            if (currentDeviceIndex == -1)
                bindingNames += getKeysName(binding.key) /* + "\n"*/;
        }
        else if (binding.type == Binding::InputType::MouseButton)
        {
            if (currentDeviceIndex == -1)
                bindingNames += "Mouse Button " + std::to_string(binding.joystickButton) + "\n";
        }
        else if (binding.type == Binding::InputType::JoystickButton)
        {
            if (currentDeviceIndex != -1)
            {
                switch (binding.joystickButton)
                {
                case 0:
                    bindingNames += "A\n";
                    break;
                case 1:
                    bindingNames += "B\n";
                    break;
                case 2:
                    bindingNames += "X\n";
                    break;
                case 3:
                    bindingNames += "Y\n";
                    break;
                case 4:
                    bindingNames += "L1\n";
                    break;
                case 5:
                    bindingNames += "R1\n";
                    break;
                case 6:
                    bindingNames += "SELECT\n";
                    break;
                case 7:
                    bindingNames += "START\n";
                    break;
                case 8:
                    bindingNames += "L3\n";
                    break;
                case 9:
                    bindingNames += "R3\n";
                    break;
                case 10:
                    bindingNames += "GUIDE\n";
                    break;
                default:
                    bindingNames += "Button " + std::to_string(binding.joystickButton) + "\n";
                    break;
                }
            }
        }
    }

    if (bindingNames.empty())
    {
        return "UNKNOWN";
    }

    return bindingNames;
}

void KeyBindings::processJoystickInput(Action action, unsigned int axis, Binding::InputType inputType)
{

    if (axis == sf::Joystick::X)
    {
        if (inputType == Binding::InputType::JoystickAxisPositive)
        {
            addBinding(action, axis, true);
        }
        else
        {
            addBinding(action, axis, false);
        }
    }
    else if (axis == sf::Joystick::Y)
    {
        if (inputType == Binding::InputType::JoystickAxisPositive)
        {
            addBinding(action, axis, true);
        }
        else
        {
            addBinding(action, axis, false);
        }
    }
}

std::string KeyBindings::getKeysName(sf::Keyboard::Key key)
{
    switch (key)
    {
    case sf::Keyboard::A:
        return "A";
    case sf::Keyboard::B:
        return "B";
    case sf::Keyboard::C:
        return "C";
    case sf::Keyboard::D:
        return "D";
    case sf::Keyboard::E:
        return "E";
    case sf::Keyboard::F:
        return "F";
    case sf::Keyboard::G:
        return "G";
    case sf::Keyboard::H:
        return "H";
    case sf::Keyboard::I:
        return "I";
    case sf::Keyboard::J:
        return "J";
    case sf::Keyboard::K:
        return "K";
    case sf::Keyboard::L:
        return "L";
    case sf::Keyboard::M:
        return "M";
    case sf::Keyboard::N:
        return "N";
    case sf::Keyboard::O:
        return "O";
    case sf::Keyboard::P:
        return "P";
    case sf::Keyboard::Q:
        return "Q";
    case sf::Keyboard::R:
        return "R";
    case sf::Keyboard::S:
        return "S";
    case sf::Keyboard::T:
        return "T";
    case sf::Keyboard::U:
        return "U";
    case sf::Keyboard::V:
        return "V";
    case sf::Keyboard::W:
        return "W";
    case sf::Keyboard::X:
        return "X";
    case sf::Keyboard::Y:
        return "Y";
    case sf::Keyboard::Z:
        return "Z";

    case sf::Keyboard::Num0:
        return "0";
    case sf::Keyboard::Num1:
        return "1";
    case sf::Keyboard::Num2:
        return "2";
    case sf::Keyboard::Num3:
        return "3";
    case sf::Keyboard::Num4:
        return "4";
    case sf::Keyboard::Num5:
        return "5";
    case sf::Keyboard::Num6:
        return "6";
    case sf::Keyboard::Num7:
        return "7";
    case sf::Keyboard::Num8:
        return "8";
    case sf::Keyboard::Num9:
        return "9";

    case sf::Keyboard::Escape:
        return "Escape";
    case sf::Keyboard::LControl:
        return "L-Control";
    case sf::Keyboard::LShift:
        return "L-Shift";
    case sf::Keyboard::LAlt:
        return "L-Alt";
    case sf::Keyboard::RControl:
        return "R-Control";
    case sf::Keyboard::RShift:
        return "R-Shift";
    case sf::Keyboard::RAlt:
        return "R-Alt";

    case sf::Keyboard::Tab:
        return "Tab";
    case sf::Keyboard::Space:
        return "Space";
    case sf::Keyboard::Enter:
        return "Enter";
    case sf::Keyboard::BackSpace:
        return "Backspace";

    case sf::Keyboard::Left:
        return "Left Arrow";
    case sf::Keyboard::Right:
        return "Right Arrow";
    case sf::Keyboard::Up:
        return "Up Arrow";
    case sf::Keyboard::Down:
        return "Down Arrow";

    case sf::Keyboard::PageUp:
        return "Page Up";
    case sf::Keyboard::PageDown:
        return "Page Down";
    case sf::Keyboard::Home:
        return "Home";
    case sf::Keyboard::End:
        return "End";
    case sf::Keyboard::Insert:
        return "Insert";
    case sf::Keyboard::Delete:
        return "Delete";

    case sf::Keyboard::F1:
        return "F1";
    case sf::Keyboard::F2:
        return "F2";
    case sf::Keyboard::F3:
        return "F3";
    case sf::Keyboard::F4:
        return "F4";
    case sf::Keyboard::F5:
        return "F5";
    case sf::Keyboard::F6:
        return "F6";
    case sf::Keyboard::F7:
        return "F7";
    case sf::Keyboard::F8:
        return "F8";
    case sf::Keyboard::F9:
        return "F9";
    case sf::Keyboard::F10:
        return "F10";
    case sf::Keyboard::F11:
        return "F11";
    case sf::Keyboard::F12:
        return "F12";

    case sf::Keyboard::Numpad0:
        return "NumPad 0";
    case sf::Keyboard::Numpad1:
        return "NumPad 1";
    case sf::Keyboard::Numpad2:
        return "NumPad 2";
    case sf::Keyboard::Numpad3:
        return "NumPad 3";
    case sf::Keyboard::Numpad4:
        return "NumPad 4";
    case sf::Keyboard::Numpad5:
        return "NumPad 5";
    case sf::Keyboard::Numpad6:
        return "NumPad 6";
    case sf::Keyboard::Numpad7:
        return "NumPad 7";
    case sf::Keyboard::Numpad8:
        return "NumPad 8";
    case sf::Keyboard::Numpad9:
        return "NumPad 9";
    case sf::Keyboard::Multiply:
        return "NumPad Multiply";
    case sf::Keyboard::Add:
        return "NumPad Add";
    case sf::Keyboard::Subtract:
        return "NumPad Subtract";
    case sf::Keyboard::Divide:
        return "NumPad Divide";

    case sf::Keyboard::SemiColon:
        return "Semicolon (;)";
    case sf::Keyboard::Comma:
        return "Comma (,)";
    case sf::Keyboard::Period:
        return "Period (.)";
    case sf::Keyboard::Slash:
        return "Slash (/)";
    case sf::Keyboard::Backslash:
        return "Backslash (\\)";
    case sf::Keyboard::Tilde:
        return "Tilde (~)";
    case sf::Keyboard::Quote:
        return "Quote (')";

    default:
        return "Unknown Key";
    }
}

std::string KeyBindings::getActionName(Action action)
{
    switch (action)
    {
    case Action::MoveUp:
        return "MoveUp";
    case Action::MoveDown:
        return "MoveDown";
    case Action::MoveLeft:
        return "MoveLeft";
    case Action::MoveRight:
        return "MoveRight";
    case Action::Interact:
        return "Interact";
    case Action::Interact2:
        return "Interact2";
    case Action::OpenWindowCrafting:
        return "OpenWindowCrafting";
    case Action::OpenTerminal:
        return "OpenTerminal";
    case Action::MouseLeftClick:
        return "MouseLeftClick";
    case Action::MouseRightClick:
        return "MouseRightClick";
    case Action::MouseXNegative:
        return "MouseXNegative";
    case Action::MouseXPositive:
        return "MouseXPositive";
    case Action::MouseYNegative:
        return "MouseYNegative";
    case Action::MouseYPositive:
        return "MouseYPositive";
    case Action::ScrollDown:
        return "ScrollDown";
    case Action::ScrollUp:
        return "ScrollUp";
    case Action::Run:
        return "Run";
    case Action::OpenWindowInventory:
        return "OpenWindowInventory";
    case Action::OpenWindowMap:
        return "OpenWindowMap";
    case Action::None:
        return "None";
    default:
        return "UNKNOWN";
    }
}
