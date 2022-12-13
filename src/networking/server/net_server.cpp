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

void NetServer::SendPacket(const Packet& packet, uint32_t userId)
{
    if (fUserConnectionMap.find(userId) == fUserConnectionMap.end())
    {
        printf("[server] Could not send packet to user %d. User was not found.\n", userId);
        return;
    }
    SendPacket(packet, fUserConnectionMap.at(userId));
}

void NetServer::SendPacket(const Packet& packet, ENetPeer* peer)
{
    ENetPacket* enPacket = enet_packet_create(NULL, 0, ENET_PACKET_FLAG_RELIABLE);
    enPacket->data = new enet_uint8[packet.Size()];
    enPacket->dataLength = packet.Size();
    memcpy(enPacket->data, &packet.GetType(), sizeof(PacketType));
    memcpy(enPacket->data + sizeof(PacketType), packet.GetBuffer(), packet.Size() - sizeof(PacketType));

    enet_peer_send(peer, 0, enPacket);
}

void NetServer::SendToAll(const Packet& packet)
{
    for (auto& entry : fUserConnectionMap)
    {
        SendPacket(packet, entry.second);
    }
}

void NetServer::SendToAllBut(const Packet& packet, uint32_t notUserId)
{
    for (auto& entry : fUserConnectionMap)
    {
        if (entry.first != notUserId)
        {
            SendPacket(packet, entry.second);
        }
    }
}

uint32_t NetServer::OnUserConnect(ENetPeer* peer)
{
    uint32_t userId = peer->connectID;
    if (fUserConnectionMap.find(userId) != fUserConnectionMap.end())
    {
        if (peer == fUserConnectionMap.at(userId))
        {
            printf("[server] Warning: User allready connected with id %d\n", userId);
            return userId;
        }
        // User id allready taken
        while (fUserConnectionMap.find(++userId) != fUserConnectionMap.end()) {}
    }
    fUserConnectionMap[userId] = peer;
    Scene::GetCurrentScene()->AddOnlinePlayer(userId);
    return userId;
}

void NetServer::OnUserDisconnect(uint32_t userId)
{
    if (fUserConnectionMap.find(userId) != fUserConnectionMap.end())
    {
        fUserConnectionMap.erase(userId);
    }
    Scene::GetCurrentScene()->RemoveOnlinePlayer(userId);
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

    uint32_t userId = OnUserConnect(event.peer);

    event.peer->data = new uint32_t;
    *((uint32_t*)event.peer->data) = userId;

    Packet joinPacket = Packet::Create(PPlayer_Join{userId});
    SendToAllBut(joinPacket, userId);

    for (auto& entry : fUserConnectionMap)
    {
        if (entry.first != userId)
        {
            Packet joinPacket = Packet::Create(PPlayer_Join{entry.first});
            SendPacket(joinPacket, event.peer);
        }
    }
}

void NetServer::ServerEventDisconnect(const ENetEvent& event)
{
    printf("[server] User disconnected from ");
    printAddress(event.peer->address.host);
    printf(":%d\n", event.peer->address.port);

    uint32_t userId = *(uint32_t*)event.peer->data;
    OnUserDisconnect(userId);
    
    delete event.peer->data;
    
    Packet leavePacket = Packet::Create(PPlayer_Leave{userId});
    SendToAllBut(leavePacket, userId);
}

void NetServer::ServerEventTimeout(const ENetEvent& event)
{
    printf("[server] User timed out from ");
    printAddress(event.peer->address.host);
    printf(":%d\n", event.peer->address.port);

    uint32_t userId = *(uint32_t*)event.peer->data;

    OnUserDisconnect(userId);

    delete event.peer->data;
    
    Packet timeOutPacket = Packet::Create(PPlayer_Timeout{userId});
    SendToAllBut(timeOutPacket, userId);
}

void NetServer::ServerEventReceive(const ENetEvent& event)
{
    if (event.packet->data && event.packet->dataLength > 0)
    {
        Packet p = Packet::CreateFromBuffer(event.packet->data, event.packet->dataLength);

        switch (p.GetType())
        {
        case PacketType::Player_Move: OnPlayerMove(p.GetVal<PPlayer_Move>()); break;
        case PacketType::Player_Shoot: OnPlayerShoot(p.GetVal<PPlayer_Shoot>()); break;
        }
    }

    enet_packet_destroy(event.packet);
}

void NetServer::OnPlayerMove(const PPlayer_Move& data)
{
    //printf("[server] Player Move Pos: (%f, %f, %f)\n", data.Position.x, data.Position.y, data.Position.z);
    Packet p = Packet::Create(data);
    SendToAllBut(p, data.PlayerID);

    rp::CollisionBody* body = Scene::GetCurrentScene()->GetEntityCollisionBody(Scene::GetCurrentScene()->GetOnlinePlayer(data.PlayerID));
    rp::Transform currentTransform = body->getTransform();
    currentTransform.setPosition(data.Position);
    body->setTransform(currentTransform);
}

void NetServer::OnPlayerShoot(const PPlayer_Shoot& data)
{
    //rp::CapsuleShape shape;
    rp::PhysicsCommon* physicsCommon = Scene::GetCurrentScene()->GetPhysicsCommon();
    rp::PhysicsWorld* world = Scene::GetCurrentScene()->GetPhysicsWorld();
    
    auto view = Scene::GetCurrentRegistry().view<OnlinePlayerComponent>();

    rp::Ray ray(data.Position, data.Position + (data.Direction * 10000));

    for (auto entity : view)
    {
        rp::CollisionBody* body = Scene::GetCurrentScene()->GetEntityCollisionBody(entity);
        rp::RaycastInfo raycastInfo;
        if (body->raycast(ray, raycastInfo))
        {
            // Player hit!
            printf("PLAYER HIT\n");
            PPlayer_Hit hit;
            hit.PlayerID = Scene::GetCurrentScene()->GetUserId(entity);
            SendToAll(Packet::Create(hit));
        }
    }
}