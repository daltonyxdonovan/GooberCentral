#ifndef WINDOWSETTINGS_H
#define WINDOWSETTINGS_H

#include "ButtonDropdown.h"
#include "DraggableWindow.h"
#include "Globals.h"
#include "SoundManager.h"
#include <functional>
#include <string>
#include <vector>

class WindowSettings : public DraggableWindow
{
  public:
    WindowSettings(std::string title, sf::Vector2f size, bool &isFullscreen, unsigned int &fpsWanted, sf::RenderWindow &window, bool &vsyncEnabled, int &width, int &height, SoundManager &soundManager, sf::Font &font, KeyBindings &keybinds, float &mouseSpeed);

    void Update(sf::RenderWindow &window, sf::Time &elapsedTime, sf::Vector2u &windowSize, sf::Vector2i &windowPosition, sf::Vector2i &mousePosition);
    void Draw(sf::RenderWindow &window);

  private:
    sf::Font font;
    float dt = 0;
    int cooldown = 0;
    int width = 0;
    int height = 0;
    sf::Vector2f size{ 0, 0 };
    SoundManager *soundManager;

    std::vector<std::unique_ptr<daltonyx::ButtonDropdown>> dropdowns;

    void AddDropdown(const std::string &label, const std::vector<std::string> &options, const std::vector<std::function<void()>> &callbacks);
};

#endif // WINDOWSETTINGS_H
