#ifndef GLOBALS_H
#define GLOBALS_H
#include <iostream>
#include <SFML/Graphics.hpp>
#include "SoundManager.h"
#include "nlohmann/json.hpp"
#include <chrono>
#include <iostream>
#include <fstream>
#include "Logger.h"
#include "KeyBindings.h"

extern Logger logger;
extern KeyBindings keybinds;
extern SoundManager soundManager;
extern sf::Font font;
extern float deltaTime;
extern unsigned int fpsWanted;

extern bool isPaused;
extern bool isDebug;
extern bool isFullscreen;
extern bool isVSync;
extern bool isTerminalOpen;
extern bool isWindowNeedingClosed;
extern bool isisMouseOutsideViewport;
extern bool isWindowFocused;

extern int width;
extern int height;

extern sf::Vector2u windowSize;
extern sf::Vector2f windowSizeF;
extern sf::Vector2f worldOffset;
extern sf::Vector2i mousePosition;
extern sf::Vector2i windowPosition;

#endif // GLOBALS_H
