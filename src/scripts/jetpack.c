#include "../Tree.h"
#include <string.h>

extern Entity player;

static void jetpackApply(Entity e)
{
    Rect* playerGl = (Rect*)entity_get(player, COMPONENT_GL_RECT);
    Rect* rPhi = (Rect*)entity_get(e, COMPONENT_PHI_RECT);
    Rect* rTex = (Rect*)entity_get(e, COMPONENT_GL_RECT);

    if ((playerGl->w > 0.0f && rTex->w > 0.0f) ||
        (playerGl->w < 0.0f && rTex->w < 0.0f)) {
        rTex->w *= -1.0f;
    } 

    rPhi->x = playerGl->x + rTex->w * 0.3f;      
    rPhi->y = playerGl->y - 4.0f;
    rTex->x = rPhi->x;
    rTex->y = rPhi->y; 
    rTex->w *= -1.0f;
}

void jetpackUse(Entity jetpack)
{
    if (*(unsigned int*)entity_get(jetpack, COMPONENT_JETPACK) != JETPACK_USED) return;
    unsigned int* fuel = (unsigned int*)entity_get(jetpack, COMPONENT_AMMO);
    if (!*fuel) return;
    vec2* playerVel = (vec2*)entity_get(player, COMPONENT_VEL_VEC2);
    vec2 pos = *(vec2*)entity_get(jetpack, COMPONENT_PHI_RECT);
    smokeEmit(pos, TEXTURE_SMOKE2);
    playerVel->y += 8.0f;
    *fuel -= 1;
}

void jetpackPick(Entity jetpack)
{
    memset(entity_get(jetpack, COMPONENT_VEL_VEC2), 0, sizeof(vec2));
    memset(entity_get(jetpack, COMPONENT_GRAVITY), 0, sizeof(bool));
    *(unsigned int*)entity_get(jetpack, COMPONENT_JETPACK) = JETPACK_USED;
}

void jetpackDrop(Entity jetpack)
{
    memset(entity_get(jetpack, COMPONENT_VEL_VEC2), 0, sizeof(vec2));
    *(unsigned int*)entity_get(jetpack, COMPONENT_JETPACK) = JETPACK_LOOSE;
    *(bool*)entity_get(jetpack, COMPONENT_GRAVITY) = true;
}

void jetpackControllerStep()
{
    unsigned int count = component_entity_count(COMPONENT_JETPACK);
    for (unsigned int i = 0; i < count; i++) {
        Entity e = entity_find(COMPONENT_JETPACK, i);
        unsigned int activated = *(unsigned int*)entity_get(e, COMPONENT_JETPACK);
        if (activated == JETPACK_USED) jetpackApply(e);
    }
}
