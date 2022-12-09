#pragma once

namespace AirSoft {

    /**
     * @brief Represents a Player in the network
     * 
     */
    class NetClient
    {
    private:
        ENetHost* fHost;
        ENetPeer* fPeer;
    public:
        NetClient();
        ~NetClient();

        void ConnectTo(const char* ipAddress = "127.0.0.1", int port = 25565);
        void Update();

        void Disconnect();

        void Send_PlayerMove(rp::Vector3 position, rp::Vector3 velocity);
    private:
        void SendPacket(const Packet& packet);
    };

}