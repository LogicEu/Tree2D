#include "UIcommon.h"
#include "../TreeNet.h"
#include <stdio.h>

extern vec2 mouse;
extern vec4 cam;

wxGroup* group;
NNetHost* host;

typedef enum {
    WX_NET_MENU_TITLE_MAIN,
    WX_NET_MENU_TITLE_IP,
    WX_NET_MENU_TITLE_USER,
    
    WX_NET_MENU_FIELD_IP,
    WX_NET_MENU_FIELD_USER,
    
    WX_NET_MENU_BUTTON_MENU,
    WX_NET_MENU_BUTTON_CONNECT
} wxNetMenuEnum;

static void getInput()
{
    bool mousePressed = glee_mouse_pressed(GLFW_MOUSE_BUTTON_LEFT);
    wxGroupUpdate(group, mouse, mousePressed, mousePressed);

    wxField* fieldIp = group->widgets[WX_NET_MENU_FIELD_IP].widget;
    wxField* fieldUser = group->widgets[WX_NET_MENU_FIELD_USER].widget;
    wxButton* button = group->widgets[WX_NET_MENU_BUTTON_CONNECT].widget;
    
    if (glee_key_pressed(GLFW_KEY_ENTER) || button->state == WIDGET_SELECTED) {
        printf("%s:%s\n", fieldUser->text, fieldIp->text);
        treeNetInit(fieldUser->text, fieldIp->text);
        return;
    }

    button = group->widgets[WX_NET_MENU_BUTTON_MENU].widget;
    if (glee_key_pressed(GLFW_KEY_ESCAPE) || button->state == WIDGET_SELECTED) {
        systemSetState(STATE_MENU);
    }
}

static void netMenuDraw()
{
    wxGroupDraw(group);
    mouseDraw();
}

void netMenuStep()
{
    getInput();
    netMenuDraw();
    cameraBackgroundSlide();
}

void netMenuDirectoryReset()
{
    group = &wxDir.groups[WX_DIR_NET_MENU];
}

void netMenuInit()
{
    netMenuDirectoryReset();
    host = NULL;
}