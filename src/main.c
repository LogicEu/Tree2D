#include "Tree.h"

int main(void)
{
    unsigned int state = STATE_MENU;
    systemInit(state);
    float lastTime = glee_time_get(), deltaTime;
    while (glee_window_is_open()) {
        deltaTime = glee_time_delta(&lastTime);
        systemStep(deltaTime);
    }
    systemExit();
}
