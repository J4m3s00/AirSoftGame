#pragma once

#define NOGDI             // All GDI defines and routines
#define NOUSER            // All USER defines and routines
#define WIN32_LEAN_AND_MEAN
#define MMNOSOUND
#include "air_game.h"

#include <stdio.h>


#include "../../deps/enet.h"



#include "packet.h"

#include "client/net_client.h"
#include "server/net_server.h"

