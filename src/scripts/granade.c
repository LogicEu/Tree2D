#include "../Tree.h"
#include <math.h>
#include <string.h>

extern vec2 mouse;
extern Entity netGranadeExp;

void granadeCollect(Entity granade)
{
    GranadeComponent* granadeComponent = (GranadeComponent*)entity_get(granade, COMPONENT_GRANADE);
    granadeComponent->state = GRANADE_COLLECTED;
    memset(entity_get(granade, COMPONENT_GRAVITY), 0, sizeof(bool));
    memset(entity_get(granade, COMPONENT_GL_RECT), 0, sizeof(Rect));
}

void granadeDrop(Entity granade, vec2 pos)
{
    GranadeComponent* granadeComponent = (GranadeComponent*)entity_get(granade, COMPONENT_GRANADE);
    Rect* r = entity_get(granade, COMPONENT_PHI_RECT);
    memcpy(r, &pos, sizeof(vec2));
    granadeComponent->state = GRANADE_LOOSE;
    *(bool*)entity_get(granade, COMPONENT_GRAVITY) = true;
    entity_set(granade, COMPONENT_GL_RECT, r);
}

void granadeThrow(Entity granade, vec2 position, float rot)
{
    GranadeComponent* granadeComponent = (GranadeComponent*)entity_get(granade, COMPONENT_GRANADE);
    granadeComponent->state = GRANADE_THROWED;
    static const int granadeSpeed = 200.0f;
    vec2 vel = {cosf(rot) * granadeSpeed, sinf(rot) * granadeSpeed};
    entity_set(granade, COMPONENT_VEL_VEC2, &vel);
    *(bool*)entity_get(granade, COMPONENT_GRAVITY) = true;
    Rect* rPhi = (Rect*)entity_get(granade, COMPONENT_PHI_RECT);
    rPhi->x = position.x;
    rPhi->y = position.y;
    entity_set(granade, COMPONENT_GL_RECT, entity_get(granade, COMPONENT_PHI_RECT));
}

static void granadeApply(Entity granade, float deltaTime)
{
    static const float granadeTick = 0.5f;

    GranadeComponent* gc = (GranadeComponent*)entity_get(granade, COMPONENT_GRANADE);
    if (gc->state != GRANADE_THROWED) return;

    gc->timer -= deltaTime * granadeTick;

    vec2* vel = (vec2*)entity_get(granade, COMPONENT_VEL_VEC2); 
    if (checkRigidCollision(granade, vec2_new(vel->x * deltaTime, 0.0f))) vel->x = -vel->x * 0.5f;
    if (checkRigidCollision(granade, vec2_new(0.0f, vel->y * deltaTime))) vel->y = -vel->y * 0.5f;
}

void granadeStep(float deltaTime)
{
    unsigned int count = component_entity_count(COMPONENT_GRANADE);
    for (unsigned int i = 0; i < count; i++) {
        Entity e = entity_find(COMPONENT_GRANADE, i);
        if (((GranadeComponent*)entity_get(e, COMPONENT_GRANADE))->timer < 0.0f) {
            vec2* v = (vec2*)entity_get(e, COMPONENT_PHI_RECT);
            archetypeBlast(*v, 80.0f);
            entity_destroy(e);
            netGranadeExp = e;
            i--;
            count--;
            continue;
        }
        granadeApply(e, deltaTime);
    }
}
