#include "../Tree.h"

extern Entity netFirebarrelExp;

void firebarrelStep()
{
    unsigned int count = component_entity_count(COMPONENT_FIREBARREL);
    for (unsigned int i = 0; i < count; i++) {
        Entity e = entity_find(COMPONENT_FIREBARREL, i);
        bool* b = (bool*)entity_get(e, COMPONENT_FIREBARREL);
        if (*b || checkPhiScaledCollision(e, COMPONENT_EXPLOTION)) {
            archetypeBlast(*(vec2*)entity_get(e, COMPONENT_PHI_RECT), 80.0f);
            entity_destroy(e);
            netFirebarrelExp = e;
            count--;
            i--;
        } 
    }
}