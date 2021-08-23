#include "../Tree.h"
#include <string.h>

static const float bulletSpeed = 225.0f;

static GunType Gun = {GUN_STATE_LOOSE, GUN_KIND_GUN, 14, 7.0f, 0.0f, 4.0f};
static GunType ShotGun = {GUN_STATE_LOOSE, GUN_KIND_SHOTGUN, 8, 10.0f, 0.0f, 2.0f};
static GunType RifleGun = {GUN_STATE_LOOSE, GUN_KIND_RIFLE, 28, 10.0f, 0.0f, 10.f};
static GunType MachineGun = {GUN_STATE_LOOSE, GUN_KIND_MACHINEGUN, 48, 10.0f, 0.0f, 25.f};
static GunType FlameGun = {GUN_STATE_LOOSE, GUN_KIND_FLAMETHROWER, 45, 10.0f, 0.0f, 15.0f};
static GunType BazookaGun = {GUN_STATE_LOOSE, GUN_KIND_BAZOOKA, 4, 10.0f, 0.0f, 1.0f};

extern vec2 mouse;
extern Entity player;

static texture_t* getTexture(GunKind gunKind)
{
    switch(gunKind) {
        case GUN_KIND_GUN:
            return assetsGetTexture(TEXTURE_GUN);
        case GUN_KIND_SHOTGUN:
            return assetsGetTexture(TEXTURE_SHOTGUN);
        case GUN_KIND_RIFLE:
            return assetsGetTexture(TEXTURE_RIFLE);
        case GUN_KIND_MACHINEGUN:
            return assetsGetTexture(TEXTURE_MACHINEGUN);
        case GUN_KIND_FLAMETHROWER:
            return assetsGetTexture(TEXTURE_FLAMETHROWER);
        case GUN_KIND_BAZOOKA:
            return assetsGetTexture(TEXTURE_BAZOOKA);
        break;
    }
    return NULL;
}

static Entity gunCreateType(vec2 position, GunType* gun)
{
    texture_t* t = getTexture(gun->kind);
    Rect r = {position.x, position.y, t->width, t->height};
    float rot = 0.0f;
    vec2 v = {0.0f, 0.0f};
    bool g = true;
    return archetype_entity(ARCHETYPE_GUN, 7, &t->id, &r, &r, &v, &g, gun, &rot);
}

static void gunFireGun(vec2 gun, vec2 theta, float off) 
{
    vec2 pos = {gun.x + theta.x * off, gun.y + theta.y * off};
    vec2 vel = {bulletSpeed * theta.x, bulletSpeed * theta.y};
    archetypeBullet(pos, vel, BULLET_KIND_BALL);
    archetypeSmoke(pos, TEXTURE_SMOKE2);
}

static void gunFireBazooka(vec2 gun, vec2 theta, float off) 
{
    vec2 pos = {gun.x + theta.x * off, gun.y + theta.y * off};
    vec2 vel = {bulletSpeed * theta.x, bulletSpeed * theta.y};
    archetypeBullet(pos, vel, BULLET_KIND_ROCKET);
    archetypeSmoke(pos, TEXTURE_SMOKE);
}

static void gunFireFlamethrower(vec2 gun, vec2 theta, float off) 
{
    static float fireSpeed = 600.0f;
    vec2 pos = {gun.x + theta.x * off, gun.y + theta.y * off};
    vec2 vel = {fireSpeed * theta.x, fireSpeed * theta.y};
    archetypeFire(pos, vel);
}

static void gunFireShotgun(vec2 gun, float rot, float off) 
{
    vec2 pos = {gun.x + cosf(rot) * off, gun.y + sinf(rot) * off};
    float range = -0.075f;
    for (int i = 0; i < 4; i++) {
        vec2 vel = {bulletSpeed * cosf(rot + range), bulletSpeed * sinf(rot + range)};
        archetypeBullet(pos, vel, BULLET_KIND_BALL);
        range += 0.05f;
    }
    archetypeSmoke(pos, TEXTURE_SMOKE2);
}

static void gunFireType(vec2 gun, vec2 theta, float rot, GunKind kind)
{
    switch(kind) {
        case GUN_KIND_GUN:
            gunFireGun(gun, theta, 12.0f);
            break;
        case GUN_KIND_SHOTGUN:
            gunFireShotgun(gun, rot, 22.0f);
            break;
        case GUN_KIND_RIFLE:
            gunFireGun(gun, theta, 22.0f);
            break;
        case GUN_KIND_MACHINEGUN:
            gunFireGun(gun, theta, 22.0f);
            break;
        case GUN_KIND_FLAMETHROWER:
            gunFireFlamethrower(gun, theta, 22.0f);
            break;
        case GUN_KIND_BAZOOKA:
            gunFireBazooka(gun, theta, 12.0f);
            break;
        break;
    }
}

static void gunControllerApply(Entity e, float deltaTime)
{
    Rect* playerPhi = (Rect*)entity_get(player, COMPONENT_PHI_RECT);
    Rect*  rPhi = (Rect*)entity_get(e, COMPONENT_PHI_RECT);
    Rect* rTex = (Rect*)entity_get(e, COMPONENT_GL_RECT);
    float* rot = (float*)entity_get(e, COMPONENT_ROTATION);
    
    GunType* gc = (GunType*)entity_get(e, COMPONENT_GUN_CONTROLLER);
    vec2 pos = {playerPhi->x, playerPhi->y};
    float rads = vec2_to_rad(vec2_sub(mouse, pos));
    vec2 theta = {cosf(rads), sinf(rads)};

    if (fabs(rads) > M_PI * 0.5) {
        if (rTex->h > 0.0f) rTex->h *= -1.0f;
    } else if (rTex->h <= 0.0f) rTex->h *= -1.0f;

    rPhi->x = pos.x + theta.x * gc->offset;      
    rPhi->y = pos.y + theta.y * gc->offset;
    rTex->x = rPhi->x;
    rTex->y = rPhi->y; 
    *rot = rads;

    if (gc->latencyTimer > 0.0f) gc->latencyTimer -= gc->latencySpeed * deltaTime;
    else if (gc->ammo && glee_mouse_down(GLFW_MOUSE_BUTTON_LEFT)) {
        gunFireType(pos, theta, rads, gc->kind);
        gc->ammo--;
        gc->latencyTimer = 1.0f;
    }
}

void gunShoot(Entity gun)
{
    vec2 pos = *(vec2*)entity_get(gun, COMPONENT_PHI_RECT);
    GunType* g = (GunType*)entity_get(gun, COMPONENT_GUN_CONTROLLER);
    float rot = *(float*)entity_get(gun, COMPONENT_ROTATION);
    gunFireType(pos, vec2_new(cosf(rot), sinf(rot)), rot, g->kind);
}

Entity gunCreate(vec2 position, GunKind gunKind)
{
    switch(gunKind) {
        case GUN_KIND_GUN:
            return gunCreateType(position, &Gun);
        case GUN_KIND_SHOTGUN:
            return gunCreateType(position, &ShotGun);
        case GUN_KIND_RIFLE:
            return gunCreateType(position, &RifleGun);
        case GUN_KIND_MACHINEGUN:
            return gunCreateType(position, &MachineGun);
        case GUN_KIND_FLAMETHROWER:
            return gunCreateType(position, &FlameGun);
        case GUN_KIND_BAZOOKA:
            return gunCreateType(position, &BazookaGun);
        break;
    }
    return 0;
}

void gunPick(Entity gun)
{
    ((GunType*)entity_get(gun, COMPONENT_GUN_CONTROLLER))->state = GUN_STATE_USED;
    memset(entity_get(gun, COMPONENT_VEL_VEC2), 0, sizeof(vec2));
    memset(entity_get(gun, COMPONENT_GRAVITY), 0, sizeof(bool));
}

void gunDrop(Entity gun)
{
    GunType* gc = (GunType*)entity_get(gun, COMPONENT_GUN_CONTROLLER);
    gc->state = GUN_STATE_LOOSE;
    gc->latencyTimer = 0.0f;
    *(bool*)entity_get(gun, COMPONENT_GRAVITY) = true;
    memset(entity_get(gun, COMPONENT_VEL_VEC2), 0, sizeof(vec2));
}

void gunControllerStep(float deltaTime)
{
    unsigned int count = component_entity_count(COMPONENT_GUN_CONTROLLER);
    for (unsigned int i = 0; i < count; i++) {
        Entity e = entity_find(COMPONENT_GUN_CONTROLLER, i);
        GunState state = ((GunType*)entity_get(e, COMPONENT_GUN_CONTROLLER))->state;
        if (state == GUN_STATE_USED) gunControllerApply(e, deltaTime);
    }
}
