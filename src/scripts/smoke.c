#include "../Tree.h"

static float smokeAlarm = 1.0f;

static void smokeApply(Entity e, float deltaTime)
{
    float* alpha = (float*)entity_get(e, COMPONENT_ALPHA);
    Rect* rGl = (Rect*)entity_get(e, COMPONENT_GL_RECT);
    float* scale = (float*)entity_get(e, COMPONENT_SCALE);
    
    rGl->y += deltaTime * 10.0f * (*alpha);
    *scale += deltaTime;
    *alpha -= deltaTime;
}

void smokeEmit(vec2 position, unsigned int textureIndex)
{
    if (smokeAlarm < 0.0f) {
        archetypeSmoke(position, textureIndex);
        smokeAlarm = 1.0f;
    }
}

void smokeStep(float deltaTime)
{
    static const float smokeAlarmRate = 10.0f;
    if (smokeAlarm >= 0.0f) smokeAlarm -= deltaTime * smokeAlarmRate;
    unsigned int count = component_entity_count(COMPONENT_SMOKE);
    for (unsigned int i = 0; i < count; i++) {
        Entity e = entity_find(COMPONENT_SMOKE, i);
        if (*(float*)entity_get(e, COMPONENT_ALPHA) < 0.0f) {
            entity_destroy(e);
            i--;
            count--;
            continue;
        }
        smokeApply(e, deltaTime);
    }
}