#include "Tree.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

map_t map_create(unsigned int w, unsigned int h)
{
    map_t m;
    m.width = w;
    m.height = h;
    m.data = (uint8_t*)malloc(w * h);
    memset(m.data, 0, w * h);
    return m;
}

void map_destroy(map_t m)
{
    free(m.data);
}

uint8_t* map_tile(map_t m, unsigned int x, unsigned int y)
{
    return m.data + (y * m.width) + x;
}

map_t map_copy(map_t* m)
{
    map_t ret;
    ret.width = m->width;
    ret.height = m->height;
    ret.data = (uint8_t*)malloc(m->width * m->height);
    memcpy(ret.data, m->data, m->width * m->height);
    return ret;
}

//Probability from 0 to 100 of block creation
map_t map_noise(unsigned int w, unsigned int h, unsigned int stat, unsigned int block)
{
    map_t m = map_create(w, h);
    for (unsigned int y = 0; y < m.height; y++) {
        for (unsigned int x = 0; x < m.width; x++) {
            if (x == 0 || y == 0 || x == m.width - 1 || y == m.height - 1) {
                *map_tile(m, x, y) = MAP_TILE_STATIC;
            } else if ((unsigned int)(rand_next() % 100) < stat) {
                *map_tile(m, x, y) = MAP_TILE_STATIC;
            } 
            else if ((unsigned int)(rand_next() % 100) < block) {
                *map_tile(m, x, y) = MAP_TILE_BLOCK;
            }
        }
    }
    return m;
}

#define IS_NULL(m, x, y) *map_tile(m, x, y) == MAP_TILE_NULL
#define IS_STATIC(m, x, y) *map_tile(m, x, y) == MAP_TILE_STATIC
#define IS_BLOCK(m, x, y) *map_tile(m, x, y) > 0 && *map_tile(m, x, y) < MAP_TILE_GUN

void map_cellular_step(map_t* m)
{
    map_t copy = map_copy(m);
    for (unsigned int y = 0; y < m->height; y++) {
        for (unsigned int x = 0; x < m->width; x++) {
            if (IS_STATIC(copy, x, y)) continue;

            unsigned int count = 0;
            if (IS_BLOCK(copy, x + 1, y)) count++;
            if (IS_BLOCK(copy, x + 1, y + 1)) count++;
            if (IS_BLOCK(copy, x, y + 1)) count++;
            if (IS_BLOCK(copy, x - 1, y + 1)) count++;
            if (IS_BLOCK(copy, x - 1, y)) count++;
            if (IS_BLOCK(copy, x - 1, y - 1)) count++;
            if (IS_BLOCK(copy, x, y - 1)) count++;
            if (IS_BLOCK(copy, x + 1, y - 1)) count++;

            if (count > 4) *map_tile(*m, x, y) = MAP_TILE_BLOCK;
            else if (count < 4) *map_tile(*m, x, y) = MAP_TILE_NULL;
        }
    }
    map_destroy(copy);
}

map_t map_cellular_generate(unsigned int width, unsigned int height, unsigned int stat, unsigned int block, unsigned int steps)
{
    map_t m = map_noise(width, height, stat, block);
    for (unsigned int i = 0; i < steps; i++) {
        map_cellular_step(&m);
    }
    return m;
}

uint8_t item_rand()
{
    unsigned int num = rand_next() % 100;
    if (num < 5) return MAP_TILE_JETPACK;
    if (num < 25) return MAP_TILE_FIREBARREL;
    if (num < 40) return MAP_TILE_GUN;
    if (num < 55) return MAP_TILE_SHOTGUN;
    if (num < 70) return MAP_TILE_GRANADE;
    if (num < 80) return MAP_TILE_RIFLE;
    if (num < 85) return MAP_TILE_MACHINEGUN;
    if (num < 90) return MAP_TILE_FLAMETHROWER;
    if (num < 95) return MAP_TILE_BAZOOKA;
    return MAP_TILE_NULL;
}

void map_itemify(map_t* m, unsigned int prob)
{
    for (unsigned int y = 0; y < m->height; y++) {
        for (unsigned int x = 0; x < m->width; x++) {
            if (IS_BLOCK(*m, x, y)) continue;
            if (y > 0 && IS_BLOCK(*m, x, y - 1) && IS_NULL(*m, x, y)) {
                if ((unsigned int)(rand_next() % 100) < prob) *map_tile(*m, x, y) = item_rand();
            }
        }
    }
}

void map_negative(map_t* map)
{
    for (unsigned int y = 0; y < map->height; y++) {
        for (unsigned int x = 0; x < map->width; x++) {
            uint8_t* t = map_tile(*map, x, y);
            
            if (*t == MAP_TILE_NULL) {
                *t = MAP_TILE_BLOCK;
            }
            else if (*t == MAP_TILE_BLOCK || *t == MAP_TILE_STATIC) {
                *t = MAP_TILE_NULL;
            }
        }
    }
}

map_t map_generate(unsigned int w, unsigned int h, unsigned int stat, unsigned int block, unsigned int steps, unsigned int item)
{
    map_t map = map_cellular_generate(w, h, stat, block, steps);
    map_negative(&map);
    map_itemify(&map, item);
    return map;
}

Entity item_to_entity(uint8_t item, unsigned int xPos, unsigned int yPos)
{
    vec2 pos = {(float)(xPos * 32) + 16.0f, (float)(yPos * 32) + 16.0f};
    switch (item) {
        case MAP_TILE_BLOCK: return archetypeTerrainTile(TEXTURE_TILE, pos);
        case MAP_TILE_STATIC: return archetypeTerrainTile(TEXTURE_TILE, pos);
        case MAP_TILE_GUN: return gunCreate(pos, GUN_KIND_GUN);
        case MAP_TILE_SHOTGUN: return gunCreate(pos, GUN_KIND_SHOTGUN);
        case MAP_TILE_RIFLE: return gunCreate(pos, GUN_KIND_RIFLE);
        case MAP_TILE_MACHINEGUN: return gunCreate(pos, GUN_KIND_MACHINEGUN);
        case MAP_TILE_FLAMETHROWER: return gunCreate(pos, GUN_KIND_FLAMETHROWER);
        case MAP_TILE_BAZOOKA: return gunCreate(pos, GUN_KIND_BAZOOKA);
        case MAP_TILE_GRANADE: return archetypeGranade(pos);
        case MAP_TILE_FIREBARREL: return archetypeFirebarrel(pos);
        case MAP_TILE_JETPACK: return archetypeJetpackController(pos);
    }
    return 0;
}

uint8_t entity_to_item(Entity e)
{
    if (entity_has(e, COMPONENT_RIGID_COLLIDER)) return MAP_TILE_BLOCK;
    else if (entity_has(e, COMPONENT_FIREBARREL)) return MAP_TILE_FIREBARREL;
    else if (entity_has(e, COMPONENT_GUN_CONTROLLER)) {
        GunType* g = (GunType*)entity_get(e, COMPONENT_GUN_CONTROLLER);
        switch (g->kind) {
            case GUN_KIND_GUN: return MAP_TILE_GUN;
            case GUN_KIND_SHOTGUN: return MAP_TILE_SHOTGUN;
            case GUN_KIND_RIFLE: return MAP_TILE_RIFLE;
            case GUN_KIND_MACHINEGUN: return MAP_TILE_MACHINEGUN;
            case GUN_KIND_FLAMETHROWER: return MAP_TILE_FLAMETHROWER;
            case GUN_KIND_BAZOOKA: return MAP_TILE_BAZOOKA;
        }
    }
    else if (entity_has(e, COMPONENT_GRANADE)) return MAP_TILE_GRANADE;
    else if (entity_has(e, COMPONENT_JETPACK)) return MAP_TILE_JETPACK;
    return MAP_TILE_NULL;
}

void map_save(const char* path, map_t map)
{
    FILE* file = fopen(path, "wb");
    if (!file) {
        printf("Could not open file '%s' for writing.\n", path);
        return;
    }

    fwrite(&map.width, sizeof(unsigned int), 2, file);
    fwrite(map.data, 1, map.width * map.height, file);
    fclose(file);
    printf("Succesfully saved file '%s'\n", path);
}

map_t map_load(const char* path)
{
    map_t ret;
    ret.width = ret.height = 0;
    ret.data = NULL;

    FILE* file = fopen(path, "rb");
    if (!file) {
        printf("Could not open file '%s' for reading\n", path);
        return ret;
    }

    fread(&ret.width, sizeof(unsigned int), 2, file);
    ret.data = (uint8_t*)malloc(ret.width * ret.height);
    fread(ret.data, 1, ret.width * ret.height, file);
    fclose(file);
    printf("Succesfully loaded file '%s'\n", path);
    return ret;
}

void module_from_map(map_t* map)
{
    for (unsigned int y = 0; y < map->height; y++) {
        for (unsigned int x = 0; x < map->width; x++) {
            uint8_t t = *map_tile(*map, x, y);
            if (t) item_to_entity(t, x, y);
        }
    }
}

map_t map_from_module()
{
    float min_x, min_y, max_x, max_y;
    min_x = min_y = 100000.0f;
    max_x = max_y = -100000.0f;

    unsigned int ecount = component_entity_count(COMPONENT_PHI_RECT);
    for (unsigned int i = 0; i < ecount; i++) {
        Entity e = entity_find(COMPONENT_PHI_RECT, i);
        Rect* r = (Rect*)entity_get(e, COMPONENT_PHI_RECT);
        if (min_x > r->x) min_x = r->x;
        if (max_x < r->x) max_x = r->x;
        if (min_y > r->y) min_y = r->y;
        if (max_y < r->y) max_y = r->y;
    }

    unsigned int x = (unsigned int)((max_x - min_x) / 32.0f) + 1;
    unsigned int y = (unsigned int)((max_y - min_y) / 32.0f) + 1;
    map_t m = map_create(x, y);
    for (unsigned int i = 0; i < ecount; i++) {
        Entity e = entity_find(COMPONENT_PHI_RECT, i);
        Rect* r = (Rect*)entity_get(e, COMPONENT_PHI_RECT);
        x = (unsigned int)clampf((r->x + 16.0f - min_x) / 32.0f, 0.0f, (float)m.width);
        y = (unsigned int)clampf((r->y + 16.0f - min_y) / 32.0f, 0.0f, (float)m.height);
        *map_tile(m, x, y) = entity_to_item(e);
    }
    return m;
}

vec2 map_spawn(map_t map)
{
    vec2 pos = {32.0f, 32.0f};
    for (unsigned int y = 0; y < map.height; y++) {
        for (unsigned int x = 0; x < map.width; x++) {
            if (!(IS_NULL(map, x, y))) continue;
            if (y > 0 && IS_BLOCK(map, x, y - 1) && rand_next() % 100 < 5) {
                pos = vec2_new((float)(x * 32) + 16.0f, (float)(y * 32) + 16.0f);
                return pos;
            } 
        }
    }   
    return pos;
}
