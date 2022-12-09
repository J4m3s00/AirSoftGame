#include "air_net.h"


#include <stdio.h>
#include <iostream>

#include <thread>
#include <atomic>



static std::thread updateThread;
static std::atomic<bool> serverRunning;

#if 0
void ServerEventConnect(const ENetEvent& event) 
{
    std::cout << "[server] User connected from "/* << event.peer->address.host*/ << ":" << event.peer->address.port << "." << std::endl;
    //printf("[server] User connected from %x:%d.\n", event.peer->address.host, event.peer->address.port);
}
void ServerEventDisconnect(const ENetEvent& event) 
{
    printf("[server] User disconnected from %x:%d.\n", event.peer->address.host, event.peer->address.port);
}

void ServerEvemtTimeout(const ENetEvent& event) 
{
    printf("[server] User timed out from %x:%d.\n", event.peer->address.host, event.peer->address.port);
}
void ServerEventReceive(const ENetEvent& event) 
{

}

void UpdateServer(ENetHost* server)
{
    ENetEvent event = {0};
    while (enet_host_service(server, &event, 1000) > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT: ServerEventConnect(event); break;
        case ENET_EVENT_TYPE_DISCONNECT: ServerEventDisconnect(event); break;
        case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT: ServerEvemtTimeout(event); break;
        case ENET_EVENT_TYPE_RECEIVE: ServerEventReceive(event); break;
        case ENET_EVENT_TYPE_NONE: break;
        }
    }
}

int main()
{
    serverRunning = true;

    updateThread = std::thread([](){
        if (enet_initialize())
        {
            printf("[server] Error initializing ENet.\n");
            return;
        }

        ENetAddress address = {0};
        address.host = ENET_HOST_ANY;
        address.port = 25565;

        ENetHost* server = enet_host_create(&address, 16, 2, 0, 0);
        if (server == NULL)
        {
            printf("[server] Could not start the server.\n");
            return;
        }

        printf("[server] Started on port %d.\n", address.port);

        while (serverRunning)
        {
            UpdateServer(server);
        }

        enet_host_destroy(server);
    });

    std::cin.get();

    serverRunning = false;

    if (updateThread.joinable())
    {
        updateThread.join();
    }

    printf("[server] Stopped!\n");

    enet_deinitialize();
    return 0;    
}

#else 


int main()
{
    

    serverRunning = true;

    updateThread = std::thread([](){
        if (enet_initialize())
        {
            printf("[server] Error initializing ENet.\n");
            return;
        }
        AirSoft::NetServer server;

        while (serverRunning)
        {
            server.Update();
        }
    });

    std::cin.get();

    serverRunning = false;

    if (updateThread.joinable())
    {
        updateThread.join();
    }

    printf("[server] Stopped!\n");

    enet_deinitialize();
}

#endif