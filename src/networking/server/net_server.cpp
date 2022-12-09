#include "air_net.h"

using namespace AirSoft;

NetServer::NetServer(int port)
{
    ENetAddress address = {0};
    address.host = ENET_HOST_ANY;
    address.port = port;

    fHost = enet_host_create(&address, 16, 2, 0, 0);
    if (fHost == NULL)
    {
        printf("[server] Could not start the server.\n");
        return;
    }

    printf("[server] Started on port %d.\n", address.port);
}

NetServer::~NetServer()
{
    ShutDown();
}

void NetServer::Update()
{
    ENetEvent event = {0};
    while (enet_host_service(fHost, &event, 1000) > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT: ServerEventConnect(event); break;
        case ENET_EVENT_TYPE_DISCONNECT: ServerEventDisconnect(event); break;
        case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT: ServerEventTimeout(event); break;
        case ENET_EVENT_TYPE_RECEIVE: ServerEventReceive(event); break;
        case ENET_EVENT_TYPE_NONE: break;
        }
    }
}

void NetServer::ShutDown()
{
    if (fHost)
    {
        enet_host_destroy(fHost);
        fHost = nullptr;
    }
}

void printPaddedHex(uint8_t byte)
{
    char str[2];
    str[0] = (byte >> 4) & 0x0f;
    str[1] = byte & 0x0f;

    for (int i = 0; i < 2; i++)
    {
        if (str[i] > 9) str[i] += 'a';
        else str[i] += '1';

        printf("%c", str[i]);
    }
}

void printAddress(in6_addr addr)
{
    static_assert(sizeof(in6_addr) == 16, "In6 address has not the right size");
    for (int word = 0; word < 8; word++)
    {
        for (int byte = 0; byte < 2; byte++)
        {
            printPaddedHex(addr.u.Byte[(word * 2) + byte]);
        }
        if (word < 8)
        {
            printf(":");
        }
    }
}

void NetServer::ServerEventConnect(const ENetEvent& event)
{
    printf("[server] User connected from ");
    printAddress(event.peer->address.host);
    printf(":%d\n", event.peer->address.port);
}

void NetServer::ServerEventDisconnect(const ENetEvent& event)
{
    printf("[server] User disconnected from ");
    printAddress(event.peer->address.host);
    printf(":%d\n", event.peer->address.port);
}

void NetServer::ServerEventTimeout(const ENetEvent& event)
{
    printf("[server] User timed out from ");
    printAddress(event.peer->address.host);
    printf(":%d\n", event.peer->address.port);
}

void NetServer::ServerEventReceive(const ENetEvent& event)
{
    enet_packet_destroy(event.packet);
}