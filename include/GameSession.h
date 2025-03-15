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
    sf::TcpListener listener;
    std::vector<sf::TcpSocket*> clients;
    bool isServerRunning;



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

    void StartServer(const sf::IpAddress& ip, unsigned short port);

    void ConnectToServer(const sf::IpAddress& ip, unsigned short port);


    // Network methods
    void SendToClients(sf::Packet& packet);
    void ReceiveFromClients(sf::Packet& packet);

  private:
    int maxPlayers;
    int currentPlayerCount;
    std::vector<std::shared_ptr<Player>> players;
    std::vector<std::shared_ptr<daltonyx::Object>> objects; // Objects container
    sf::Clock roundClock;
    bool isRoundActive;

    // Helper method for generating UUID
    std::string GenerateUUID();
};
