#include "air_net.h"

#include <stdio.h>
#include <iostream>

#include <thread>
#include <atomic>


static std::thread updateThread;
static std::atomic<bool> serverRunning;


int main()
{
    // Min req
    AirSoft::GameInstance::Instance().CreateScene();

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
