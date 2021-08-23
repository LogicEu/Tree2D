#include "../Tree.h"

void boxOpen(Entity box)
{
    BoxState* state = (BoxState*)entity_get(box, COMPONENT_BOX);
    if (*state == BOX_STATE_LOOSE) *state = BOX_STATE_OPEN;
}

static void boxRullette(Entity box)
{
    vec2 pos = *(vec2*)entity_get(box, COMPONENT_PHI_RECT);
    smokeEmit(pos, TEXTURE_SMOKE);
}

void boxStep()
{
    unsigned int count = component_entity_count(COMPONENT_BOX);
    for (unsigned int i = 0; i < count; i++) {
        Entity e = entity_find(COMPONENT_BOX, i);
        BoxState s = *(BoxState*)entity_get(e, COMPONENT_BOX);
        if (s != BOX_STATE_OPEN) continue;
        
        boxRullette(e);
        entity_destroy(e);
        i--;
        count--;
    }
}