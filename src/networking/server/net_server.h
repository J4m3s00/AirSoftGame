#pragma once

namespace AirSoft {

    /**
     * @brief Server for letting multiple people play
     * 
     */
    class NetServer 
    {
    private:
        ENetHost* fHost;
    public:
        NetServer(int port = 25565);
        ~NetServer();

        void Update();

        void ShutDown();

        void SendToAllBut(Packet packet, ENetPeer* not);
    private:
        void ServerEventConnect(const ENetEvent& event);
        void ServerEventDisconnect(const ENetEvent& event);
        void ServerEventTimeout(const ENetEvent& event);
        void ServerEventReceive(const ENetEvent& event);

    private:
        void OnPlayerMove(const PPlayer_Move& data);
    };

}