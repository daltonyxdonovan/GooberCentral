#include "../include/WindowSettings.h"
#include "../include/SoundManager.h"
#include <SFML/Graphics.hpp>
#include <iostream>

WindowSettings::WindowSettings(std::string title, sf::Vector2f size, bool &isFullscreen, unsigned int &fpsWanted, sf::RenderWindow &window, bool &vsyncEnabled, int &width, int &height, SoundManager &soundManager, sf::Font &font, KeyBindings &keybinds, float &mouseSpeed) : font(font), DraggableWindow(title, size, font, keybinds)
{
    this->mainOffset = sf::Vector2f((width / 2) + size.x / 2, height / 2 - size.y / 2);
    this->size = size;
    this->key = sf::Keyboard::Key::F2;
    this->soundManager = &soundManager;

    // Resolution dropdown
    std::vector<sf::VideoMode> availableResolutions = sf::VideoMode::getFullscreenModes();
    std::vector<std::string> resolutionOptions = {};

    for (const auto &resolution : availableResolutions)
    {
        if (resolution.width < 1024 || resolution.height < 768)
            continue;
        resolutionOptions.push_back(std::to_string(resolution.width) + "x" + std::to_string(resolution.height));
    }

    std::vector<std::function<void()>> resolutionCallbacks;

    for (int i = 0; i < resolutionOptions.size(); i++)
    {
        resolutionCallbacks.push_back([availableResolutions, &window, &width, &height, &isFullscreen, i, fpsWanted, vsyncEnabled]() {
            width = availableResolutions[i].width;
            height = availableResolutions[i].height;
            window.create(sf::VideoMode(width, height), "Animalien", isFullscreen ? sf::Style::Fullscreen : sf::Style::None);
            window.setFramerateLimit(fpsWanted);
            window.setVerticalSyncEnabled(vsyncEnabled);
            logger.Inform("Set resolution to: (" + std::to_string(width) + "x" + std::to_string(height) + ")");
        });
    }

    int currentResolutionIndex = 0;
    for (int i = 0; i < availableResolutions.size(); ++i)
    {
        if (availableResolutions[i].width == width && availableResolutions[i].height == height)
        {
            currentResolutionIndex = i;
            break;
        }
    }

    AddDropdown("Display", resolutionOptions, resolutionCallbacks);
    dropdowns.back()->SetSelectedOption(currentResolutionIndex);

    // Fullscreen dropdown
    std::vector<std::string> fullscreenOptions = { "Windowed", "Fullscreen" };
    std::vector<std::function<void()>> fullscreenCallbacks = { [&, fpsWanted, vsyncEnabled]() {
                                                                  isFullscreen = false;
                                                                  window.create(sf::VideoMode(width, height), "Animalien", isFullscreen ? sf::Style::Fullscreen : sf::Style::None);
                                                                  window.setFramerateLimit(fpsWanted);
                                                                  window.setVerticalSyncEnabled(vsyncEnabled);
                                                              },
        [&, fpsWanted, vsyncEnabled]() {
            isFullscreen = true;
            window.create(sf::VideoMode(width, height), "Animalien", isFullscreen ? sf::Style::Fullscreen : sf::Style::None);
            window.setFramerateLimit(fpsWanted);
            window.setVerticalSyncEnabled(vsyncEnabled);
        } };

    AddDropdown("Fullscreen", fullscreenOptions, fullscreenCallbacks);
    dropdowns.back()->SetSelectedOption(isFullscreen ? 1 : 0);

    // FPS dropdown
    // FPS dropdown
    std::vector<std::string> fpsOptions = { "30 FPS", "60 FPS", "75 FPS", "120 FPS", "144 FPS", "240 FPS", "VSync" };
    std::vector<std::function<void()>> fpsCallbacks = { [&]() {
                                                           fpsWanted = 30;
                                                           vsyncEnabled = false;
                                                           window.setFramerateLimit(fpsWanted);
                                                           window.setVerticalSyncEnabled(vsyncEnabled);
                                                           logger.Inform("Set FPS to " + fpsWanted);
                                                       },
        [&]() {
            fpsWanted = 60;
            vsyncEnabled = false;
            window.setFramerateLimit(fpsWanted);
            window.setVerticalSyncEnabled(vsyncEnabled);
            logger.Inform("Set FPS to " + fpsWanted);
        },
        [&]() {
            fpsWanted = 75;
            vsyncEnabled = false;
            window.setFramerateLimit(fpsWanted);
            window.setVerticalSyncEnabled(vsyncEnabled);
            logger.Inform("Set FPS to " + fpsWanted);
        },
        [&]() {
            fpsWanted = 120;
            vsyncEnabled = false;
            window.setFramerateLimit(fpsWanted);
            window.setVerticalSyncEnabled(vsyncEnabled);
            logger.Inform("Set FPS to " + fpsWanted);
        },
        [&]() {
            fpsWanted = 144;
            vsyncEnabled = false;
            window.setFramerateLimit(fpsWanted);
            window.setVerticalSyncEnabled(vsyncEnabled);
            logger.Inform("Set FPS to " + fpsWanted);
        },
        [&]() {
            fpsWanted = 240;
            vsyncEnabled = false;
            window.setFramerateLimit(fpsWanted);
            window.setVerticalSyncEnabled(vsyncEnabled);
            logger.Inform("Set FPS to " + fpsWanted);
        },
        [&]() {
            vsyncEnabled = true;
            fpsWanted = 0; // No fixed framerate
            window.setFramerateLimit(0);
            window.setVerticalSyncEnabled(vsyncEnabled);
            logger.Inform("Set FPS to " + fpsWanted);
        } };

    int currentFpsIndex = std::distance(fpsOptions.begin(), std::find(fpsOptions.begin(), fpsOptions.end(), std::to_string(fpsWanted) + " FPS"));

    AddDropdown("FPS Limit", fpsOptions, fpsCallbacks);
    dropdowns.back()->SetSelectedOption(currentFpsIndex);


    // Mouse speed
    std::vector<std::string> mouseSpeedOptions;
    std::vector<std::function<void()>> mouseSpeedCallbacks;

    for (int i = 1; i <= 20; ++i)
    {
        mouseSpeedOptions.push_back("MouseSpeed: " + std::to_string(i));
        mouseSpeedCallbacks.push_back([&, i]() { mouseSpeed = i; });
    }

    AddDropdown("Mouse Speed (Controller)", mouseSpeedOptions, mouseSpeedCallbacks);
    dropdowns.back()->SetSelectedOption(10);

    // Volume dropdowns
    std::vector<std::string> volumeOptionsMaster;
    std::vector<std::string> volumeOptionsMusic;
    std::vector<std::string> volumeOptionsSound;
    std::vector<std::function<void()>> masterVolumeCallbacks;
    std::vector<std::function<void()>> soundVolumeCallbacks;
    std::vector<std::function<void()>> musicVolumeCallbacks;

    for (int i = 0; i <= 100; i += 5)
    {
        volumeOptionsMaster.push_back("Master Vol: " + std::to_string(i));
        volumeOptionsMusic.push_back("Music Vol: " + std::to_string(i));
        volumeOptionsSound.push_back("Sound Vol: " + std::to_string(i));
        masterVolumeCallbacks.push_back([&, i]() { soundManager.masterVolume = i; });
        soundVolumeCallbacks.push_back([&, i]() { soundManager.soundVolume = i; });
        musicVolumeCallbacks.push_back([&, i]() { soundManager.musicVolume = i; });
    }

    AddDropdown("Master Volume", volumeOptionsMaster, masterVolumeCallbacks);
    dropdowns.back()->SetSelectedOption((int)(soundManager.masterVolume * .2));

    AddDropdown("Sound Volume", volumeOptionsSound, soundVolumeCallbacks);
    dropdowns.back()->SetSelectedOption((int)(soundManager.soundVolume * .2));

    AddDropdown("Music Volume", volumeOptionsMusic, musicVolumeCallbacks);
    dropdowns.back()->SetSelectedOption((int)(soundManager.musicVolume * .2));
}

void WindowSettings::AddDropdown(const std::string &label, const std::vector<std::string> &options, const std::vector<std::function<void()>> &callbacks)
{
    std::string yes = options[0];
    sf::Vector2f position = mainOffset + sf::Vector2f(20, 80 + dropdowns.size() * 60);
    auto dropdown = std::make_unique<daltonyx::ButtonDropdown>(yes, position, *soundManager, font);

    dropdown->AddOptions(options);

    dropdown->SetCallbacks(callbacks);
    dropdowns.push_back(std::move(dropdown));
}

void WindowSettings::Update(sf::RenderWindow &window, sf::Time &elapsedTime, sf::Vector2u &windowSize, sf::Vector2i &windowPosition, sf::Vector2i &mousePosition)
{
    if (isPaused)
        return;
    dt += elapsedTime.asSeconds();
    if (dt > 0.5f)
    {
        dt = 0;
        if (cooldown > 0)
            cooldown--;
    }
    if (isHidden)
        return;
    DraggableWindow::UpdateBase(window, elapsedTime, windowSize, windowPosition, mousePosition);

    for (size_t i = 0; i < dropdowns.size(); ++i)
    {

        sf::Vector2f newPosition = mainOffset + sf::Vector2f(45, 90 + i * 38);
        dropdowns[i]->Update(mousePosition, newPosition, window, elapsedTime, *keybinds);
    }

    if (cooldown == 0 && sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        cooldown = 1;
    }
}

void WindowSettings::Draw(sf::RenderWindow &window)
{
    if (isPaused)
        return;
    if (isHidden)
        return;
    else

    {
        DraggableWindow::DrawBase(window);

        for (auto &dropdown : dropdowns)
        {
            dropdown->Draw(window);
        }
    }
    this->exitButton->Draw(window);
    window.draw(textTitle);
}
