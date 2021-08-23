#include "Tree.h"

static const vec2 vecZero = {0.0f, 0.0f};
static const bool bTrue = true, bFalse = false;
static const float fOne = 1.0f;

extern vec2 spawnPoint;
extern unsigned int currentPlayerSprite;

Entity archetypePlayer(unsigned int sprite)
{
    static unsigned int k = SPRITE_IDLE;
    Rect hitbox = {spawnPoint.x, spawnPoint.y, 12.0f, 24.0f};

    sprite_t* s = assetsGetSprite(currentPlayerSprite ,k);
    Rect r = {spawnPoint.x, spawnPoint.y, s->textures->width, s->textures->height};
    return archetype_entity(ARCHETYPE_PLAYER, 6, &k, &sprite, &r, &hitbox, &vecZero, &bTrue);
}

Entity archetypeTerrainTile(unsigned int texture_index, vec2 position)
{
    static bool rc = true;

    texture_t* t = assetsGetTexture(texture_index);
    Rect r = {position.x, position.y, 32.0f, 32.0f};
    return archetype_entity(ARCHETYPE_TERRAIN, 4, &t->id, &r, &r, &rc);
}

Entity archetypeBullet(vec2 position, vec2 vel, BulletKind kind)
{
    texture_t* t;
    if (kind == BULLET_KIND_ROCKET) t = assetsGetTexture(TEXTURE_ROCKET);
    else t = assetsGetTexture(TEXTURE_REDBALL);
    Rect r = {position.x, position.y, t->width, t->height};
    float f = vec2_to_rad(vel);
    return archetype_entity(ARCHETYPE_BULLET, 6, &t->id, &r, &r, &vel, &kind, &f);
}

Entity archetypeBlast(vec2 position, float explotionForce)
{   
    texture_t* t = assetsGetTexture(TEXTURE_EXPLOTION);
    Rect r = {position.x, position.y, t->width, t->height};
    float rot = (float)(rand() & 66 / 10);
    return archetype_entity(ARCHETYPE_BLAST, 8, &t->id, &r, &r, &vecZero, &fOne, &fOne, &rot, &explotionForce);
}

Entity archetypeFire(vec2 position, vec2 vel)
{   
    static float force = 20.0f;

    texture_t* t = assetsGetTexture(TEXTURE_FIRE);
    Rect r = {position.x, position.y, t->width, t->height};
    float rot = vec2_to_rad(vel);
    return archetype_entity(ARCHETYPE_BLAST, 8, &t->id, &r, &r, &vel, &fOne, &fOne, &rot, &force);
}

Entity archetypeSmoke(vec2 position, unsigned int textureIndex)
{
    static float s = 0.5f;

    texture_t* t = assetsGetTexture(textureIndex);
    Rect r = {position.x, position.y, t->width, t->height};
    float rot = (float)(rand() & 66 / 10);
    return archetype_entity(ARCHETYPE_SMOKE, 6, &t->id, &r, &fOne, &s, &rot, &bTrue);
}

Entity archetypeShadow(vec2 position, unsigned int sprite, float side)
{
    static float a = 0.6f;

    texture_t* t = assetsGetSprite(sprite, SPRITE_JUMPING)->textures;
    Rect r = {position.x, position.y, t->width * (-1.0f + 2.0f * (float)(side > 0.0f)), t->height};
    return archetype_entity(ARCHETYPE_SHADOW, 4, &t->id, &r, &a, &bTrue);
}

Entity archetypeFirebarrel(vec2 position)
{
    texture_t* t = assetsGetTexture(TEXTURE_FIREBARREL);
    Rect r = {position.x, position.y, t->width, t->height};
    return archetype_entity(ARCHETYPE_FIREBARREL, 6, &t->id, &r, &r, &vecZero, &bTrue, &bFalse);
}

Entity archetypeJetpackController(vec2 position)
{
    static unsigned int fuel = 250;
    static unsigned int jetpack = JETPACK_LOOSE;
    texture_t* t = assetsGetTexture(TEXTURE_JETPACK);
    Rect r = {position.x, position.y, t->width, t->height};
    return archetype_entity(ARCHETYPE_JETPACK, 7, &t->id, &r, &r, &vecZero, &bTrue, &jetpack, &fuel);
}

Entity archetypeGranade(vec2 position)
{
    static GranadeComponent gc = {GRANADE_LOOSE, 1.0f};

    texture_t* t = assetsGetTexture(TEXTURE_GRANADE);
    Rect r = {position.x, position.y, t->width, t->height};
    return archetype_entity(ARCHETYPE_GRANADE, 6, &t->id, &r, &r, &vecZero, &bTrue, &gc);
}

Entity archetypeBox(vec2 position)
{
    static BoxState state = BOX_STATE_LOOSE;
    texture_t* t = assetsGetTexture(TEXTURE_BOX);
    Rect r = {position.x, position.y, t->width, t->height};
    return archetype_entity(ARCHETYPE_BOX, 6, &t->id, &r, &r, &vecZero, &bTrue, &state);
}

void archetypesInit()
{
    // Player
    archetype_create(6, COMPONENT_SPRITE_ID, COMPONENT_SPRITE_COLLECTION, COMPONENT_GL_RECT, COMPONENT_PHI_RECT, COMPONENT_VEL_VEC2, COMPONENT_GRAVITY);
    // Tile
    archetype_create(4, COMPONENT_TEX_ID, COMPONENT_GL_RECT, COMPONENT_PHI_RECT, COMPONENT_RIGID_COLLIDER);
    // Gun
    archetype_create(8, COMPONENT_TEX_ID, COMPONENT_GL_RECT, COMPONENT_PHI_RECT, COMPONENT_VEL_VEC2, COMPONENT_GRAVITY, COMPONENT_GUN_CONTROLLER, COMPONENT_ROTATION);
    // Bullet
    archetype_create(6, COMPONENT_TEX_ID, COMPONENT_GL_RECT, COMPONENT_PHI_RECT, COMPONENT_VEL_VEC2, COMPONENT_BULLET, COMPONENT_ROTATION);
    // Blast 
    archetype_create(8, COMPONENT_TEX_ID, COMPONENT_GL_RECT, COMPONENT_PHI_RECT, COMPONENT_VEL_VEC2, COMPONENT_ALPHA, COMPONENT_SCALE, COMPONENT_ROTATION, COMPONENT_EXPLOTION);
    // Smoke
    archetype_create(6, COMPONENT_TEX_ID, COMPONENT_GL_RECT, COMPONENT_ALPHA, COMPONENT_SCALE, COMPONENT_ROTATION, COMPONENT_SMOKE);
    // Shadow
    archetype_create(4, COMPONENT_TEX_ID, COMPONENT_GL_RECT, COMPONENT_ALPHA, COMPONENT_SHADOW);
    // Firebarrel
    archetype_create(6, COMPONENT_TEX_ID, COMPONENT_GL_RECT, COMPONENT_PHI_RECT, COMPONENT_VEL_VEC2, COMPONENT_GRAVITY, COMPONENT_FIREBARREL);
    // Jetpack+
    archetype_create(7, COMPONENT_TEX_ID, COMPONENT_GL_RECT, COMPONENT_PHI_RECT, COMPONENT_VEL_VEC2, COMPONENT_GRAVITY, COMPONENT_JETPACK, COMPONENT_AMMO);
    // Granade
    archetype_create(6, COMPONENT_TEX_ID, COMPONENT_GL_RECT, COMPONENT_PHI_RECT, COMPONENT_VEL_VEC2, COMPONENT_GRAVITY, COMPONENT_GRANADE);
    // Box
    archetype_create(6, COMPONENT_TEX_ID, COMPONENT_GL_RECT, COMPONENT_PHI_RECT, COMPONENT_VEL_VEC2, COMPONENT_GRAVITY, COMPONENT_BOX);
}