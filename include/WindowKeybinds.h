#ifndef WINDOWKEYBINDS_H
#define WINDOWKEYBINDS_H

#include "DraggableWindow.h"
#include "KeyBindings.h"
#include <functional>
#include <unordered_map>
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>
#include <iostream>

class WindowKeybinds : public DraggableWindow
{
  public:
    bool isDrawingKeybindsInfo = true;
    explicit WindowKeybinds(std::string title, sf::Vector2f size, sf::RenderWindow &window, int &width, int &height, KeyBindings &keybinds, sf::Font &font);

    void Update(sf::RenderWindow &window, sf::Time &elapsedTime, sf::Vector2u &windowSize, sf::Vector2i &windowPosition, sf::Vector2i &mousePosition, sf::Vector2f playerPositionInWorld, KeyBindings &keybinds, sf::Event &event);
    void DetectConnectedJoystick();
    void Draw(sf::RenderWindow &window, const std::string &gameState);

  private:
    KeyBindings::Action currentActionToRebind = KeyBindings::Action::None;
    bool waitingForKeyPress = false;
    int cooldown = 0;

    sf::Font font;
    sf::RectangleShape shapeKeysInfo;
    sf::Texture textureKeysInfo;
    std::vector<sf::Text> texts;
    std::vector<std::unique_ptr<daltonyx::Button>> buttons;
    std::vector<sf::Text> names;
    std::unordered_map<std::string, KeyBindings::Action> actionMap;
    sf::Text currentDevice;
    sf::Text currentDeviceHeader;
    int currentDeviceIndex = -1;
    int deviceCooldown = 0;



    void InitButtons(KeyBindings &keybinds);
    void HandleJoystickInput(KeyBindings &keybinds);
    void WaitForKeyPress(KeyBindings::Action action, KeyBindings &keybinds);
    void DrawKeyInfo(sf::RenderWindow &window);
};

#endif // WINDOWKEYBINDS_H
