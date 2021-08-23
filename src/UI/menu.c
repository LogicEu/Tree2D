#include "UIcommon.h"

extern vec2 mouse;
extern vec4 cam;

static wxGroup* group;

typedef enum {
    WX_TITLE_GAME,
    WX_TITLE_MAIN_MENU,
    WX_BUTTON_PLAY,
    WX_BUTTON_LEVEL_EDITOR,
    WX_BUTTON_QUIT,
    WX_BUTTON_UI_EDITOR,
    WX_BUTTON_SPRITE_EDITOR,
    WX_BUTTON_OPTIONS,
    WX_BUTTON_ONLINE
} menuUIEnum;

static void getInput()
{
    bool mousePressed = glee_mouse_pressed(GLFW_MOUSE_BUTTON_LEFT);
    wxGroupUpdate(group, mouse, mousePressed, mousePressed);

    wxButton* play = group->widgets[WX_BUTTON_PLAY].widget;
    wxButton* level_editor = group->widgets[WX_BUTTON_LEVEL_EDITOR].widget;
    wxButton* quit = group->widgets[WX_BUTTON_QUIT].widget;
    wxButton* ui_editor = group->widgets[WX_BUTTON_UI_EDITOR].widget;
    wxButton* options = group->widgets[WX_BUTTON_OPTIONS].widget;
    wxButton* sprite_editor = group->widgets[WX_BUTTON_SPRITE_EDITOR].widget;
    wxButton* online = group->widgets[WX_BUTTON_ONLINE].widget;

    if (play->state == WIDGET_SELECTED) systemSetState(STATE_PLAY);
    if (level_editor->state == WIDGET_SELECTED) systemSetState(STATE_LEVEL_EDITOR);
    if (ui_editor->state == WIDGET_SELECTED) systemSetState(STATE_UI_EDITOR);
    if (options->state == WIDGET_SELECTED) systemSetState(STATE_OPTIONS);
    if (sprite_editor->state == WIDGET_SELECTED) systemSetState(STATE_SPRITE_EDITOR);
    if (online->state == WIDGET_SELECTED) systemSetState(STATE_NET_MENU);
    if (glee_key_pressed(GLFW_KEY_ESCAPE) || quit->state == WIDGET_SELECTED) {
        systemExit();
    }
}

void mouseDraw()
{
    static float camera[] = {0.0f, 0.0f, 1.0f, 0.0f};
    static vec4 white = {1.0f, 1.0f, 1.0f, 1.0f};
    glUseProgram(assetsGetShader(SHADER_TEXTURE));
    glBindVertexArray(quadVAO);
    glee_shader_uniform_set(assetsGetShader(SHADER_TEXTURE), 4, "camera", &camera[0]);
    drawTextureColor(*assetsGetTexture(TEXTURE_MOUSE_CURSOR), vec2_new(mouse.x + 4.0f, mouse.y - 4.0f), white);
}

void menuDraw()
{
    wxGroupDraw(group);
    mouseDraw();
}

void cameraBackgroundSlide()
{
    static float target = 1600.0f;
    if (cam.x >= 1600.0f) target = 0.0f;
    if (cam.x <= 0.0f) target = 1600.0f;
    cam.x = lerpf(cam.x, target, 0.0001f);
}

void menuStep()
{
    getInput();
    menuDraw();
    cameraBackgroundSlide();
}

void menuDirectoryReset()
{
    group = &wxDir.groups[WX_DIR_MAIN_MENU];   
}

void menuInit()
{
    menuDirectoryReset();
}