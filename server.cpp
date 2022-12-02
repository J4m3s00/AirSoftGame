#include "deps/librg/code/librg.h"
#include "deps/enet.h"


#include <stdio.h>

librg_world *server_world = NULL;


int StartServer(int port)
{
    ENetAddress address = {0};

    address.host = ENET_HOST_ANY; /* Bind the server to the default localhost.     */
    address.port = port; /* Bind the server to port . */

    /* create a server */
    ENetHost *server = enet_host_create(&address, 4, 2, 0, 0);

    if (server == NULL) {
        printf("[server] An error occurred while trying to create an ENet server host.\n");
        return 1;
    }

    printf("[server] Started an ENet server...\n");
    server_world = librg_world_create();

    if (server_world == NULL) {
        printf("[server] An error occurred while trying to create a server world.\n");
        return 1;
    }

    printf("[server] Created a new server world\n");

    /* store our host to the userdata */
    librg_world_userdata_set(server_world, server);

    /* config our world grid */
    librg_config_chunksize_set(server_world, 16, 16, 16);
    librg_config_chunkamount_set(server_world, 9, 9, 9);
    librg_config_chunkoffset_set(server_world, LIBRG_OFFSET_MID, LIBRG_OFFSET_MID, LIBRG_OFFSET_MID);


    return 0;
}

int StopServer()
{
    if (!librg_world_valid(server_world))
        return 1;

    ENetHost *server = (ENetHost *)librg_world_userdata_get(server_world);

    enet_host_destroy(server);
    librg_world_destroy(server_world);
    server_world = NULL;

    return 0;
}

int main()
{
    if (enet_initialize() != 0)
    {
        printf("Error: Could not start enet!\n");
        return 1;
    }
    StartServer(25565);


    StopServer();
    enet_deinitialize();
    return 0;
}