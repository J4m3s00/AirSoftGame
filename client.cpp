#define ENET_IMPLEMENTATION
#include "deps/enet.h"

#include "client.h"

#include <stdio.h>

NetClient::NetClient()
    : fHost(NULL), fPeer(NULL)
{
    
}

NetClient::~NetClient()
{
    Disconnect();
}

void NetClient::ConnectTo(const char* ipAddress, int port)
{
    ENetHost* client = {0};
    client = enet_host_create(NULL, 1, 2, 0, 0);
    if (client == NULL)
    {   
        printf("[client] Error create ENet client host.\n");
        return;
    }

    ENetAddress address = {0};
    ENetEvent event = {0};
    ENetPeer* peer = {0};

    enet_address_set_host(&address, ipAddress);
    address.port = port;

    peer = enet_host_connect(client, &address, 2, 0);
    if (peer == NULL)
    {
        printf("[client] Error connecting to host.\n");
        return;
    }

    if (enet_host_service(client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
    {
        printf("[client] Connected to server succeeded.\n");
    }
    else 
    {
        enet_peer_reset(peer);
        printf("[client] Connection to server failed.\n");
    }

    fHost = (void*) client;
    fPeer = (void*) peer;
}

void NetClient::Update()
{
    ENetEvent event = { 0 };
    enet_host_service((ENetHost*)fHost, &event, 0);

}

void NetClient::Disconnect()
{
    if (!fHost) {return;}
    if (!fPeer) {return;}
    
    enet_peer_disconnect((ENetPeer*)fPeer, 0);

    ENetEvent event = {0};
    if (enet_host_service((ENetHost*)fHost, &event, 1000) == 0 || event.type != ENET_EVENT_TYPE_DISCONNECT)
    {
        enet_peer_reset((ENetPeer*)fPeer);
    }

    enet_host_destroy((ENetHost*)fHost);

    fPeer = NULL;
    fHost = NULL;
}

int NetInit()
{
    int result = enet_initialize();
    if (result != 0)
    {
        printf("[net] Could not start ENet.\n");
    }
    return result;
}

void NetDeinit()
{
    enet_deinitialize();
}