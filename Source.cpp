#include "include/CrashHandler.h"
#include "include/Globals.h"
#include "include/KeyBindings.h"
#include "include/Menu.h"
#include "include/Player.h"
#include "include/World.h"
#include "include/ObjectPlaceable.h"
#include "include/ObjectImmovable.h"
#include "include/SoundManager.h"
#include "include/WindowKeybinds.h"
#include "include/WindowSettings.h"
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <chrono>
#include <Windows.h>
#include <iostream>
#include "include/GameSession.h"

float deltaTime = 0;
int saveNumber = 0;
int width = 1920;
int height = 1080;
unsigned int fpsWanted = 60;
unsigned int debugCooldown = 0;
bool isDebug = false;
bool isFullscreen = false;
bool isVSync = true;
bool isWindowNeedingClosed = false;
bool isMouseOutsideViewport = false;
bool isWindowFocused = true;
bool isPaused = false;
bool isTerminalOpen = false;
float mouseSpeed = 5.f;
sf::Vector2f worldOffset(0, 0);
sf::Vector2u windowSize{ 1280, 720 };
sf::Vector2i windowPosition{ 0, 0 };
sf::Vector2i mousePosition{ 0, 0 };
sf::Vector2f windowSizeF{ 0, 0 };
sf::Clock clickTimer;
const sf::Time clickDelay = sf::milliseconds(150);
sf::Font font;
Logger logger("RES/logs/log.txt");

void SetOSMousePosition(sf::RenderWindow& window, KeyBindings& keybinds)
{
    sf::Vector2i cursorPos = static_cast<sf::Vector2i>(keybinds.cursor.getPosition());
    sf::Vector2i windowPos = window.getPosition();
    SetCursorPos(windowPos.x + cursorPos.x + 10, windowPos.y + cursorPos.y);
}

void AddObject(sf::Vector2f position, sf::Vector2f size, GameSession& gameSession)
{
    auto platform = std::make_shared<ObjectImmovable>(position, size);
    gameSession.AddObject(platform);
}

void GenerateWorld(GameSession& world)
{
    std::cout << "Generating world now..." << std::endl;
    // Create platforms (horizontal surfaces)
    AddObject(sf::Vector2f(200, 1000), sf::Vector2f(400, 50), world);  // Top wall
    AddObject(sf::Vector2f(600, 1000), sf::Vector2f(400, 50), world);  // Top wall
    AddObject(sf::Vector2f(1000, 1000), sf::Vector2f(400, 50), world); // Top wall
    AddObject(sf::Vector2f(1400, 1000), sf::Vector2f(400, 50), world); // Top wall
    AddObject(sf::Vector2f(1800, 1000), sf::Vector2f(400, 50), world); // Top wall

    // Create vertical walls for wall jumps
    AddObject(sf::Vector2f(150, 200), sf::Vector2f(50, 600), world); // Left wall
    AddObject(sf::Vector2f(550, 200), sf::Vector2f(50, 600), world); // Right wall

    // Optionally, add a top wall to prevent the player from going off the screen
    AddObject(sf::Vector2f(200, 50), sf::Vector2f(400, 50), world);  // Top wall
    AddObject(sf::Vector2f(600, 50), sf::Vector2f(400, 50), world);  // Top wall
    AddObject(sf::Vector2f(1000, 50), sf::Vector2f(400, 50), world); // Top wall
    AddObject(sf::Vector2f(1400, 50), sf::Vector2f(400, 50), world); // Top wall
    AddObject(sf::Vector2f(1800, 50), sf::Vector2f(400, 50), world); // Top wall
}


void HandleCollisions(World& world, Player& player)
{
    bool wasGrounded = player.isGrounded;
    player.isGrounded = false;
    player.isWallLeft = false; // Reset wall flags
    player.isWallRight = false;

    sf::Vector2i playerSpatialID = player.spatialID;
    sf::Vector2f checkPos = player.GetPosition() + sf::Vector2f(0, player.GetCollider().GetHalfSize().y + 2);

    // Checking for ground collision as before
    for (int offsetX = -1; offsetX <= 1; ++offsetX)
    {
        for (int offsetY = -1; offsetY <= 1; ++offsetY)
        {
            sf::Vector2i neighborID = playerSpatialID + sf::Vector2i(offsetX, offsetY);

            if (world.spatialMap.count(neighborID) > 0)
            {
                auto& objectsInPartition = world.spatialMap[neighborID];

                for (auto& obj : objectsInPartition)
                {
                    player.HandleCollision(obj);
                }

                for (auto& obj : objectsInPartition)
                {
                    if (obj->GetCollider().CheckPointCollision(checkPos))
                    {
                        player.isGrounded = true;
                        break;
                    }
                }

                if (player.isGrounded)
                    break;
            }
        }
        if (player.isGrounded)
            break;
    }

    if (wasGrounded && !player.isGrounded)
    {
        player.velocity.y = std::max(player.velocity.y, 0.0f);
    }

    // Checking for wall collisions (left and right)
    sf::Vector2f leftCheckPos = player.GetPosition() - sf::Vector2f(2.0f, 0.0f);                                         // 2 pixels to the left of the player's left side
    sf::Vector2f rightCheckPos = player.GetPosition() + sf::Vector2f(player.GetCollider().GetHalfSize().x + 2.0f, 0.0f); // 2 pixels to the right of the player's right side

    // Check if either point is colliding with a wall
    for (int offsetX = -1; offsetX <= 1; ++offsetX)
    {
        for (int offsetY = -1; offsetY <= 1; ++offsetY)
        {
            sf::Vector2i neighborID = playerSpatialID + sf::Vector2i(offsetX, offsetY);

            if (world.spatialMap.count(neighborID) > 0)
            {
                auto& objectsInPartition = world.spatialMap[neighborID];

                for (auto& obj : objectsInPartition)
                {
                    // Check left side collision (point 2 pixels to the left)
                    if (obj->GetCollider().CheckPointCollision(leftCheckPos))
                    {
                        player.isWallLeft = true;
                    }

                    // Check right side collision (point 2 pixels to the right)
                    if (obj->GetCollider().CheckPointCollision(rightCheckPos))
                    {
                        player.isWallRight = true;
                    }
                }
            }
        }
    }
}



int main()
{

#pragma region INIT

    sf::RenderWindow window(sf::VideoMode(width, height), "Animalien", sf::Style::None);
    font.loadFromFile("RES/fonts/JetBrainsMono-Regular.ttf");
    sf::Clock clock;
    sf::Time dt = sf::Time::Zero;
    CrashHandler::Initialize();
    logger.Inform("====================== INITIALIZING ENGINE ======================\n");
    SoundManager soundManager(logger);
    daltonyx::Menu menu(width, soundManager, font);
    KeyBindings keybinds;
    WindowSettings settings("Settings", sf::Vector2f(435, 500), isFullscreen, fpsWanted, window, isVSync, width, height, soundManager, font, keybinds, mouseSpeed);
    WindowKeybinds windowKeybinds("Keybinds", sf::Vector2f(800, 560), window, width, height, keybinds, font);
    window.setVerticalSyncEnabled(isVSync);
    window.setFramerateLimit(fpsWanted);
    menu.SetupMenu(menu, window, width, height, isFullscreen, isDebug, fpsWanted, isVSync, isWindowNeedingClosed, settings.isHidden);
    logger.Inform("================= ENGINE STARTED SUCCESSFULLY ===================\n");

#pragma endregion

    keybinds.loadProfileFromFile("RES/keymaps/XBOX.json");


    World world;


    GameSession gameSession(4);
    GenerateWorld(gameSession);


    auto player1 = std::make_shared<Player>(sf::Vector2f(400, 400), sf::Vector2f(50, 50), 1, "0", sf::IpAddress::LocalHost, 5000, true, true);
    gameSession.AddPlayer(player1, sf::IpAddress::LocalHost, 5000);


    auto player2 = std::make_shared<Player>(sf::Vector2f(600, 400), sf::Vector2f(50, 50), 1, "0", sf::IpAddress::LocalHost, 5001, true, true);
    gameSession.AddPlayer(player2, sf::IpAddress::LocalHost, 5001);
    while (window.isOpen())
    {
        isTerminalOpen = false;
        keybinds.isTerminalOpen = isTerminalOpen;
        sf::Time elapsed = clock.restart();
        deltaTime = elapsed.asSeconds();
        dt += elapsed;

        if (dt.asSeconds() >= 1.f)
        {
            dt = sf::Time::Zero;
            if (debugCooldown > 0)
                debugCooldown--;
        }

        windowSize = window.getSize();
        width = windowSize.x;
        height = windowSize.y;
        windowPosition = window.getPosition();
        mousePosition = sf::Mouse::getPosition(window);
        windowSizeF = sf::Vector2f(width, height);

        if (debugCooldown == 0 && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::F4))
        {
            isDebug = !isDebug;
            debugCooldown = 2;
        }

        if (debugCooldown == 0 && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::F1))
        {
            gameSession.StartRound();
            debugCooldown = 2;
        }

        if (debugCooldown == 0 && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::F2))
        {
            gameSession.EndRound();
            debugCooldown = 2;
        }

        if (debugCooldown == 0 && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LBracket))
        {
            gameSession.StartServer("192.168.1.254", 53000); // Local IP
            debugCooldown = 2;
        }

        if (debugCooldown == 0 && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RBracket))
        {
            gameSession.ConnectToServer("192.168.1.254", 53000); // Local IP
            debugCooldown = 2;
        }

        window.setMouseCursorVisible(false);
        sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
        keybinds.cursor.setPosition(static_cast<float>(mousePosition.x - 10), static_cast<float>(mousePosition.y));

        sf::Event event;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::LostFocus)
            {
                isWindowFocused = false;
                logger.Inform("Lost Window Focus!");
            }
            if (event.type == sf::Event::GainedFocus)
            {
                isWindowFocused = true;
                logger.Inform("Gained Window Focus!");
            }



            if (isWindowFocused)
            {
                settings.HandleEvents(elapsed.asSeconds(), event, keybinds);
                windowKeybinds.HandleEvents(elapsed.asSeconds(), event, keybinds);
                windowKeybinds.Update(window, elapsed, windowSize, windowPosition, mousePosition, worldOffset, keybinds, event);
                menu.HandleEvents(event);

                if (event.type == sf::Event::Closed)
                    isWindowNeedingClosed = true;
                if (event.type == sf::Event::MouseLeft)
                    isMouseOutsideViewport = true;
                if (event.type == sf::Event::MouseEntered)
                    isMouseOutsideViewport = false;
            }
        }



        if (!isMouseOutsideViewport)
        {

            std::vector<std::shared_ptr<daltonyx::Object>> platforms = world.GetAllPlatforms();
            if (isWindowFocused)
            {
                gameSession.Update(deltaTime);


                settings.Update(window, elapsed, windowSize, windowPosition, mousePosition);
                menu.Update(window, width, 50, elapsed, keybinds);
                soundManager.Update("game", deltaTime);
                if (keybinds.isActionPressed(KeyBindings::Action::MouseXNegative))
                {
                    keybinds.updateCursorPosition(keybinds.cursor.getPosition().x - mouseSpeed, keybinds.cursor.getPosition().y);
                    SetOSMousePosition(window, keybinds);
                }
                if (keybinds.isActionPressed(KeyBindings::Action::MouseXPositive))
                {
                    keybinds.updateCursorPosition(keybinds.cursor.getPosition().x + mouseSpeed, keybinds.cursor.getPosition().y);
                    SetOSMousePosition(window, keybinds);
                }
                if (keybinds.isActionPressed(KeyBindings::Action::MouseYNegative))
                {
                    keybinds.updateCursorPosition(keybinds.cursor.getPosition().x, keybinds.cursor.getPosition().y - mouseSpeed);
                    SetOSMousePosition(window, keybinds);
                }
                if (keybinds.isActionPressed(KeyBindings::Action::MouseYPositive))
                {
                    keybinds.updateCursorPosition(keybinds.cursor.getPosition().x, keybinds.cursor.getPosition().y + mouseSpeed);
                    SetOSMousePosition(window, keybinds);
                }
            }

            window.clear();

            for (auto& platform : platforms)
            {
                platform->Draw(window);
                platform->GetCollider().Draw(window);
                platform->UpdateSpatial();
            }

            gameSession.Draw(window);

            settings.Draw(window);
            windowKeybinds.Draw(window, "gameState");
            menu.Draw(window);
            menu.UpdateGameButtons((sf::Vector2f)windowSize, (sf::Vector2f)mousePosition, window);
            keybinds.drawCursor(window);
            window.display();

            if (isWindowNeedingClosed)
            {
                window.close();
            }
        }
    }

    return 0;
}
