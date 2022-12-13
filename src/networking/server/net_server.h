#pragma once

namespace AirSoft {

    /**
     * @brief Server for letting multiple people play
     * 
     */
    class NetServer 
    {
    private:
        ENetHost*                               fHost;
        std::unordered_map<uint32_t, ENetPeer*> fUserConnectionMap;
    public:
        NetServer(int port = 25565);
        ~NetServer();

        void Update();

        void ShutDown();

        void SendPacket(const Packet& packet, uint32_t userId);
        void SendPacket(const Packet& packet, ENetPeer* peer);
        void SendToAll(const Packet& packet);
        void SendToAllBut(const Packet& packet, uint32_t notUserId);
    private:
        uint32_t OnUserConnect(ENetPeer* peer);
        void OnUserDisconnect(uint32_t userId);

        void ServerEventConnect(const ENetEvent& event);
        void ServerEventDisconnect(const ENetEvent& event);
        void ServerEventTimeout(const ENetEvent& event);
        void ServerEventReceive(const ENetEvent& event);

    private:
        void OnPlayerMove(const PPlayer_Move& data);
        void OnPlayerShoot(const PPlayer_Shoot& data);
    };

}