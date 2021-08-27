#include "../TreeNet.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

static unsigned int global_world_seed;

static void send_users_bmp(bmp_t* bitmaps, array_t* users, ENetPeer* peer, ENetPacket* packet)
{
    unsigned int size = users->used, i = 0;
    uint8_t buff[(4096 + sizeof(Packet)) * size];
    Packet* p = users->data;
    for (Packet* end = p + size; p != end; p++) {
        Packet b = packetBmp(p->data[PACKET_ID], size);
        memcpy(&buff[i * (4096 + sizeof(Packet))], &b, sizeof(Packet));
        memcpy(&buff[i * (4096 + sizeof(Packet)) + sizeof(Packet)], bitmaps[p->data[PACKET_ID]].pixels, 4096);
        i++;
    }

    packet = enet_packet_create(&buff[0], size * (4096 + sizeof(Packet)), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 1, packet);
}

static void send_user_bmp_new(bmp_t* bmp, uint8_t id, NNetHost* server)
{
    Packet b = packetBmp(id, 1);
    unsigned int size = 4096 + sizeof(Packet);
    uint8_t buff[size];
    memcpy(&buff[0], &b, sizeof(Packet));
    memcpy(&buff[sizeof(Packet)], bmp->pixels, 4096);
    NNet_broadcast(server->host, server->packet, &buff[0], size, 1);
}

static Packet userNew(uint8_t id)
{
    Packet p;
    memset(&p, 0, sizeof(Packet));
    p.data[PACKET_ID] = id;
    p.data[PACKET_TYPE] = PACKET_TYPE_USER;
    p.data[PACKET_STATE] = NET_CONNECTING;
    return p;
}

static void deltas_commit(array_t* deltas, queue_t* queue, Packet* packet)
{
    uint16_t id = packetId16(packet);

    Packet* p = (Packet*)deltas->data;
    for (Packet* end = p + deltas->used; p != end; p++) {
        if (packetId16(p) == id) {
            if (memcmp(p, packet, sizeof(Packet))) {
                memcpy(p, packet, sizeof(Packet));
                queue_push(queue, packet);
            }
            return;
        }
    }
    array_push(deltas, packet);
    queue_push(queue, packet);
}

static void seed_new()
{
    global_world_seed = rand_uint(rand_uint(time(NULL)));
}

int main(int argc, char** argv)
{
    if (argc > 1) global_world_seed = (unsigned int)atoi(argv[1]);
    else seed_new();

    bmp_t bitmaps[NET_MAX_CLIENT_COUNT];
    for (int i = 0; i < NET_MAX_CLIENT_COUNT; i++) {
        bitmaps[i] = bmp_new(32, 32, 4);
    }

    array_t* users = array_new(NET_MAX_CLIENT_COUNT, sizeof(Packet));
    array_t* deltas = array_new(64, sizeof(Packet));
    queue_t* queue = queue_new(64, sizeof(Packet));

    initNanoNet();
    NNetHost* server = NNetHost_create(NULL, NET_PORT, NET_MAX_CLIENT_COUNT, NET_CHANNELS, NET_BUFFER_SIZE, NET_TIMEOUT);
    printf("TreeNet Server is up and running.\n");
    printf("Server listening on port %u.\n", NET_PORT);
    printf("Global seed is: %u\n", global_world_seed);
    
    Packet seed = packetSeed(global_world_seed);

    bool userConnected = false;
    unsigned int received = 0;
    uint8_t id = 0;
    while (1) {
        while (enet_host_service(server->host, &server->event, 0) > 0) {
            switch (server->event.type) {
                case ENET_EVENT_TYPE_CONNECT: {
                    if (server->event.peer->data == NULL) {
                        id = packetNewId(users);
                        server->event.peer->data = malloc(sizeof(uint8_t));
                        memcpy(server->event.peer->data, &id, sizeof(uint8_t));

                        send_users_bmp(bitmaps, users, server->event.peer, server->packet);
                        Packet user = userNew(id);
                        array_push(users, &user);
                    }
                    userConnected = true;
                    received++;
                    printf("User %u connected.\n", id);
                    break;
                }
                case ENET_EVENT_TYPE_RECEIVE: {
                    id = *(uint8_t*)server->event.peer->data;
                    Packet* user = packetFind(users, id);
                    unsigned int size = NNet_read(server->event.packet, server->buffer) / sizeof(Packet);
                    Packet* p = (Packet*)server->buffer;
                    if (p->data[PACKET_TYPE] == PACKET_TYPE_USER) {
                        memcpy(user, p, sizeof(Packet));
                        if (user->data[PACKET_STATE] == NET_CONNECTING) {
                            user->data[PACKET_STATE] = NET_CONNECTED;
                        }  
                        //printf("Receive:\n");
                        //packetPrint(p);
                        p++;
                        for (unsigned int i = 1; i < size; i++) {
                            //packetPrint(p);
                            deltas_commit(deltas, queue, p++);
                        }
                        received++;
                    } else {
                        p++;
                        memcpy(bitmaps[id].pixels, p, 4096);
                        send_user_bmp_new(&bitmaps[id], id, server);
                    }
                    break;
                }
                case ENET_EVENT_TYPE_DISCONNECT: {
                    id = *(uint8_t*)server->event.peer->data;
                    Packet* user = packetFind(users, id);
                    user->data[PACKET_STATE] = NET_DISCONNECTED;
                    free(server->event.peer->data);
                    server->event.peer->data = NULL;
                    received++;
                    printf("User %u has disconnected.\n", id);
                    break;
                }
                case ENET_EVENT_TYPE_NONE: { break; }
                default: { break; }
            }
        }

        if (users->used && received >= users->used) {
            Packet* p = server->buffer, *start; 
            //printf("Send:\n");
            unsigned int objects_size = 0;
            if (userConnected) {
                objects_size = deltas->used + 1;
                memcpy(p, &seed, sizeof(Packet));
                //packetPrint(p);
                p++;
                start = (Packet*)deltas->data;
                for (Packet* pp = start; pp != start + deltas->used; pp++) {
                    memcpy(p, pp, sizeof(Packet));
                    //packetPrint(p);
                    p++;
                }
                userConnected = false;
                queue->used = 0;
            } else {
                objects_size = queue->used;
                start = (Packet*)queue->data;
                while(!queue_is_empty(queue)) {
                    memcpy(p, queue_pop(queue), sizeof(Packet));
                    //packetPrint(p);
                    p++;
                }
            }

            start = users->data;
            unsigned int index = 0, users_size = users->used;
            for (Packet* u = start; u != start + users_size; u++) {
                memcpy(p, u, sizeof(Packet));
                //packetPrint(p);
                if (u->data[PACKET_STATE] == NET_DISCONNECTED) {
                    array_remove(users, index);
                } else index++;
                p++;
            }

            unsigned int size = (users_size + objects_size) * sizeof(Packet);
            NNet_broadcast(server->host, server->packet, server->buffer, size, 1);
            received = 0;

            if (!users->used) {
                queue->used = 0;
                deltas->used = 0;
                seed_new();
                seed = packetSeed(global_world_seed);
                printf("All users left. Reseted all server data.\n");
                printf("Global seed is: %u\n", global_world_seed);
            }
        }
    }
    exitNanoNet(server->host);
    for (int i = 0; i < NET_MAX_CLIENT_COUNT; i++) {
        bmp_free(&bitmaps[i]);
    }
    array_destroy(users);
    return 0;
}
