#pragma once

class NetClient 
{
private:
    void* fHost;
    void* fPeer;
public:
    NetClient();
    ~NetClient();


    void ConnectTo(const char* ipAddress = "127.0.0.1", int port = 25565);
    void Update();

    void Disconnect();
};

int NetInit();
void NetDeinit();