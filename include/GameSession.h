#pragma once
#include "Player.h"
#include "Object.h"
#include <SFML/Network.hpp>
#include <vector>
#include <memory>
#include <string>
#include <random>

class GameSession
{
  public:
    bool isServerReadyToAcceptPlayers = true;
    GameSession(int maxPlayers);
    ~GameSession();

    void AddPlayer(std::shared_ptr<Player>& player, const sf::IpAddress& ip, unsigned short port);
    void RemovePlayer(std::shared_ptr<Player> player);
    void AddObject(std::shared_ptr<daltonyx::Object> object); // Add generic objects (non-player)
    void RemoveObject(std::shared_ptr<daltonyx::Object> object);
    void Update(float dt);
    void SendObjectUpdatesToClients();
    void ReceiveObjectUpdatesFromClients();
    void Draw(sf::RenderWindow& window);
    void HandleCollisions();

    void StartRound();
    void EndRound();

    // Network methods
    void SendToClients(sf::Packet& packet);
    void ReceiveFromClients(sf::Packet& packet);

  private:
    int maxPlayers;
    int currentPlayerCount;
    std::vector<std::shared_ptr<Player>> players;
    std::vector<std::shared_ptr<daltonyx::Object>> objects; // Objects container
    std::vector<sf::TcpSocket*> clients;                    // For managing client connections
    sf::Clock roundClock;
    bool isRoundActive;

    // Helper method for generating UUID
    std::string GenerateUUID();
};
