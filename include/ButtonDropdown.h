#ifndef BUTTON_DROPDOWN_H
#define BUTTON_DROPDOWN_H

#include "Button.h"
#include "SoundManager.h"
#include "Globals.h"
#include <SFML/Graphics.hpp>
#include <functional>
#include <string>
#include <vector>

namespace daltonyx
{
class ButtonDropdown
{
  public:
    int selectedOption = 0;

    ButtonDropdown(std::string &initialOption, sf::Vector2f position, SoundManager &soundManager, sf::Font &font);

    void AddOptions(const std::vector<std::string> &optionList);
    void SetConfirmCallback(const std::function<void(const std::string &)> &callback);
    void SetCallbacks(const std::vector<std::function<void()>> callback);
    void Update(sf::Vector2i &mousePosition, sf::Vector2f positionToBe, sf::RenderWindow &window, sf::Time &time, KeyBindings &keybinds);
    void SetSelectedOption(int index);
    void Draw(sf::RenderWindow &window);
    std::string GetSelectedOption() const;

  private:
    daltonyx::Button minus;
    daltonyx::Button option;
    daltonyx::Button plus;
    daltonyx::Button confirm;
    std::string minusText = "<";
    std::string plusText = ">";
    std::string confirmText = "CONFIRM";
    SoundManager *soundManager;
    int cooldown = 0;
    std::vector<std::string> options;
    std::function<void(const std::string &)> confirmCallback;
    std::vector<std::function<void()>> callbacks;
    sf::Font font;
    float dt = 0;
};
} // namespace daltonyx

#endif // BUTTON_DROPDOWN_H
