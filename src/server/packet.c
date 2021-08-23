#include "../TreeNet.h"
#include <stdio.h>
#include <string.h>

Packet packetBmp(uint8_t id, unsigned int size)
{
    Packet p;
    memset(p.data, 0, sizeof(Packet));
    p.data[PACKET_TYPE] = PACKET_TYPE_BMP;
    p.data[PACKET_STATE] = size;
    p.data[PACKET_ID] = id;
    return p;
}

uint8_t packetNewId(array_t* users)
{
    unsigned int i;
    for (i = 0; i < NET_MAX_CLIENT_COUNT; i++) {
        Packet* p = users->data;
        bool found = false;
        for (unsigned int j = 0; j < users->used; j++) {
            if (i == p->data[PACKET_ID]) {
                found = true;
                break;
            }
            p++;
        }
        if (!found) return i;
    }
    return i;
}

Packet* packetFind(array_t* packets, uint8_t id)
{
    Packet* p = packets->data;
    for (Packet* end = p + packets->used; p != end; p++) {
        if (p->data[PACKET_ID] == id) return p;
    }
    return NULL;
}

uint8_t packetId8(Packet* p)
{
    return p->data[PACKET_ID];
}

uint16_t packetId16(Packet* p)
{
    return *(uint16_t*)&p->data[PACKET_ENTITY_ID];
}

Packet packetSeed(unsigned int seed)
{
    Packet p;
    memset(&p, 0, sizeof(Packet));
    p.data[PACKET_TYPE] = PACKET_TYPE_SEED;
    memcpy(&p.data[PACKET_SEED], &seed, sizeof(unsigned int));
    return p;
}

void packetPrint(Packet* p)
{
    if (p->data[PACKET_TYPE] == PACKET_TYPE_USER) {
        printf(
            "User Id: %u, State: %u, X: %f, Y: %f\n", 
            p->data[PACKET_ID], 
            p->data[PACKET_STATE],
            *(float*)&p->data[PACKET_FLOAT_X], 
            *(float*)&p->data[PACKET_FLOAT_Y]
        );
    } else if (p->data[PACKET_TYPE] == PACKET_TYPE_SEED) {
        printf("Seed: %u\n", *(unsigned int*)&p->data[PACKET_SEED]);
    } else {
        printf(
            "Entity ID: %u, Type: %u, Ammo %u, X: %f, Y: %f, Z: %f\n",
            *(uint16_t*)&p->data[PACKET_ENTITY_ID],
            p->data[PACKET_TYPE],
            p->data[PACKET_OP],
            *(float*)&p->data[PACKET_FLOAT_X], 
            *(float*)&p->data[PACKET_FLOAT_Y],
            *(float*)&p->data[PACKET_FLOAT_Z]
        );
    }
}