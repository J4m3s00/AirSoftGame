#pragma once

#define PCLASS(type) public: static const PacketType sPacketType = PacketType:: type;

// Basic Network packet
enum class PacketType
{
    UNKNOWN,

    User_Connect,
    User_Disconnect,
    User_Timeout,
    Player_Move,
    Player_Shoot,
    Player_Jump,
    Player_ThrowNade
};


// Packet data structs
struct PPlayer_Move
{
    PCLASS(Player_Move)
    Vec3 Position;
    Vec3 Velocity;
};

struct PPlayer_Shoot
{
    PCLASS(Player_Shoot)
    Vec3 Position;
    Vec3 Direction;
    float Spread;
    Vec2 SprayOffset;
};

struct PPlayer_Jump
{
    PCLASS(Player_Jump)
    Vec3 Position;
    Vec3 Velocity;
};

struct PPlayer_ThrowNade
{
    PCLASS(Player_ThrowNade)
    Vec3 Position;
    Vec3 Velocity;
};

class Packet
{
private:
    PacketType      fType;
    unsigned char*  fBuffer;
    size_t          fBufferSize;
public:
    Packet() = default;
    ~Packet() = default;

    inline uint32_t Size() const
    {
        return sizeof(PacketType) + fBufferSize;
    }

    inline bool Valid() const
    {
        return fBuffer != NULL;
    }

    inline const PacketType& GetType() const 
    {
        return fType;
    }

    inline const void* GetBuffer() const 
    {
        return fBuffer;
    }

    inline void Release()
    {
        if (Valid())
        {
            delete fBuffer;
        }
        fBuffer = NULL;
        fBufferSize = 0;
    }

    template <typename T>
    T GetVal() const
    {
        if (!Valid())
        {
            printf("No Data in Packet.\n");
            return T{};
        }
        if (T::sPacketType != fType)
        {
            printf("Trying to get wrong data from Packet.\n");
            return T{};
        }
        if (sizeof(T) != fBufferSize)
        {
            printf("The data of a packet is corrupted.\n");
            return T{};
        }
        T result{};
        memcpy(&result, fBuffer, fBufferSize);
        return result;
    }

    void FromBuffer(void* buffer, size_t bufferSize)
    {
        Release();
        fType = *(PacketType*)buffer;
        void* ptr = ((uint8_t*)buffer) + sizeof(PacketType);
        
        uint32_t packetDataSize = bufferSize - sizeof(PacketType);
        fBuffer = new uint8_t[packetDataSize];
        fBufferSize = packetDataSize;
        memcpy(fBuffer, ptr, packetDataSize);
    }
private:
    template <typename T>
    static Packet CreatePacket(PacketType type, const T& data)
    {
        Packet result;
        result.fType = type;
        result.fBuffer = new unsigned char[sizeof(T)];
        result.fBufferSize = sizeof(T);
        memcpy(result.fBuffer, &data, sizeof(T));
        return result;
    }
public:
    static Packet Create(PPlayer_Move data) { return CreatePacket(PacketType::Player_Move, data); }
    static Packet Create(PPlayer_Jump data) { return CreatePacket(PacketType::Player_Jump, data); }
    static Packet Create(PPlayer_Shoot data) { return CreatePacket(PacketType::Player_Shoot, data); }
    static Packet Create(PPlayer_ThrowNade data) { return CreatePacket(PacketType::Player_ThrowNade, data); }    
};