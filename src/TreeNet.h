#ifndef TREE_NET_H
#define TREE_NET_H

#include "Tree.h"

#define NET_MAX_CLIENT_COUNT 16
#define NET_PORT 7777
#define NET_CHANNELS 2
#define NET_BUFFER_SIZE 4096 + 16
#define NET_TIMEOUT 5000

#define PACKET_SIZE 16

typedef enum {
    NET_DISCONNECTED,
    NET_CONNECTING,
    NET_CONNECTED
} netStateEnum;

typedef struct {
    uint8_t data[PACKET_SIZE];
} Packet;

/* PLAYER / USER
0 -> ID
1 -> CONNECTED / CONNECTING/ DISCONNECTED
2 -> PACKET TYPE USER
3 -> OP -> WALLSLIDING / DASHING / JETPACKING
4 -> FLOAT X
8 -> FLOAT Y
12 -> Sprite ID
13 -> Orientation

 WEAPON
0 -> Entity ID
2 -> Packet Type Gun Loose or Used
3 -> AMMO
4 -> FLOAT X
8 -> FLOAT Y
12 -> FLOAT ROT

*/

typedef enum {
    PACKET_ID = 0,
    PACKET_ENTITY_ID = 0,
    PACKET_STATE = 1,
    PACKET_TYPE = 2,
    PACKET_OP = 3,
    PACKET_SEED = 4,
    PACKET_FLOAT_X = 4,
    PACKET_FLOAT_Y = 8,
    PACKET_FLOAT_Z = 12,
    PACKET_SPRITE_ID = 12,
    PACKET_ORIENTATION = 13
} packetBitsEnum;

typedef enum {
    PACKET_TYPE_NONE,
    PACKET_TYPE_USER,
    PACKET_TYPE_SEED,
    PACKET_TYPE_BMP,
    PACKET_TYPE_GUN_LOOSE,
    PACKET_TYPE_GUN_USED,
    PACKET_TYPE_JETPACK_LOOSE,
    PACKET_TYPE_JETPACK_USED,
    PACKET_TYPE_GRANADE_PICK,
    PACKET_TYPE_GRANADE_THROW,
    PACKET_TYPE_GRANADE_DROP,
    PACKET_TYPE_GRANADE_EXPLODE,
    PACKET_TYPE_FIREBARREL_EXPLODE
} PacketTypeEnum;

typedef enum {
    PACKET_OP_NONE,
    PACKET_OP_WALLSLIDING,
    PACKET_OP_DASHING,
    PACKET_OP_JETPACKING
} PacketOpEnum;

uint8_t packetNewId(array_t* users);
Packet* packetFind(array_t* packets, uint8_t id);
void packetPrint(Packet* p);
uint16_t packetId16(Packet* p);
uint8_t packetId8(Packet* p);
Packet packetSeed(unsigned int seed);
Packet packetBmp(uint8_t id, unsigned int size);

#endif