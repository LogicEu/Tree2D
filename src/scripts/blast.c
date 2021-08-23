#include "../Tree.h"

static void blastApply(Entity e, float deltaTime)
{
    float* explotion = (float*)entity_get(e, COMPONENT_EXPLOTION);
    float* alpha = (float*)entity_get(e, COMPONENT_ALPHA);
    Rect* rGl = (Rect*)entity_get(e, COMPONENT_GL_RECT);
    Rect* rPhi = (Rect*)entity_get(e, COMPONENT_PHI_RECT);
    float* scale = (float*)entity_get(e, COMPONENT_SCALE);
    vec2* vel = (vec2*)entity_get(e, COMPONENT_VEL_VEC2);
    
    *explotion = lerpf(*explotion, 0.0f, 0.1f);
    *scale += *explotion * deltaTime;
    *alpha -= deltaTime;
    rPhi->y += deltaTime * 10.0f * (*alpha);
    rGl->y = rPhi->y;

    *vel = vec2_mult(*vel, 0.94f);
}

void blastStep(float deltaTime)
{
    unsigned int count = component_entity_count(COMPONENT_EXPLOTION);
    for (unsigned int i = 0; i < count; i++) {
        Entity e = entity_find(COMPONENT_EXPLOTION, i);
        if (*(float*)entity_get(e, COMPONENT_ALPHA) < 0.0f) {
            entity_destroy(e);
            i--;
            count--;
            continue;
        }
        blastApply (e, deltaTime);
    }
}