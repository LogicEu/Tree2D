#include "../Tree.h"

bool checkRigidCollision(Entity entity, vec2 off)
{
    Rect* rPhiPtr = (Rect*)entity_get(entity, COMPONENT_PHI_RECT);
    Rect rPhi = *rPhiPtr;
    rPhi.x += off.x;
    rPhi.y += off.y;
    const unsigned int count = component_entity_count(COMPONENT_RIGID_COLLIDER);
    for (unsigned int i = 0; i < count; i++) {
        Entity e = entity_find(COMPONENT_RIGID_COLLIDER, i);
        Rect* r = (Rect*)entity_get(e, COMPONENT_PHI_RECT);
        if (r == rPhiPtr) continue;
        if (rect_overlap(rPhi, *r)) return true;
    }
    return false;
}

Entity checkGunCollision(Entity entity)
{
    Rect rPhi = *(Rect*)entity_get(entity, COMPONENT_PHI_RECT);
    const unsigned int count = component_entity_count(COMPONENT_GUN_CONTROLLER);
    for (unsigned int i = 0; i < count; i++) {
        Entity e = entity_find(COMPONENT_GUN_CONTROLLER, i);
        if (*(GunState*)entity_get(e, COMPONENT_GUN_CONTROLLER) != GUN_STATE_LOOSE) continue;
        Rect r = *(Rect*)entity_get(e, COMPONENT_PHI_RECT);
        if (rect_overlap(rPhi, r)) return e;
    }
    return 0;
}

Entity checkJetpackCollision(Entity entity)
{
    Rect rPhi = *(Rect*)entity_get(entity, COMPONENT_PHI_RECT);
    const unsigned int count = component_entity_count(COMPONENT_JETPACK);
    for (unsigned int i = 0; i < count; i++) {
        Entity e = entity_find(COMPONENT_JETPACK, i);
        if (*(unsigned int*)entity_get(e, COMPONENT_JETPACK) != JETPACK_LOOSE) continue;
        Rect r = *(Rect*)entity_get(e, COMPONENT_PHI_RECT);
        if (rect_overlap(rPhi, r)) return e;
    }
    return 0;
}

Entity checkFirebarrelCollision(Entity entity)
{
    Rect rPhi = *(Rect*)entity_get(entity, COMPONENT_PHI_RECT);
    const unsigned int count = component_entity_count(COMPONENT_FIREBARREL);
    for (unsigned int i = 0; i < count; i++) {
        Entity e = entity_find(COMPONENT_FIREBARREL, i);
        Rect r = *(Rect*)entity_get(e, COMPONENT_PHI_RECT);
        if (rect_overlap(rPhi, r)) return e;
    }
    return 0;
}

Entity checkGranadeCollision(Entity entity)
{
    Rect rPhi = *(Rect*)entity_get(entity, COMPONENT_PHI_RECT);
    const unsigned int count = component_entity_count(COMPONENT_GRANADE);
    for (unsigned int i = 0; i < count; i++) {
        Entity e = entity_find(COMPONENT_GRANADE, i);
        if (!((GranadeComponent*)entity_get(e, COMPONENT_GRANADE))->state) continue;
        Rect r = *(Rect*)entity_get(e, COMPONENT_PHI_RECT);
        if (rect_overlap(rPhi, r)) return e;
    }
    return 0;
}

Entity checkPhiCollision(Entity entity, Component component)
{
    Rect rPhi = *(Rect*)entity_get(entity, COMPONENT_PHI_RECT);
    unsigned int count = component_entity_count(component);
    for (unsigned int i = 0; i < count; i++) {
        Entity e = entity_find(component, i);
        Rect r = *(Rect*)entity_get(e, COMPONENT_PHI_RECT);
        if (rect_overlap(rPhi, r)) return e;
    }
    return 0;
}

unsigned int checkBlastCollision(Entity entity)
{
    Rect rPhi = *(Rect*)entity_get(entity, COMPONENT_PHI_RECT);
    unsigned int count = component_entity_count(COMPONENT_EXPLOTION);
    unsigned int dammage = 0;
    for (unsigned int i = 0; i < count; i++) {
        Entity e = entity_find(COMPONENT_EXPLOTION, i);
        Rect r = *(Rect*)entity_get(e, COMPONENT_PHI_RECT);
        float scale = *(float*)entity_get(e, COMPONENT_SCALE);
        float exp = *(float*)entity_get(e, COMPONENT_EXPLOTION);
        r.w *= scale;
        r.h *= scale;
        if (rect_overlap(rPhi, r)) {
            dammage += (unsigned int)(exp / scale);
        }
    }
    return dammage;
}

Entity checkPhiScaledCollision(Entity entity, Component component)
{
    Rect rPhi = *(Rect*)entity_get(entity, COMPONENT_PHI_RECT);
    unsigned int count = component_entity_count(component);
    for (unsigned int i = 0; i < count; i++) {
        Entity e = entity_find(component, i);
        Rect r = *(Rect*)entity_get(e, COMPONENT_PHI_RECT);
        float scale = *(float*)entity_get(e, COMPONENT_SCALE);
        r.w *= scale;
        r.h *= scale;
        if (rect_overlap(rPhi, r)) return e;
    }
    return 0;
}

Entity checkGlCollision(Entity entity, Component component)
{
    Rect rGl = *(Rect*)entity_get(entity, COMPONENT_GL_RECT);
    unsigned int count = component_entity_count(component);
    for (unsigned int i = 0; i < count; i++) {
        Entity e = entity_find(component, i);
        Rect r = *(Rect*)entity_get(e, COMPONENT_GL_RECT);
        if (rect_overlap(rGl, r)) return e;
    }
    return 0;
}