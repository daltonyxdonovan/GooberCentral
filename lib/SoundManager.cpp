#include "../include/SoundManager.h"
#include <iostream>
#include <thread>

SoundManager::SoundManager(Logger &logger)
{
    this->logger = &logger;
    logger.Inform("SoundManager Created");
    LoadSounds("RES/sounds");
    LoadMusic("RES/music");
}



void SoundManager::LoadSounds(const std::string &folderPath)
{

    logger->Inform("Loading Sounds");
    int num = 0;
    for (const auto &entry : std::filesystem::directory_iterator(folderPath))
    {
        if (entry.is_regular_file())
        {
            std::string filePath = entry.path().string();
            std::string soundName = entry.path().stem().string();

            auto buffer = std::make_shared<sf::SoundBuffer>();
            if (buffer->loadFromFile(filePath))
            {
                soundBuffers[soundName] = buffer;
                logger->Inform("Loading: " + filePath);
                num++;
            }
            else
            {
                logger->Error("Failed to load: " + filePath);
            }
        }
    }
    logger->Inform("Loaded " + std::to_string(num) + " Sounds!\n");
}

void SoundManager::LoadMusic(const std::string &folderPath)
{
    logger->Inform("Loading Music");
    int num = 0;

    for (const auto &entry : std::filesystem::directory_iterator(folderPath))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".ogg")
        {
            std::string filePath = entry.path().string();
            std::string musicName = entry.path().stem().string();
            auto music = std::make_unique<sf::Music>();

            if (music->openFromFile(filePath))
            {
                logger->Inform("Loading: " + filePath);
                musicTracks[musicName] = std::move(music);
                musicTrackNames.push_back(musicName);
                num++;
            }
            else
            {
                logger->Error("Failed to load: " + filePath);
            }
        }
    }

    logger->Inform("Loaded " + std::to_string(num) + " Musics!\n");
}

void SoundManager::PlayMusic()
{
    if (musicTrackNames.empty())
    {
        logger->Error("No music tracks loaded to play");
        return;
    }

    if (currentMusicIndex == -1)
    {
        currentMusicIndex = 0;
    }

    PlayNextMusicTrack();
}

void SoundManager::StopMusic()
{
    for (auto &music : musicTracks)
        if (music.second->getStatus() == sf::Sound::Status::Playing)
            music.second->stop();
}

void SoundManager::PlayTitleMusic()
{
    if (currentMusic && currentMusic->getStatus() == sf::Music::Playing)
    {
        return;
    }

    if (musicTrackNames.empty())
        return;

    currentMusicIndex = 0;

    if (titleSongIndex == -1)
    {
        for (int i = 0; i < musicTrackNames.size(); i++)
        {
            if (foundTitleSong)
                continue;
            if (musicTrackNames[i] == "titleSong")
            {
                currentMusicIndex = i;
                foundTitleSong = true;
                titleSongIndex = i;
            }
        }
    }
    else
        currentMusicIndex = titleSongIndex;

    const std::string &nextTrackName = musicTrackNames[currentMusicIndex];
    currentMusic = musicTracks[nextTrackName].get();

    if (currentMusic)
    {
        currentMusic->play();
        currentMusic->setVolume(musicVolume * (masterVolume / 100.0f));
        currentMusic->setLoop(true);
    }
    else
    {
        logger->Error("Failed to play Music Track: " + nextTrackName);
    }
}

void SoundManager::PlayNextMusicTrack()
{
    if (currentMusic && currentMusic->getStatus() == sf::Music::Playing)
    {
        return;
    }

    if (musicTrackNames.empty())
        return;

    currentMusicIndex = (currentMusicIndex + 1) % musicTrackNames.size();
    if (currentMusicIndex == titleSongIndex)
        currentMusicIndex++;
    const std::string &nextTrackName = musicTrackNames[currentMusicIndex];
    currentMusic = musicTracks[nextTrackName].get();

    if (currentMusic)
    {
        currentMusic->play();
        currentMusic->setVolume(musicVolume * (masterVolume / 100.0f));
    }
    else
    {
        logger->Error("Failed to play Music Track: " + nextTrackName);
    }
}

void SoundManager::PlayThisSound(const std::string &soundName)
{
    auto it = soundBuffers.find(soundName);
    if (it != soundBuffers.end())
    {
        auto sound = std::make_shared<sf::Sound>();
        sound->setBuffer(*it->second);
        sound->setRelativeToListener(true);
        sound->setVolume(soundVolume * (masterVolume / 100.0f)); // Apply sound and master volume scaling
        sound->setPitch(1 + ((rand() % 4) * .1f));
        sound->play();
        activeSounds.push_back(sound);
    }
}

void SoundManager::PlayThisSound(const std::string &soundName, int volume)
{
    auto it = soundBuffers.find(soundName);
    if (it != soundBuffers.end())
    {
        auto sound = std::make_shared<sf::Sound>();
        sound->setBuffer(*it->second);
        sound->setRelativeToListener(true);
        float adjustedVolume = volume * (soundVolume / 100.0f) * (masterVolume / 100.0f);
        sound->setVolume(adjustedVolume);
        sound->setPitch(1 + ((rand() % 4) * .1f));
        sound->play();
        activeSounds.push_back(sound);
    }
}

void SoundManager::PlayThisSound(const std::string &soundName, int baseVolume, sf::Vector2f worldPositionOfNPC, int width, int height, sf::Vector2f worldOffset)
{
    auto it = soundBuffers.find(soundName);
    if (it != soundBuffers.end())
    {
        auto sound = std::make_shared<sf::Sound>();
        sound->setBuffer(*it->second);
        sound->setPitch(1.0f + ((rand() % 4) * 0.1f));

        sf::Vector2f relativePos = worldPositionOfNPC - worldOffset;
        float distance = std::sqrt(relativePos.x * relativePos.x + relativePos.y * relativePos.y);

        // 100% at 0 distance, 0% at 1000+ distance
        float volumeFactor = std::max(0.0f, 1.0f - (distance / 1000.0f));
        float finalVolume = baseVolume * volumeFactor * (soundVolume / 100.0f) * (masterVolume / 100.0f);
        sound->setVolume(finalVolume);

        sound->setRelativeToListener(true);

        sound->play();
        activeSounds.push_back(sound);
    }
    else
    {
        logger->Error("Sound not found: " + soundName);
    }
}

void SoundManager::TransitionToBiomeMusic(const std::string &newBiome)
{
    if (musicTracks.find(newBiome) == musicTracks.end())
    {
        logger->Error("Biome music not found: " + newBiome);
        return;
    }

    if (currentMusic && currentMusic->getStatus() == sf::Music::Playing)
    {
        fadingOutMusic = currentMusic;
        fadeProgress = 0.0f; // Start fade transition
    }

    // Set the new music track, preserving playback position
    newMusic = musicTracks[newBiome].get();
    sf::Time currentPosition = currentMusic ? currentMusic->getPlayingOffset() : sf::Time::Zero;

    newMusic->setLoop(true);
    newMusic->setVolume(0.0f);                                  // Start silent
    newMusic->play();                                           // Start playing only after setting offset
    newMusic->setVolume(musicVolume * (masterVolume / 100.0f)); // Set volume immediately

    newMusic->setPlayingOffset(currentPosition); // Resume from the same point

    transitioning = true; // Signal that we are in a transition
}

void SoundManager::Update(std::string gameState, float dt)
{
    isInGame = (gameState == "game");

    // Remove finished sounds from active list
    activeSounds.erase(std::remove_if(activeSounds.begin(), activeSounds.end(), [](const std::shared_ptr<sf::Sound> &sound) { return sound->getStatus() == sf::Sound::Stopped; }), activeSounds.end());

    if (transitioning && fadingOutMusic && newMusic)
    {
        accumulatedDT += dt; // Accumulate time

        float fadeDuration = 2.0f; // Duration of fade (1 second)
        fadeProgress += dt / fadeDuration;
        accumulatedDT = 0.0f; // Reset accumulated time

        float fadeOutVolume = (1.0f - fadeProgress) * musicVolume;
        float fadeInVolume = fadeProgress * musicVolume;

        fadingOutMusic->setVolume(fadeOutVolume * (masterVolume / 100.0f));
        newMusic->setVolume(fadeInVolume * (masterVolume / 100.0f));

        if (fadeProgress >= 1.0f)
        {
            fadingOutMusic->stop(); // Stop the old music completely
            fadingOutMusic = nullptr;
            currentMusic = newMusic;
            newMusic = nullptr;
            transitioning = false;
        }
    }
    else if (currentMusic)
    {
        currentMusic->setVolume(musicVolume * (masterVolume / 100.0f));
        if (newMusic)
            newMusic->setVolume(0);
        if (fadingOutMusic)
            fadingOutMusic->setVolume(0);
    }

    if (!transitioning && currentMusic)
    {
        currentMusic->setVolume(musicVolume * (masterVolume / 100.0f));
    }
    if (newMusic)
    {
        newMusic->setVolume(musicVolume * (masterVolume / 100.0f));
    }


    if (isInGame && biomeWeAreIn != currentBiome)
    {
        TransitionToBiomeMusic(biomeWeAreIn + "Song");
        currentBiome = biomeWeAreIn;
    }
}
