#pragma once

#include <SFML/Audio.hpp>
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "Logger.h"

class SoundManager
{
  public:
    std::string biomeWeAreIn = "title";
    std::vector<std::shared_ptr<sf::Sound>> activeSounds;
    int masterVolume = 100;
    int soundVolume = 100;
    int musicVolume = 100;

    SoundManager(Logger &logger);
    ~SoundManager()
    {
        logger->Error("SoundManager Deleted");
    }
    void PlayTitleMusic();
    void PlayMusic();
    void LoadSounds(const std::string &folderPath);
    void LoadMusic(const std::string &folderPath);
    void PlayThisSound(const std::string &soundName);
    void PlayThisSound(const std::string &soundName, int volume);
    void PlayThisSound(const std::string &soundName, int volume, sf::Vector2f worldPositionOfNPC, int width, int height, sf::Vector2f worldOffset);
    void Update(std::string gameState, float dt);
    void StopMusic();
    void TransitionToBiomeMusic(const std::string &newBiome);
    void PlayNextMusicTrack();


  private:
    bool foundTitleSong = false;
    float accumulatedDT = 0;
    int titleSongIndex = -1;
    bool isInGame = false;
    bool isFading = false;
    float fadeTimer = 0.0f;
    float fadeDuration = 1.0f; // Duration of fade effect in seconds
    float initialVolume = 100.0f;
    float targetVolume = 0.0f;
    std::string nextBiome = "";
    std::string currentBiome = "";
    Logger *logger;
    float fadeProgress = 0;
    bool transitioning = false;
    std::vector<std::string> musicTrackNames;
    int currentMusicIndex = -1;
    sf::Music *currentMusic = nullptr;
    sf::Music *newMusic = nullptr;
    sf::Music *fadingOutMusic = nullptr;
    std::unordered_map<std::string, std::unique_ptr<sf::Music>> musicTracks;
    std::unordered_map<std::string, std::shared_ptr<sf::SoundBuffer>> soundBuffers; // Use shared_ptr for sound buffers
};
