#pragma once

#define PCLASS(type) public: static const PacketType sPacketType = PacketType:: type;

struct vec2 {float x; float y;};
struct vec3 {float x;float y;float z;};


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
    vec3 Position;
    vec3 Velocity;
};

struct PPlayer_Shoot
{
    PCLASS(Player_Shoot)
    vec3 Position;
    vec3 Direction;
    float Spread;
    vec2 SprayOffset;
};

struct PPlayer_Jump
{
    PCLASS(Player_Jump)
    vec3 Position;
    vec3 Velocity;
};

struct PPlayer_ThrowNade
{
    PCLASS(Player_ThrowNade)
    vec3 Position;
    vec3 Velocity;
};

class PacketData
{
private:
    PacketType      fType;
    unsigned char*  fBuffer;
    size_t          fBufferSize;
public:
    PacketData() = default;
    ~PacketData() = default;

    inline uint32_t Size() const
    {
        return sizeof(PacketType) + fBufferSize;
    }

    inline bool Valid() const
    {
        return fBuffer != NULL;
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
    static PacketData CreatePacket(PacketType type, const T& data)
    {
        PacketData result;
        result.fType = type;
        result.fBuffer = new unsigned char[sizeof(T)];
        result.fBufferSize = sizeof(T);
        memcpy(result.fBuffer, &data, sizeof(T));
        return result;
    }
public:
    static PacketData Create(PPlayer_Move data) { return CreatePacket(PacketType::Player_Move, data); }
    static PacketData Create(PPlayer_Jump data) { return CreatePacket(PacketType::Player_Jump, data); }
    static PacketData Create(PPlayer_Shoot data) { return CreatePacket(PacketType::Player_Shoot, data); }
    static PacketData Create(PPlayer_ThrowNade data) { return CreatePacket(PacketType::Player_ThrowNade, data); }    
};