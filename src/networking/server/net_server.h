#pragma once

namespace AirSoft {

    /**
     * @brief Server for letting multiple people play
     * 
     */
    class NetServer 
    {
        ENetHost* fHost;
    public:
        NetServer(int port = 25565);
        ~NetServer();

        void Update();

        void ShutDown();
    private:
        void ServerEventConnect(const ENetEvent& event);
        void ServerEventDisconnect(const ENetEvent& event);
        void ServerEventTimeout(const ENetEvent& event);
        void ServerEventReceive(const ENetEvent& event);
    };

}