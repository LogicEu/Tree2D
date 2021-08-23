#include "../TreeNet.h"
#include <string.h>
#include <stdio.h>

#define FILE_NET_MODULE "assets/data/.net_module.bin"
#define FILE_NET_MAP "assets/data/.net_map"

static char clientUsername[256] = "\0";
static char serverIp[16] = "\0";
static NNetHost* client = NULL;
static array_t* netEntities = NULL; 
static unsigned int received;

extern unsigned int randSeed;
extern Mesh mesh;
extern unsigned int currentPlayerSprite;

unsigned int sprites[NET_MAX_CLIENT_COUNT];

extern bmp_t bmp;
extern Entity player;
extern Entity usedWeapon;
extern Entity jetpack;
extern vec2 spawnPoint;
extern Entity granades[GRANADE_MAX];
extern unsigned int granadeCount;

Entity netUsedWeapon;
Entity netJetpack;
Entity netGranadePick;
Entity netGranadeThrow;
Entity netGranadeExp;
Entity netFirebarrelExp;
queue_t* netGranadeDrop = NULL;

extern bool stateWallSliding;
extern bool stateDashing;
extern bool stateJetpacking;

typedef struct {
    uint8_t id;
    Entity entity;
} NetEntity;

static NetEntity netEntityCreate(uint8_t id, Entity e)
{
    NetEntity n = {id, e};
    return n; 
}

static Entity netEntityFindById(uint8_t id)
{
    NetEntity* n = netEntities->data;
    for (NetEntity* end = n + netEntities->used; n != end; n++) {
        if (n->id == id) return n->entity;
    }
    return 0;
}

static void netPlayerUpdate(Entity entity, Packet* packet)
{
    Rect* PhiRect = (Rect*)entity_get(entity, COMPONENT_PHI_RECT);
    memcpy(&PhiRect->x, &packet->data[PACKET_FLOAT_X], sizeof(float));
    memcpy(&PhiRect->y, &packet->data[PACKET_FLOAT_Y], sizeof(float));

    Rect* glRect = (Rect*)entity_get(entity, COMPONENT_GL_RECT);
    unsigned int* sprite = (unsigned int*)entity_get(entity, COMPONENT_SPRITE_ID);
    unsigned int sprite_pckg = (unsigned int)packet->data[PACKET_SPRITE_ID];
    int orientation = (int)(unsigned int)packet->data[PACKET_ORIENTATION];
    orientation = orientation * 2 - 1;

    if (entity != player) {
        if (packet->data[PACKET_OP] == PACKET_OP_WALLSLIDING) {
            smokeEmit(vec2_new(PhiRect->x + (float)orientation * 8.0f, PhiRect->y - 12.0f), TEXTURE_SMOKE);
        }
        else if (packet->data[PACKET_OP] == PACKET_OP_JETPACKING) {
            smokeEmit(vec2_new(PhiRect->x, PhiRect->y - 6.0f), TEXTURE_SMOKE2);
        }
        else if (packet->data[PACKET_OP] == PACKET_OP_DASHING) {
            shadowEmit(entity, (float)orientation);
        }
        else if (*sprite != SPRITE_JUMPING && sprite_pckg == SPRITE_JUMPING) {
            smokeEmit(vec2_new(PhiRect->x, PhiRect->y - 12.0f), TEXTURE_SMOKE);
        }
        else if (sprite_pckg == SPRITE_RUNNING) {
            smokeEmit(vec2_new(PhiRect->x, PhiRect->y - 12.0f), TEXTURE_SMOKE);
        }
    }
    
    *sprite = sprite_pckg;
    if ((glRect->w > 0.0f && orientation < 0) ||
        (glRect->w < 0.0f && orientation > 0)) {
        glRect->w *= -1.0f;
    }

    memcpy(&glRect->x, &PhiRect->x, sizeof(float));
    memcpy(&glRect->y, &PhiRect->y, sizeof(float));
    //sprite_frame_update(assetsGetSprite(currentPlayerSprite, *sprite));
}

static void netWeaponUpdate(Packet* p)
{
    Entity e = (Entity)packetId16(p);
    if (e >= entity_count() || !entity_has(e, COMPONENT_GUN_CONTROLLER)) return;
    if (e == netUsedWeapon || e == usedWeapon) return;

    Rect* r = (Rect*)entity_get(e, COMPONENT_PHI_RECT);
    Rect* rg = (Rect*)entity_get(e, COMPONENT_GL_RECT);
    GunType* gun = (GunType*)entity_get(e, COMPONENT_GUN_CONTROLLER);
    float* rot = (float*)entity_get(e, COMPONENT_ROTATION);
    bool* g = (bool*)entity_get(e, COMPONENT_GRAVITY);

    unsigned int ammo = (unsigned int)p->data[PACKET_OP];
    if (ammo < gun->ammo) {
        gunShoot(e);
        gun->ammo = ammo;
    }

    memcpy(rot, &p->data[PACKET_FLOAT_Z], sizeof(float));
    if (fabs(*rot) > M_PI * 0.5) {
        if (rg->h > 0.0f) rg->h *= -1.0f;
    } else if (rg->h <= 0.0f) rg->h *= -1.0f;

    if (p->data[PACKET_TYPE] == PACKET_TYPE_GUN_USED && gun->state != GUN_STATE_COLLECTED) {
        memset(entity_get(e, COMPONENT_VEL_VEC2), 0, sizeof(vec2));
        *g = false;
        gun->state = GUN_STATE_COLLECTED;
    } else if (p->data[PACKET_TYPE] == PACKET_TYPE_GUN_LOOSE && gun->state != GUN_STATE_LOOSE) {
        memset(entity_get(e, COMPONENT_VEL_VEC2), 0, sizeof(vec2));
        *g = true;
        gun->state = GUN_STATE_LOOSE;
    }

    memcpy(&r->x, &p->data[PACKET_FLOAT_X], sizeof(float));
    memcpy(&r->y, &p->data[PACKET_FLOAT_Y], sizeof(float));
    memcpy(&rg->x, &p->data[PACKET_FLOAT_X], sizeof(float));
    memcpy(&rg->y, &p->data[PACKET_FLOAT_Y], sizeof(float));
}

static void netJetpackUpdate(Packet* p)
{
    Entity e = (Entity)packetId16(p);
    if (e >= entity_count() || !entity_has(e, COMPONENT_JETPACK)) return;
    if (e == netJetpack || e == jetpack) return;

    unsigned int* state = (unsigned int*)entity_get(e, COMPONENT_JETPACK);
    Rect* r = (Rect*)entity_get(e, COMPONENT_PHI_RECT);
    Rect* rg = (Rect*)entity_get(e, COMPONENT_GL_RECT);
    unsigned int* fuel = (unsigned int*)entity_get(e, COMPONENT_AMMO);
    bool* g = (bool*)entity_get(e, COMPONENT_GRAVITY);
    int orientation = (int)(unsigned int)p->data[PACKET_ORIENTATION];
    orientation = orientation * 2 - 1;

    if (p->data[PACKET_TYPE] == PACKET_TYPE_JETPACK_USED && *state != JETPACK_COLLECTED) {
        memset(entity_get(e, COMPONENT_VEL_VEC2), 0, sizeof(vec2));
        *g = false;
        *state = JETPACK_COLLECTED;
    } else if (p->data[PACKET_TYPE] == PACKET_TYPE_JETPACK_LOOSE && *state != JETPACK_LOOSE) {
        memset(entity_get(e, COMPONENT_VEL_VEC2), 0, sizeof(vec2));
        *g = true;
        *state = JETPACK_LOOSE;
    }

    if ((rg->w > 0.0f && orientation < 0) ||
        (rg->w < 0.0f && orientation > 0)) {
        rg->w *= -1.0f;
    }

    memcpy(&r->x, &p->data[PACKET_FLOAT_X], sizeof(float));
    memcpy(&r->y, &p->data[PACKET_FLOAT_Y], sizeof(float));
    memcpy(&rg->x, &p->data[PACKET_FLOAT_X], sizeof(float));
    memcpy(&rg->y, &p->data[PACKET_FLOAT_Y], sizeof(float));
    *fuel = (unsigned int)p->data[PACKET_OP];
}

static void netGranadeUpdate(Packet* p)
{
    Entity e = (Entity)packetId16(p);
    if (e >= entity_count() || !entity_has(e, COMPONENT_GRANADE)) return;
    for (unsigned int i = 0; i < granadeCount; i++) {
        if (granades[i] == e) return;
    }

    Rect* r = (Rect*)entity_get(e, COMPONENT_PHI_RECT);
    Rect* rg = (Rect*)entity_get(e, COMPONENT_GL_RECT);
    GranadeComponent* granade = (GranadeComponent*)entity_get(e, COMPONENT_GRANADE);
    if (p->data[PACKET_TYPE] == PACKET_TYPE_GRANADE_PICK && granade->state != GRANADE_COLLECTED) {
        granadeCollect(e);
    } else if (p->data[PACKET_TYPE] == PACKET_TYPE_GRANADE_THROW && granade->state != GRANADE_THROWED) {
        granadeThrow(e, vec2_new(r->x, r->y), *(float*)&p->data[PACKET_FLOAT_Z]);
    } else if (p->data[PACKET_TYPE] == PACKET_TYPE_GRANADE_DROP) {
        granadeDrop(e, *(vec2*)&p->data[PACKET_FLOAT_X]);
    } else if (p->data[PACKET_TYPE] == PACKET_TYPE_GRANADE_EXPLODE) {
        if (granade->state == GRANADE_THROWED) {
            archetypeBlast(*(vec2*)r, 80.0f);
        } 
        entity_destroy(e);
        return;
    }

    memcpy(&r->x, &p->data[PACKET_FLOAT_X], sizeof(float));
    memcpy(&r->y, &p->data[PACKET_FLOAT_Y], sizeof(float));
    memcpy(&rg->x, &p->data[PACKET_FLOAT_X], sizeof(float));
    memcpy(&rg->y, &p->data[PACKET_FLOAT_Y], sizeof(float));
}

static void netFirebarrelUpdate(Packet* p)
{
    Entity e = (Entity)packetId16(p);
    if (e >= entity_count() || !entity_has(e, COMPONENT_FIREBARREL)) return;
    entity_destroy(e);
}

static void packetParse(Packet* p)
{
    if (p->data[PACKET_TYPE] == PACKET_TYPE_GUN_LOOSE || p->data[PACKET_TYPE] == PACKET_TYPE_GUN_USED) {
        netWeaponUpdate(p);
    } else if (p->data[PACKET_TYPE] == PACKET_TYPE_JETPACK_LOOSE || p->data[PACKET_TYPE] == PACKET_TYPE_JETPACK_USED) {
        netJetpackUpdate(p);
    } else if (p->data[PACKET_TYPE] >= PACKET_TYPE_GRANADE_PICK && p->data[PACKET_TYPE] <= PACKET_TYPE_GRANADE_EXPLODE) {
        netGranadeUpdate(p);
    } else if (p->data[PACKET_TYPE] == PACKET_TYPE_FIREBARREL_EXPLODE) {
        netFirebarrelUpdate(p);
    }
}

static void netRead()
{
    if (received) return;
    received = NNetHost_read(client, 0);
    if (!received) return;

    bool entire_world_package = false;
    uint8_t disconnected = 255;
    Packet* p = client->buffer;
    unsigned int size = received / sizeof(Packet);
    for (unsigned int i = 0; i < size; i++) {
        if (p->data[PACKET_TYPE] == PACKET_TYPE_USER) {
            uint8_t id = p->data[PACKET_ID];
            Entity e = netEntityFindById(id);
            if (e) {
                if (p->data[PACKET_STATE] == NET_DISCONNECTED) disconnected = id;
                else if (p->data[PACKET_ID] != client->id) netPlayerUpdate(e, p);
            } else {
                NetEntity n = {id, archetypePlayer(sprites[id])};
                array_push(netEntities, &n);
                printf("Player %u has connected\n", id);
            }
        } else if (p->data[PACKET_TYPE] == PACKET_TYPE_BMP) {
            uint8_t id = p->data[PACKET_ID];
            p++;
            bmp_t bitmap = bmp_new(32, 32, 4);
            memcpy(bitmap.pixels, p, 4096);
            sprites[id] = spriteCollectionSubmit(&bitmap);
            bmp_free(&bitmap);
            break;
        } else if (p->data[PACKET_TYPE] == PACKET_TYPE_SEED) {
            entire_world_package = true;
        } else if (!entire_world_package) {
            packetParse(p);
        } 
        //packetPrint(p);
        p++;
    }

    if (disconnected == 255) return;
    NetEntity* n = netEntities->data;
    for (unsigned int i = 0; i < netEntities->used; i++) {
        if (n->id == disconnected) {
            entity_destroy(n->entity);
            array_remove(netEntities, i);
            break;
        }
        n++;
    }
    printf("Player %u has disconnected\n", disconnected);
}

static void netSendPlayer(Packet* packet)
{
    Rect glRect = *(Rect*)entity_get(player, COMPONENT_GL_RECT);
    packet->data[PACKET_ID] = client->id;
    packet->data[PACKET_STATE] = NET_CONNECTED;
    packet->data[PACKET_TYPE] = PACKET_TYPE_USER;
    memcpy(&packet->data[PACKET_FLOAT_X], &glRect.x, sizeof(float));
    memcpy(&packet->data[PACKET_FLOAT_Y], &glRect.y, sizeof(float));
    packet->data[PACKET_SPRITE_ID] = (uint8_t)*(unsigned int*)entity_get(player, COMPONENT_SPRITE_ID);
    packet->data[PACKET_ORIENTATION] = (glRect.w >= 0.0f);

    if (stateJetpacking) packet->data[PACKET_OP] = PACKET_OP_JETPACKING;
    else if (stateWallSliding) packet->data[PACKET_OP] = PACKET_OP_WALLSLIDING;
    else if (stateDashing) packet->data[PACKET_OP] = PACKET_OP_DASHING;
    else packet->data[PACKET_OP] = PACKET_OP_NONE;
}

static void netSendWeapon(Packet* packet, Entity weapon)
{
    vec2 pos = *(vec2*)entity_get(weapon, COMPONENT_PHI_RECT);
    float* rot = (float*)entity_get(weapon, COMPONENT_ROTATION);
    GunType* g = (GunType*)entity_get(weapon, COMPONENT_GUN_CONTROLLER);
    uint16_t id = (uint16_t)weapon;
    
    if (g->state == GUN_STATE_USED) packet->data[PACKET_TYPE] = PACKET_TYPE_GUN_USED;
    else packet->data[PACKET_TYPE] = PACKET_TYPE_GUN_LOOSE;
    packet->data[PACKET_OP] = (uint8_t)g->ammo;
    memcpy(&packet->data[PACKET_ENTITY_ID], &id, sizeof(uint16_t));
    memcpy(&packet->data[PACKET_FLOAT_X], &pos.x, sizeof(float));
    memcpy(&packet->data[PACKET_FLOAT_Y], &pos.y, sizeof(float));
    memcpy(&packet->data[PACKET_FLOAT_Z], rot, sizeof(float));
}

static void netSendJetpack(Packet* packet, Entity jet)
{
    unsigned int j = *(unsigned int*)entity_get(jet, COMPONENT_JETPACK);
    unsigned int fuel = *(unsigned int*)entity_get(jet, COMPONENT_AMMO);
    vec2 pos = *(vec2*)entity_get(jet, COMPONENT_PHI_RECT);
    Rect* rGl = (Rect*)entity_get(player, COMPONENT_GL_RECT);
    uint16_t id = (uint16_t)jet;

    memcpy(&packet->data[PACKET_ENTITY_ID], &id, sizeof(uint16_t));
    if (j == JETPACK_USED) packet->data[PACKET_TYPE] = PACKET_TYPE_JETPACK_USED;
    else packet->data[PACKET_TYPE] = PACKET_TYPE_JETPACK_LOOSE;
    packet->data[PACKET_OP] = (uint8_t)fuel;
    memcpy(&packet->data[PACKET_FLOAT_X], &pos.x, sizeof(float));
    memcpy(&packet->data[PACKET_FLOAT_Y], &pos.y, sizeof(float));
    packet->data[PACKET_ORIENTATION] = (rGl->w >= 0.0f);
}

static void netSendGranadePick(Packet* packet, Entity e)
{
    vec2 pos = *(vec2*)entity_get(e, COMPONENT_PHI_RECT);
    uint16_t id = (uint16_t)e;

    memcpy(&packet->data[PACKET_ENTITY_ID], &id, sizeof(uint16_t));
    packet->data[PACKET_TYPE] = PACKET_TYPE_GRANADE_PICK;
    memcpy(&packet->data[PACKET_FLOAT_X], &pos.x, sizeof(float));
    memcpy(&packet->data[PACKET_FLOAT_Y], &pos.y, sizeof(float));
}

static void netSendGranadeThrow(Packet* packet, Entity e)
{
    vec2 pos = *(vec2*)entity_get(e, COMPONENT_PHI_RECT);
    float rot = vec2_to_rad(*(vec2*)entity_get(e, COMPONENT_VEL_VEC2));
    uint16_t id = (uint16_t)e;

    memcpy(&packet->data[PACKET_ENTITY_ID], &id, sizeof(uint16_t));
    packet->data[PACKET_TYPE] = PACKET_TYPE_GRANADE_THROW;
    memcpy(&packet->data[PACKET_FLOAT_X], &pos.x, sizeof(float));
    memcpy(&packet->data[PACKET_FLOAT_Y], &pos.y, sizeof(float));
    memcpy(&packet->data[PACKET_FLOAT_Z], &rot, sizeof(float));
}

static void netSendGranadeDrop(Packet* packet, Entity e)
{
    uint16_t id = (uint16_t)e;
    vec2 pos = *(vec2*)entity_get(e, COMPONENT_PHI_RECT);
    memcpy(&packet->data[PACKET_ENTITY_ID], &id, sizeof(uint16_t));
    packet->data[PACKET_TYPE] = PACKET_TYPE_GRANADE_DROP;
    memcpy(&packet->data[PACKET_FLOAT_X], &pos.x, sizeof(float));
    memcpy(&packet->data[PACKET_FLOAT_Y], &pos.y, sizeof(float));
}

static void netSendGranadeExp(Packet* packet, Entity e)
{
    uint16_t id = (uint16_t)e;
    memcpy(&packet->data[PACKET_ENTITY_ID], &id, sizeof(uint16_t));
    packet->data[PACKET_TYPE] = PACKET_TYPE_GRANADE_EXPLODE;
}

static void netSendFirebarrelExp(Packet* packet, Entity firebarrel)
{
    uint16_t id = (uint16_t)firebarrel;
    memcpy(&packet->data[PACKET_ENTITY_ID], &id, sizeof(uint16_t));
    packet->data[PACKET_TYPE] = PACKET_TYPE_FIREBARREL_EXPLODE;
}

static void netSend()
{   
    unsigned int size = 1;
    Packet* packet = (Packet*)client->buffer;
    netSendPlayer(packet);

    if (netUsedWeapon) {
        size++;
        packet++;
        netSendWeapon(packet, netUsedWeapon);
        //packetPrint(packet);
    }
    if (usedWeapon && usedWeapon != netUsedWeapon) {
        size++;
        packet++;
        netSendWeapon(packet, usedWeapon);
        //packetPrint(packet);
    }
    if (netJetpack) {
        size++;
        packet++;
        netSendJetpack(packet, netJetpack);
        //packetPrint(packet);
    }
    if (jetpack && netJetpack != jetpack) {
        size++;
        packet++;
        netSendJetpack(packet, jetpack);
        //packetPrint(packet);
    }

    if (netGranadePick) {
        size++;
        packet++;
        netSendGranadePick(packet, netGranadePick);
    }
    if (netGranadeThrow) {
        size++;
        packet++;
        netSendGranadeThrow(packet, netGranadeThrow);
    }
    if (netGranadeExp) {
        size++;
        packet++;
        netSendGranadeExp(packet, netGranadeExp);
    }
    while(!queue_is_empty(netGranadeDrop)) {
        size++;
        packet++;
        netSendGranadeDrop(packet, *(Entity*)queue_pop(netGranadeDrop));
    }

    if (netFirebarrelExp) {
        size++;
        packet++;
        netSendFirebarrelExp(packet, netFirebarrelExp);
    }

    netUsedWeapon = usedWeapon;
    netJetpack = jetpack;
    netGranadePick = 0;
    netGranadeThrow = 0;
    netGranadeExp = 0;
    netFirebarrelExp = 0;

    NNet_send(client->server, client->packet, client->buffer, size * sizeof(Packet), 0);
    received = 0;
}

void netSendBmp()
{
    Packet b = packetBmp(client->id, 1);
    Packet* p = client->buffer;
    bmp_t tmp = bmp_flip_vertical(&bmp);
    memcpy(p++, &b, sizeof(Packet));
    memcpy(p, tmp.pixels, 4096);
    bmp_free(&tmp);
    NNet_send(client->server, client->packet, client->buffer, 4096 + sizeof(Packet), 0);
    received = 0;
}

void netGameStep(float deltaTime)
{
    if (!client->connected) {
        printf("You were disconnected from %s\n", serverIp);
        treeNetExit();
        return;
    }

    netRead();
    gameStep(deltaTime);
    if (received) netSend();
}

void treeNetExit()
{
    if (client->connected) {
        if (player) {
            playerKill();
            while (!received) {
                received = NNetHost_read(client, 0);
            }
            netSend();
            while (!received) {
                received = NNetHost_read(client, 0);
            }
        }
        NNet_disconnect(client->host, client->server, &client->event, NET_TIMEOUT);
    }
    exitNanoNet(client->host);
    NNetHost_free(client);

    memset(serverIp, 0, 16);
    memset(clientUsername, 0, 256);
    array_destroy(netEntities);
    netEntities = NULL;
    client = NULL;
    received = 0;

    levelReset();
    systemSetState(STATE_MENU);
}

static void map_world_net() 
{
    module_destroy(module_current());
    moduleInit();
    map_t map = map_generate(30, 20, 5, 40, 2, 30);
    if (!map.data) {
        systemSetState(STATE_MENU);
        return;
    }
    module_from_map(&map);
    meshDestroy(&mesh);
    mesh = meshFromMap(map);
    spawnPoint = map_spawn(map);
    while (spawnPoint.x < 40.0f && spawnPoint.y < 40.0f) {
        spawnPoint = map_spawn(map);
    }
    map_destroy(map);
    //terrainRecalcTextures();
}

static void netReadBmp()
{
    uint8_t buff[(4096 + sizeof(Packet)) * NET_MAX_CLIENT_COUNT];
    while (enet_host_service(client->host, &client->event, NET_TIMEOUT) > 0) {
        if (client->event.type == ENET_EVENT_TYPE_RECEIVE) {
            received = NNet_read(client->event.packet, buff);
            break;
        }
        else continue;
    }

    unsigned int size = received / (4096 + sizeof(Packet));
    printf("Number of external net bitmaps: %u\n", size);
    for (unsigned int i = 0; i < size; i++) {
        Packet* p = (Packet*)&buff[i * (4096 + sizeof(Packet))];
        uint8_t id = p->data[PACKET_ID];
        p++;

        bmp_t bitmap = bmp_new(32, 32, 4);
        memcpy(bitmap.pixels, p, 4096);
        sprites[id] = spriteCollectionSubmit(&bitmap);
        bmp_free(&bitmap);
    }
}

void treeNetInit(const char* username, const char* ip)
{
    strcpy(clientUsername, username);
    strcpy(serverIp, ip);
    received = 0;
    player = 0;
    for (int i = 0; i < NET_MAX_CLIENT_COUNT; i++) {
        sprites[i] = currentPlayerSprite;
    }

    netGranadeDrop = queue_new(GRANADE_MAX, sizeof(Entity));
    netUsedWeapon = netJetpack = netGranadeThrow = netGranadePick = netGranadeExp = netFirebarrelExp = 0;
    netEntities = array_new(NET_MAX_CLIENT_COUNT, sizeof(NetEntity));
    client = NNetHost_create(serverIp, NET_PORT, NET_MAX_CLIENT_COUNT, NET_CHANNELS, NET_BUFFER_SIZE, NET_TIMEOUT);
    netReadBmp();
    netSendBmp();
initiation:
    while (enet_host_service(client->host, &client->event, NET_TIMEOUT) > 0) {
        if (client->event.type == ENET_EVENT_TYPE_RECEIVE) {
            unsigned int size = NNet_read(client->event.packet, client->buffer) / sizeof(Packet);
            Packet* p = (Packet*)client->buffer;
            if (p->data[PACKET_TYPE] == PACKET_TYPE_BMP) goto initiation;
            for (unsigned int i = 0; i < size; i++) {
                if (p->data[PACKET_TYPE] == PACKET_TYPE_USER) {
                    Entity e = archetypePlayer(sprites[p->data[PACKET_ID]]);
                    Rect* r = (Rect*)entity_get(e, COMPONENT_PHI_RECT);
                    r->x = spawnPoint.x;
                    r->y = spawnPoint.y;

                    NetEntity n = netEntityCreate(p->data[PACKET_ID], e);
                    array_push(netEntities, &n);
                    if (p->data[PACKET_STATE] == NET_CONNECTING) {
                        client->id = p->data[PACKET_ID];
                        player = n.entity;
                    }
                } else if (p->data[PACKET_TYPE] == PACKET_TYPE_SEED) {
                    unsigned int seed = *(unsigned int*)&p->data[PACKET_SEED];
                    randSeed = seed;
                    rand_seed(randSeed);
                    map_world_net();
                    printf("Random Net Seed is : %u\n", randSeed);
                } else {
                    packetParse(p);
                }
                p++;
            }
            received++;
            break;
        }
    }

    if (!client->connected) {
        printf("There was a problem connecting to %s\n", serverIp);
        treeNetExit();
        return;
    } 
    
    printf("You are connected to %s\n", serverIp);
    printf("Player ID: %u\n", client->id);
    systemSetState(STATE_NET_PLAY);
}