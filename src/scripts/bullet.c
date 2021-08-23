#include "../Tree.h"

static float trailTimer = 0.0f;

void bulletStep(float deltaTime)
{
    static const vec2 vecZero = {0.0f, 0.0f};

    if (trailTimer > 0.0f) trailTimer -= deltaTime * 25.0f;
    const bool trailEmit = trailTimer <= 0;
    if (trailEmit) trailTimer = 1.0f;

    unsigned int count = component_entity_count(COMPONENT_BULLET);
    for (unsigned int i = 0; i < count; i++) {
        Entity e = entity_find(COMPONENT_BULLET, i);
        vec2 pos = *(vec2*)entity_get(e, COMPONENT_PHI_RECT);
        bool rigidCollision = checkRigidCollision(e, vecZero);
        Entity playerCollision = checkPhiCollision(e, COMPONENT_SPRITE_ID);
        Entity firebarrel = checkFirebarrelCollision(e);

        float force;
        if (*(BulletKind*)entity_get(e, COMPONENT_BULLET) == BULLET_KIND_ROCKET) {
            if (trailEmit) archetypeSmoke(pos, TEXTURE_SMOKE3);
            force = 50.0f;
        } else force = 5.0f;

        if (!rigidCollision && !firebarrel && !playerCollision) continue;
        entity_destroy(e);
        archetypeBlast(pos, force);
        i--;
        count--;
    }
}