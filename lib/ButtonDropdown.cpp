#include "../include/ButtonDropdown.h"
#include <iostream>

namespace daltonyx
{

ButtonDropdown::ButtonDropdown(std::string &initialOption, sf::Vector2f position, SoundManager &soundManager, sf::Font &font) : selectedOption(0)

{
    this->font = font;
    minus = daltonyx::Button(minusText, position + sf::Vector2f(50, 0), font);
    option = daltonyx::Button(initialOption, position + sf::Vector2f(100, 0), font);
    plus = daltonyx::Button(plusText, position + sf::Vector2f(350, 0), font);
    confirm = daltonyx::Button(confirmText, position + sf::Vector2f(410, 0), font);

    minus.SetSize(sf::Vector2f(50, 30));
    option.SetSize(sf::Vector2f(200, 30));
    plus.SetSize(sf::Vector2f(50, 30));
    confirm.SetSize(sf::Vector2f(80, 30));
    this->soundManager = &soundManager;
}

void ButtonDropdown::AddOptions(const std::vector<std::string> &optionList)
{
    options = optionList;
}

void ButtonDropdown::SetConfirmCallback(const std::function<void(const std::string &)> &callback)
{
    confirmCallback = callback;
}

void ButtonDropdown::SetCallbacks(const std::vector<std::function<void()>> callback)
{
    callbacks = callback;
}

void ButtonDropdown::Update(sf::Vector2i &mousePosition, sf::Vector2f positionToBe, sf::RenderWindow &window, sf::Time &time, KeyBindings &keybinds)
{
    dt += time.asSeconds();
    if (dt > 0.25f)
    {
        dt = 0;
        cooldown = 0;
    }

    minus.SetPosition(positionToBe);
    option.SetPosition(positionToBe + sf::Vector2f(130, 0));
    plus.SetPosition(positionToBe + sf::Vector2f(260, 0));
    confirm.SetPosition(positionToBe + sf::Vector2f(330, 0));

    minus.stringText = "<";
    plus.stringText = ">";
    confirm.stringText = "CONFIRM";

    option.stringText = options[selectedOption];

    minus.Update(mousePosition);
    option.Update(mousePosition);
    plus.Update(mousePosition);
    confirm.Update(mousePosition);

    if (cooldown != 0)
        return;
    if (minus.IsClicked(window) || (minus.state == HOVER && keybinds.isActionPressed(KeyBindings::Action::MouseLeftClick)))
    {
        dt = 0;
        if (soundManager)
            soundManager->PlayThisSound("dingDown");
        selectedOption--;
        cooldown = 1;
    }
    if (plus.IsClicked(window) || (plus.state == HOVER && keybinds.isActionPressed(KeyBindings::Action::MouseLeftClick)))
    {
        dt = 0;
        if (soundManager)
            soundManager->PlayThisSound("dingUp");
        selectedOption++;
        cooldown = 1;
    }
    if (confirm.IsClicked(window) || (confirm.state == HOVER && keybinds.isActionPressed(KeyBindings::Action::MouseLeftClick)))
    {
        dt = 0;
        callbacks[selectedOption]();
        cooldown = 1;

        if (soundManager)
            soundManager->PlayThisSound("select");
    }
    if (selectedOption < 0)
        selectedOption = options.size() - 1;
    if (selectedOption >= options.size())
        selectedOption = 0;
}

void ButtonDropdown::Draw(sf::RenderWindow &window)
{
    minus.Draw(window);
    option.Draw(window);
    plus.Draw(window);
    confirm.Draw(window);
}

void ButtonDropdown::SetSelectedOption(int index)
{
    // Validate the provided index
    if (index < 0 || index >= options.size())
    {
        std::cerr << "SetSelectedOption: Index out of bounds (" << index << ")" << std::endl;
        return;
    }

    // Update the selected option index
    selectedOption = index;

    // Log the update for debugging purposes
    //logger.Inform("Setting: " + std::to_string(selectedOption) + " (" + options[selectedOption]
    // + ")\n");
}

std::string ButtonDropdown::GetSelectedOption() const
{
    return options[selectedOption];
}

} // namespace daltonyx
