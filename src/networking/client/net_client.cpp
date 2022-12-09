#include "air_net.h"

using namespace AirSoft;

NetClient::NetClient()
    : fHost(nullptr), fPeer(nullptr)
{
    
}

NetClient::~NetClient()
{
    Disconnect();
}

void NetClient::ConnectTo(const char* ipAddress, int port)
{
    ENetHost* host = enet_host_create(NULL, 1, 2, 0, 0);
    if (host == NULL)
    {
        printf("[client] Error create ENet client host.\n");
        return;
    }

    ENetAddress address = {0};
    ENetEvent event = {0};
    ENetPeer* peer = {0};

    enet_address_set_host(&address, ipAddress);
    address.port = port;

    peer = enet_host_connect(host, &address, 2, 0);
    if (peer == NULL)
    {
        printf("[client] Error connecting to host.\n");
        return;
    }

    if (enet_host_service(host, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
    {
        printf("[client] Connected to server succeeded.\n");
    }
    else 
    {
        enet_peer_reset(peer);
        printf("[client] Connection to server failed.\n");
    }

    fHost = host;
    fPeer = peer;
}

void NetClient::Update()
{
    ENetEvent event = { 0 };
    enet_host_service(fHost, &event, 0);
}

void NetClient::Disconnect()
{
    if (!fHost) {return;}
    if (!fPeer) {return;}
    
    enet_peer_disconnect(fPeer, 0);

    ENetEvent event = {0};
    if (enet_host_service(fHost, &event, 1000) == 0 || event.type != ENET_EVENT_TYPE_DISCONNECT)
    {
        enet_peer_reset(fPeer);
    }

    enet_host_destroy(fHost);

    fPeer = NULL;
    fHost = NULL;

}

void NetClient::Send_PlayerMove(rp::Vector3 position, rp::Vector3 velocity)
{
    
}
