#include "../include/Menu.h"

namespace daltonyx
{

Menu::Menu(float width, SoundManager &soundManager, sf::Font &font, float heights) : width(width), height(46)
{
    this->font = font;

    bar.setSize(sf::Vector2f(width - 8, 46));
    bar.setFillColor(sf::Color(24, 24, 24, 255));
    bar.setOutlineColor(sf::Color(50, 50, 50));
    bar.setOutlineThickness(2);
    bar.setPosition(2, 2);
    borderWindow.setSize(sf::Vector2f(width - 4, heights - 4));
    borderWindow.setFillColor(sf::Color(24, 24, 24, 0));
    borderWindow.setOutlineColor(sf::Color(50, 50, 50));
    borderWindow.setOutlineThickness(2);
    borderWindow.setPosition(2, 2);

    titleBgColor = sf::Color(25, 25, 25, 255);
    itemBgColor = sf::Color(25, 25, 25, 255);
    outlineColor = sf::Color(50, 50, 50, 255);
    this->soundManager = &soundManager;

    /*buttonsGame.push_back(std::make_unique<daltonyx::Button>("INVENTORY", sf::Vector2f(), font));
    buttonsGame.back()->SetSize(sf::Vector2f(100, 38));
    buttonsGame.push_back(std::make_unique<daltonyx::Button>("CRAFTING", sf::Vector2f(), font));
    buttonsGame.back()->SetSize(sf::Vector2f(100, 38));
    buttonsGame.push_back(std::make_unique<daltonyx::Button>("MAP", sf::Vector2f(), font));
    buttonsGame.back()->SetSize(sf::Vector2f(100, 38));
    buttonsGame.push_back(std::make_unique<daltonyx::Button>("QUESTS", sf::Vector2f(), font));
    buttonsGame.back()->SetSize(sf::Vector2f(100, 38));*/
}

void Menu::UpdateGameButtons(sf::Vector2f windowSize, sf::Vector2f mousePosition, sf::RenderWindow &window)
{
    sf::Vector2i mousePos = (sf::Vector2i)mousePosition;
    for (int i = 0; i < buttonsGame.size(); i++)
    {
        buttonsGame[i]->SetPosition(sf::Vector2f(windowSize.x - 56 - ((i) * 105), 25));
        buttonsGame[i]->Update(mousePos);
        buttonsGame[i]->Draw(window);
    }

    //if (buttonsGame[0]->IsClicked(window) || (buttonsGame[0]->state == HOVER && keybinds->isActionPressed(KeyBindings::Action::MouseLeftClick)) || keybinds->isActionPressed(KeyBindings::Action::OpenWindowInventory))
    //{
    //    if (gameButtonsCooldown == 0)
    //    {
    //        isInventoryHidden = !isInventoryHidden;
    //        gameButtonsCooldown = 1;
    //        gameButtonsDT = 0;
    //    }
    //    isCraftingHidden = true;
    //    isMapHidden = true;
    //    isQuestsHidden = true;
    //}
    //else if (buttonsGame[1]->IsClicked(window) || (buttonsGame[1]->state == HOVER && keybinds->isActionPressed(KeyBindings::Action::MouseLeftClick)) || keybinds->isActionPressed(KeyBindings::Action::OpenWindowCrafting))
    //{
    //    if (gameButtonsCooldown == 0)
    //    {
    //        isCraftingHidden = !isCraftingHidden;
    //        gameButtonsCooldown = 1;
    //        gameButtonsDT = 0;
    //        isMapHidden = true;
    //        isInventoryHidden = true;
    //        isQuestsHidden = true;
    //    }
    //}
    //else if (buttonsGame[2]->IsClicked(window) || (buttonsGame[2]->state == HOVER && keybinds->isActionPressed(KeyBindings::Action::MouseLeftClick)) || keybinds->isActionPressed(KeyBindings::Action::OpenWindowMap))
    //{
    //    if (gameButtonsCooldown == 0)
    //    {
    //        isMapHidden = !isMapHidden;
    //        gameButtonsCooldown = 1;
    //        gameButtonsDT = 0;
    //        isCraftingHidden = true;
    //        isInventoryHidden = true;
    //        isQuestsHidden = true;
    //    }
    //}
    //else if (buttonsGame[3]->IsClicked(window) || (buttonsGame[3]->state == HOVER && keybinds->isActionPressed(KeyBindings::Action::MouseLeftClick)) || keybinds->isActionPressed(KeyBindings::Action::OpenWindowMap))
    //{
    //    if (gameButtonsCooldown == 0)
    //    {
    //        isQuestsHidden = !isQuestsHidden;
    //        gameButtonsCooldown = 1;
    //        gameButtonsDT = 0;
    //        isMapHidden = true;
    //        isInventoryHidden = true;
    //        isCraftingHidden = true;
    //    }
    //}
}

void Menu::AddDropdown(const std::string &title, const std::vector<std::string> &items, const std::vector<std::function<void()>> &callbacks)
{
    Dropdown dropdown;
    dropdown.title = CreateText(title, 0, 0);
    dropdown.title.setFillColor(sf::Color::White);

    dropdown.titleBackground.setSize(sf::Vector2f(40, height - 8));
    dropdown.titleBackground.setFillColor(sf::Color(25, 25, 25)); // Default color
    dropdown.titleBackground.setPosition(4, 4);
    dropdown.titleBackground.setOutlineColor(sf::Color(50, 50, 50));
    dropdown.titleBackground.setOutlineThickness(2);

    float posY = height;
    for (size_t i = 0; i < items.size(); ++i)
    {
        sf::Text itemText = CreateText(items[i], 0, posY);
        itemText.setFillColor(sf::Color::White);
        dropdown.items.push_back(itemText);
        posY += height;

        dropdown.itemCallbacks.push_back(callbacks[i]);

        sf::RectangleShape itemBackground;
        itemBackground.setSize(sf::Vector2f(std::max((int)itemText.getLocalBounds().width, 100) + itemPadding * 2, 18 + itemPadding * 2));
        itemBackground.setPosition(itemText.getPosition().x - itemPadding, itemText.getPosition().y - itemPadding);
        itemBackground.setFillColor(itemBgColor);
        itemBackground.setOutlineColor(outlineColor);
        itemBackground.setOutlineThickness(outlineThickness);

        dropdown.itemBackgrounds.push_back(itemBackground);
    }

    dropdowns[title] = dropdown;
}

void Menu::Update(const sf::RenderWindow &window, int width, int heights, sf::Time &dt, KeyBindings &keybinds)
{
    this->keybinds = &keybinds;
    if (isPaused)
        return;


    this->versionText.setPosition(sf::Vector2f(width - 15, 15));

    if (this->borderWindow.getSize().x != width - 4)
        borderWindow.setSize(sf::Vector2f(width - 4, heights - 4));

    this->width = width;
    if (this->bar.getSize().x != width - 4)
        this->bar.setSize(sf::Vector2f(width - 4, 46));
    this->dt += dt.asSeconds();
    this->gameButtonsDT += dt.asSeconds();
    if (this->gameButtonsDT > .25f)
    {
        this->gameButtonsDT = 0;
        if (gameButtonsCooldown > 0)
            gameButtonsCooldown--;
    }
    if (this->dt > .25f)
    {
        this->dt = 0;
        if (cooldown > 0)
            cooldown--;
        if (cooldown < 0)
            cooldown = 0;
    }

    sf::Vector2i mousePos = sf::Mouse::getPosition(window);

    float posX = 0.0f;
    int counter = 0;
    // Dropdown is the whole menu ( ex: 'FILE') while Dropdown.item is the sub-option. ex: 'SAVE'
    for (auto &entry : dropdowns)
    {
        const std::string &title = entry.first;
        Dropdown &dropdown = entry.second;
        dropdown.title.setPosition(posX + 18, 15);
        dropdown.titleBackground.setPosition(posX + 6, 6);

        if (cooldown == 0 && dropdown.titleBackground.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))
        {
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left) || keybinds.isActionPressed(KeyBindings::Action::MouseLeftClick))
            {
                soundManager->PlayThisSound("select");
                int counterBuffer = 0;
                for (auto &entry2 : dropdowns)
                {
                    if (counterBuffer != counter)
                    {
                        Dropdown &dropdown2 = entry2.second;
                        dropdown2.isOpen = false;
                    }
                    counterBuffer++;
                }

                dropdown = entry.second;
                dropdown.isOpen = !dropdown.isOpen;
                cooldown = 1;
                this->dt = 0;
            }
            dropdown.titleBackground.setFillColor(sf::Color(100, 100, 100));
        }
        else if (dropdown.isOpen)
        {
            dropdown.titleBackground.setFillColor(sf::Color(90, 90, 90));
        }
        else
        {
            dropdown.titleBackground.setFillColor(sf::Color(70, 70, 70));
        }

        if (dropdown.isOpen)
        {
            float itemPosX = posX + 6;
            float itemPosY = height + 2;
            for (size_t i = 0; i < dropdown.items.size(); ++i)
            {
                dropdown.items[i].setPosition(itemPosX + 10, itemPosY + 12);
                itemPosY += 40;

                if (i < dropdown.itemBackgrounds.size() && dropdown.itemBackgrounds[i].getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))
                {
                    dropdown.items[i].setFillColor(sf::Color(150, 150, 255));
                    if (cooldown == 0 && (sf::Mouse::isButtonPressed(sf::Mouse::Left) || (keybinds.isActionPressed(KeyBindings::Action::MouseLeftClick))))
                    {
                        soundManager->PlayThisSound("selectLow");
                        dropdown.itemCallbacks[i]();
                        dropdown.isOpen = false;
                        this->dt = 0;
                        this->cooldown = 1;
                    }
                }
                else
                {
                    dropdown.items[i].setFillColor(sf::Color::White);
                }
            }
        }

        counter++;
        posX += dropdown.titleBackground.getSize().x + 5.0f;
    }

    leftMouseReleased = false;
}

void Menu::Draw(sf::RenderWindow &window)
{
    window.draw(borderWindow);
    window.draw(bar);
    for (auto &entry : dropdowns)
    {
        Dropdown &dropdown = entry.second;

        dropdown.titleBackground.setFillColor(titleBgColor);
        dropdown.titleBackground.setOutlineColor(outlineColor);
        dropdown.titleBackground.setOutlineThickness(outlineThickness);
        window.draw(dropdown.titleBackground);
        window.draw(dropdown.title);

        if (dropdown.isOpen)
        {
            for (const auto &itemBackground : dropdown.itemBackgrounds)
            {
                window.draw(itemBackground);
            }
            int num = 0;
            for (const auto &item : dropdown.items)
            {
                sf::RectangleShape &itemBackground = dropdown.itemBackgrounds[num];
                itemBackground.setPosition(item.getPosition().x - itemPadding, item.getPosition().y - itemPadding);
                window.draw(item);
                num++;
            }
        }
    }
}

sf::Text Menu::CreateText(const std::string &str, float posX, float posY)
{
    sf::Text text;
    text.setFont(font);
    text.setString(str);
    text.setCharacterSize(18);
    text.setPosition(posX + 15, posY);
    return text;
}

void Menu::SetupMenu(daltonyx::Menu &menu, sf::RenderWindow &window, int &width, int &height, bool &isFullscreen, bool &isDebug, unsigned int &fpsWanted, bool &vsyncEnabled, bool &isWindowNeedingClosed, bool &isHidden)
{
    AddDropdown("X", { "Exit" }, { [&]() { isWindowNeedingClosed = true; } });
    AddDropdown("S", { "Settings" }, { [&]() { isHidden = !isHidden; } });
}

void Menu::HandleEvents(sf::Event &event)
{

    if (isPaused)
        return;
    if (event.type = sf::Event::MouseButtonReleased)
    {
        if (event.mouseButton.button == sf::Mouse::Left)
            leftMouseReleased = true;
    }
}
} // namespace daltonyx
