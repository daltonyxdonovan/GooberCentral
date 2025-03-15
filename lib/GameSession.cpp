#include "../include/GameSession.h"
#include <SFML/Network.hpp>
#include <iostream>
#include <random>
#include <sstream>

GameSession::GameSession(int maxPlayers) : maxPlayers(maxPlayers), currentPlayerCount(0), isRoundActive(false), isServerReadyToAcceptPlayers(true)
{
}

GameSession::~GameSession()
{
    for (auto client : clients)
    {
        delete client;
    }
}

void GameSession::AddPlayer(std::shared_ptr<Player>& player, const sf::IpAddress& ip, unsigned short port)
{
    if (currentPlayerCount < maxPlayers && isServerReadyToAcceptPlayers)
    {
        // Generate a unique UUID for the player
        std::string uuid = GenerateUUID();

        // Proceed to add the player to the session
        player->SetUUID(uuid); // Assuming the Player class has a SetUUID method.
        players.push_back(player);
        currentPlayerCount++;

        std::cout << "Player added with UUID: " << uuid << std::endl;
    }
    else
    {
        std::cout << "Server is not accepting players at the moment." << std::endl;
    }
}

void GameSession::RemovePlayer(std::shared_ptr<Player> player)
{
    auto it = std::find(players.begin(), players.end(), player);
    if (it != players.end())
    {
        players.erase(it);
        currentPlayerCount--;
    }
}

void GameSession::AddObject(std::shared_ptr<daltonyx::Object> object)
{
    objects.push_back(object);
}

void GameSession::RemoveObject(std::shared_ptr<daltonyx::Object> object)
{
    auto it = std::find(objects.begin(), objects.end(), object);
    if (it != objects.end())
    {
        objects.erase(it);
    }
}

void GameSession::Update(float dt)
{
    if (!isRoundActive)
        return;

    for (auto& player : players)
    {
        player->Update(dt);
    }

    for (auto& object : objects)
    {
        object->Update(dt); // Update all game objects, not just players
    }

    HandleCollisions();
    SendObjectUpdatesToClients();
    ReceiveObjectUpdatesFromClients();
}

void GameSession::HandleCollisions()
{
    // Handle collisions for non-player objects (e.g., environment)
    for (auto& object : objects)
    {
        for (auto& player : players)
        {
            player->HandleCollision(object); // Player and object collision
        }
    }
}

void GameSession::SendObjectUpdatesToClients()
{
    for (auto client : clients)
    {
        sf::Packet packet;

        // Serialize player data
        for (auto& player : players)
        {
            player->Serialize(packet); // Serialize the player state
        }

        // Serialize all objects
        for (auto& object : objects)
        {
            object->Serialize(packet); // Serialize each object state
        }

        client->send(packet);
    }
}

void GameSession::ReceiveObjectUpdatesFromClients()
{
    for (auto client : clients)
    {
        sf::Packet packet;
        if (client->receive(packet) == sf::Socket::Done)
        {
            // Deserialize player data
            for (auto& player : players)
            {
                player->Deserialize(packet); // Deserialize the player state
            }

            // Deserialize object data
            for (auto& object : objects)
            {
                object->Deserialize(packet); // Deserialize object state
            }
        }
    }
}

void GameSession::Draw(sf::RenderWindow& window)
{
    for (auto& player : players)
    {
        player->Draw(window);
    }

    for (auto& platform : objects)
    {
        platform->Draw(window);
    }
}

void GameSession::StartRound()
{
    isRoundActive = true;
    roundClock.restart();
}

void GameSession::EndRound()
{
    isRoundActive = false;
    // Implement logic to handle end-of-round (e.g., score tracking, round reset)
}

void GameSession::SendToClients(sf::Packet& packet)
{
    for (auto client : clients)
    {
        client->send(packet);
    }
}

void GameSession::ReceiveFromClients(sf::Packet& packet)
{
    for (auto client : clients)
    {
        if (client->receive(packet) == sf::Socket::Done)
        {
            // Handle the received data (e.g., player movement, commands, etc.)
        }
    }
}

std::string GameSession::GenerateUUID()
{
    // Simple random UUID generation using random numbers
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);

    std::stringstream uuid;

    // Create a simple UUID-like string with random hexadecimal characters
    for (int i = 0; i < 8; ++i)
    {
        uuid << std::hex << dis(gen);
    }

    uuid << "-";
    for (int i = 0; i < 4; ++i)
    {
        uuid << std::hex << dis(gen);
    }

    uuid << "-";
    for (int i = 0; i < 4; ++i)
    {
        uuid << std::hex << dis(gen);
    }

    uuid << "-";
    for (int i = 0; i < 4; ++i)
    {
        uuid << std::hex << dis(gen);
    }

    uuid << "-";
    for (int i = 0; i < 12; ++i)
    {
        uuid << std::hex << dis(gen);
    }

    return uuid.str();
}

void GameSession::StartServer(const sf::IpAddress& ip, unsigned short port)
{
    // Initialize the server (listener)
    listener.setBlocking(false); // Make sure it doesn't block while waiting for connections
    if (listener.listen(port) != sf::Socket::Done)
    {
        std::cerr << "Error starting server on port " << port << std::endl;
        return;
    }

    std::cout << "Server started, listening for connections..." << std::endl;

    // Accept new players
    while (isServerRunning)
    {
        sf::TcpSocket* newClient = new sf::TcpSocket;
        if (listener.accept(*newClient) == sf::Socket::Done)
        {
            // Successfully connected, add player and initialize their connection
            std::shared_ptr<Player> newPlayer = std::make_shared<Player>();
            AddPlayer(newPlayer, newClient->getRemoteAddress(), newClient->getRemotePort());
        }
        else
        {
            delete newClient; // Connection attempt failed
        }
    }
}

void GameSession::ConnectToServer(const sf::IpAddress& ip, unsigned short port)
{
    sf::TcpSocket socket;
    if (socket.connect(ip, port) == sf::Socket::Done)
    {
        std::cout << "Successfully connected to the server!" << std::endl;

        // Send some initial connection data or start game logic
        // For example, you might send a "join request" packet
    }
    else
    {
        std::cerr << "Error connecting to server." << std::endl;
    }
}
