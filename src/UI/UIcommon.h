#include "../Tree.h"

#define FILE_MENU_UI_SAVE "assets/data/menu_style.bin"

typedef enum {
    WX_DIR_MAIN_MENU,
    WX_DIR_GAME,
    WX_DIR_LEVEL_EDITOR,
    WX_DIR_UI_EDITOR,
    WX_DIR_SPRITE_EDITOR,
    WX_DIR_OPTIONS,
    WX_DIR_NET_MENU,
    WX_DIR_META
} dirEnum;

extern wxDirectory wxDir;

void mouseDraw();
void cameraBackgroundSlide();
