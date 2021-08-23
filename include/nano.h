#ifndef NANO_NET_H
#define NANO_NET_H

#ifdef __cplusplus
extern "C" {
#endif

/*

==========================================================

>>>>>>>>>>>>>>  NANO NETWORKING LIBRARY   >>>>>>>>>>>>>>>>

Thin layer of abstraction over thethe ENet C library.

Client: Connect to Server's Public IP
LAN Client: Connect to Server's Private IP
Server: Host with Private IP Port Forwarded from Public IP

================================================  @eulogic

*/

#include <enet/enet.h>

/*
-----------------------
 -> Nano Networking  <- 
-----------------------
*/

void initNanoNet();
void exitNanoNet(ENetHost* host);

ENetAddress NNet_adress(const char* host, enet_uint16 port);
ENetPeer* NNet_server(ENetHost* client, ENetAddress* address, size_t channels);
ENetHost* NNet_host_create(ENetAddress* address, size_t clients, size_t channels);
int NNet_connect(ENetHost* client, ENetPeer* server, ENetEvent* event, unsigned int timeout);
unsigned int NNet_read(ENetPacket* packet, void* buffer);
void NNet_send(ENetPeer* server, ENetPacket* packet, void* buffer, size_t size, enet_uint8 channel);
void NNet_broadcast(ENetHost* host, ENetPacket* packet, void* buffer, size_t size, enet_uint8 channel);
void NNet_disconnect(ENetHost* client, ENetPeer* server, ENetEvent* event, unsigned int timeout);

/*
----------------------------
 -> Nano Networking Host  <- 
----------------------------
*/

typedef struct {
    uint8_t id;
    int connected;
    void* buffer;
    unsigned int size;
    ENetEvent event;
    ENetPacket* packet;
    ENetHost* host;
    ENetAddress address;
    ENetPeer* server;
} NNetHost;

NNetHost* NNetHost_create(const char* host, enet_uint16 port, unsigned int clients, unsigned int channels, unsigned int buffer_size, unsigned int timeout);
void NNetHost_free(NNetHost* host);
unsigned int NNetHost_read(NNetHost* client, unsigned int timeout);

#ifdef __cplusplus
}
#endif
#endif
