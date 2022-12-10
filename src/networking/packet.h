#pragma once

#define PCLASS(type) public: static const PacketType sPacketType = PacketType:: type;

#define PACK_PLAYER uint32_t PlayerID;

// Basic Network packet
enum class PacketType
{
    UNKNOWN,

    Player_Join,
    Player_Leave,
    Player_Timeout,
    Player_Move,
    Player_Shoot,
    Player_Jump,
    Player_ThrowNade
};


// Packet data structs

struct PPlayer_Join
{
    PCLASS(Player_Join)
    PACK_PLAYER
};

struct PPlayer_Leave
{
    PCLASS(Player_Leave)
    PACK_PLAYER
};
struct PPlayer_Timeout
{
    PCLASS(Player_Timeout)
    PACK_PLAYER
};

struct PPlayer_Move
{
    PCLASS(Player_Move)
    PACK_PLAYER
    Vec3 Position;
    Vec3 Velocity;
};

struct PPlayer_Shoot
{
    PCLASS(Player_Shoot)
    PACK_PLAYER
    Vec3 Position;
    Vec3 Direction;
    float Spread;
    Vec2 SprayOffset;
};

struct PPlayer_Jump
{
    PCLASS(Player_Jump)
    PACK_PLAYER
    Vec3 Position;
    Vec3 Velocity;
};

struct PPlayer_ThrowNade
{
    PCLASS(Player_ThrowNade)
    PACK_PLAYER
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
    static Packet Create(PPlayer_Join data) { return CreatePacket(PacketType::Player_Join, data); }
    static Packet Create(PPlayer_Leave data) { return CreatePacket(PacketType::Player_Leave, data); }
    static Packet Create(PPlayer_Timeout data) { return CreatePacket(PacketType::Player_Timeout, data); }
    static Packet Create(PPlayer_Move data) { return CreatePacket(PacketType::Player_Move, data); }
    static Packet Create(PPlayer_Jump data) { return CreatePacket(PacketType::Player_Jump, data); }
    static Packet Create(PPlayer_Shoot data) { return CreatePacket(PacketType::Player_Shoot, data); }
    static Packet Create(PPlayer_ThrowNade data) { return CreatePacket(PacketType::Player_ThrowNade, data); }    


    
    static Packet CreateFromBuffer(void* buffer, size_t bufferSize)
    {
        Packet result{};
        result.fType = *(PacketType*)buffer;
        void* ptr = ((uint8_t*)buffer) + sizeof(PacketType);
        
        uint32_t packetDataSize = bufferSize - sizeof(PacketType);
        result.fBuffer = new uint8_t[packetDataSize];
        result.fBufferSize = packetDataSize;
        memcpy(result.fBuffer, ptr, packetDataSize);
        
        return result;
    }
};