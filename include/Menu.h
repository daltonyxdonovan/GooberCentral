#ifndef MENU_H
#define MENU_H

#include "Button.h"
#include "Globals.h"
#include "KeyBindings.h"
#include "SoundManager.h"
#include <SFML/Graphics.hpp>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

namespace daltonyx
{

class Menu
{
  public:
    Menu(float width, SoundManager &soundManager, sf::Font &font, float height = 50);

    void AddDropdown(const std::string &title, const std::vector<std::string> &items, const std::vector<std::function<void()>> &callbacks);
    void UpdateGameButtons(sf::Vector2f windowSize, sf::Vector2f mousePosition, sf::RenderWindow &window);
    void Update(const sf::RenderWindow &window, int width, int height, sf::Time &dt, KeyBindings &keybinds);

    void Draw(sf::RenderWindow &window);
    void SetupMenu(daltonyx::Menu &menu, sf::RenderWindow &window, int &width, int &height, bool &isFullscreen, bool &isDebug, unsigned int &fpsWanted, bool &vsyncEnabled, bool &isWindowNeedingClosed, bool &isHidden);
    void HandleEvents(sf::Event &event);

  private:
    struct Dropdown
    {
        sf::Text title;
        std::vector<sf::Text> items;
        std::vector<sf::RectangleShape> itemBackgrounds;
        sf::RectangleShape titleBackground;
        std::vector<std::function<void()>> itemCallbacks;
        bool isOpen = false;
    };
    float dt = 0;
    std::vector<std::unique_ptr<daltonyx::Button>> buttonsGame;
    sf::Color titleBgColor;
    sf::Color itemBgColor;
    SoundManager *soundManager;
    sf::Color outlineColor;
    float outlineThickness = 2.0f;
    float itemPadding = 10.0f;
    float gameButtonsDT = 0;
    int gameButtonsCooldown = 0;
    sf::RectangleShape bar;
    sf::RectangleShape borderWindow;
    sf::Font font;
    sf::Text versionText;
    KeyBindings *keybinds;
    float width;
    float height;
    unsigned int cooldown = 0;
    bool leftMouseReleased = false;

    std::unordered_map<std::string, Dropdown> dropdowns;

    sf::Text CreateText(const std::string &str, float posX, float posY);
};

} // namespace daltonyx

#endif
