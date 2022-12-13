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
    while (enet_host_service(fHost, &event, 0) > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:
        {
            break;
        }
        case ENET_EVENT_TYPE_RECEIVE:
        {
            Packet p = Packet::CreateFromBuffer(event.packet->data, event.packet->dataLength);
            switch (p.GetType())
            {
            case PacketType::Player_Join:
            {
                PPlayer_Join joinData = p.GetVal<PPlayer_Join>();
                printf("[client] User %u connected.\n", joinData.PlayerID);
                Scene::GetCurrentScene()->AddOnlinePlayer(joinData.PlayerID);
                break;
            }
            case PacketType::Player_Leave:
            {
                PPlayer_Leave leaveData = p.GetVal<PPlayer_Leave>();
                printf("[client] User %u disconnected.\n", leaveData.PlayerID);
                Scene::GetCurrentScene()->RemoveOnlinePlayer(leaveData.PlayerID);
                break;
            }
            case PacketType::Player_Timeout:
            {
                PPlayer_Timeout timeoutData = p.GetVal<PPlayer_Timeout>();
                printf("[client] User %u timed out.\n", timeoutData.PlayerID);
                Scene::GetCurrentScene()->RemoveOnlinePlayer(timeoutData.PlayerID);
                break;
            }
            case PacketType::Player_Move:
            {
                PPlayer_Move moveData = p.GetVal<PPlayer_Move>();
                entt::entity playerEntity = Scene::GetCurrentScene()->GetOnlinePlayer(moveData.PlayerID);
                OnlinePlayerComponent& opc = Scene::GetCurrentScene()->GetRegistry().get<OnlinePlayerComponent>(playerEntity);
                opc.Position = moveData.Position;
                break;
            }
            }
            break;
        }
        }
    }
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
    Packet p = Packet::Create(PPlayer_Move{fPeer->connectID, position, velocity});

    SendPacket(p);

    p.Release();
}

void NetClient::SendPacket(const Packet& packet)
{
    ENetPacket* enPacket = enet_packet_create(NULL, 0, ENET_PACKET_FLAG_RELIABLE);
    enPacket->data = new enet_uint8[packet.Size()];
    enPacket->dataLength = packet.Size();
    memcpy(enPacket->data, &packet.GetType(), sizeof(PacketType));
    memcpy(enPacket->data + sizeof(PacketType), packet.GetBuffer(), packet.Size() - sizeof(PacketType));
 
    enet_peer_send(fPeer, 0, enPacket);
}
