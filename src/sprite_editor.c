#include "Tree.h"
#include "UI/UIcommon.h"
#include <string.h>
#include <stdio.h>

extern vec3 viewport;
extern vec2 mouse;
extern wxDirectory wxDir;
extern unsigned int currentPlayerSprite;
extern Entity player;

static wxGroup* group;
bmp_t bmp;
static vec4 cursorColor;
static float scale;
static unsigned int fieldWidth, fieldHeight;

#define SCALE_MULT 4.0f

typedef enum {
    WX_SE_TITLE_MAIN,
    WX_SE_TITLE_R,
    WX_SE_TITLE_G,
    WX_SE_TITLE_B,
    WX_SE_TITLE_A,
    WX_SE_TITLE_ZOOM,

    WX_SE_BUTTON_MENU,
    WX_SE_BUTTON_SAVE,
    WX_SE_BUTTON_LOAD,
    WX_SE_BUTTON_NEW,
    WX_SE_BUTTON_SUBMIT,
    
    WX_SE_FIELD_DIR,
    WX_SE_FIELD_WIDTH,
    WX_SE_FIELD_HEIGHT,

    WX_SE_SLIDER_RED,
    WX_SE_SLIDER_GREEN,
    WX_SE_SLIDER_BLUE,
    WX_SE_SLIDER_ALPHA,
    WX_SE_SLIDER_ZOOM,
    WX_SE_SLIDER_RGB,

    WX_SE_RECT_BLACK,
    WX_SE_RECT_RED,
    WX_SE_RECT_GREEN,
    WX_SE_RECT_BLUE,
    WX_SE_RECT_YELLOW,
    WX_SE_RECT_CIAN,
    WX_SE_RECT_VIOLET,
    WX_SE_RECT_WHITE,
    WX_SE_RECT_CURSOR,

    WX_SE_ICON_BRUSH,
    WX_SE_ICON_ERASER,
    WX_SE_ICON_TAR,
    WX_SE_ICON_DROPPICKER
} spriteEditorEnum;

typedef enum {
    CURSOR_MODE_BRUSH,
    CURSOR_MODE_ERASER,
    CURSOR_MODE_TAR,
    CURSOR_MODE_DROPPICKER
} cursorModeState;

static cursorModeState cursorMode;

static void floodFill(bmp_t* restrict bmp, const uint8_t* restrict src, const uint8_t* restrict stat, unsigned int x, unsigned int y)
{
    uint8_t* p = px_at(bmp, x, y);
    if (!memcmp(p, src, bmp->channels) ||
        memcmp(p, stat, bmp->channels)) {
        return;
    }
    memcpy(p, src, bmp->channels);

    if (x + 1 < bmp->width) floodFill(bmp, src, stat, x + 1, y);
    if (x > 0) floodFill(bmp, src, stat, x - 1, y);
    if (y + 1 < bmp->height) floodFill(bmp, src, stat, x, y + 1);
    if (y > 0) floodFill(bmp, src, stat, x, y - 1);
}

static void slidersReset()
{
    wxSlider* slider;
    slider = group->widgets[WX_SE_SLIDER_RED].widget;
    slider->lerp = cursorColor.x;
    slider = group->widgets[WX_SE_SLIDER_GREEN].widget;
    slider->lerp = cursorColor.y;
    slider = group->widgets[WX_SE_SLIDER_BLUE].widget;
    slider->lerp = cursorColor.z;
    slider = group->widgets[WX_SE_SLIDER_ALPHA].widget;
    slider->lerp = cursorColor.w;
    slider = group->widgets[WX_SE_SLIDER_ZOOM].widget;
    slider->lerp = clampf(0.0f, 1.0f, (scale - 0.5f) / SCALE_MULT);
}

static void wxInput()
{
    wxRect* rect = group->widgets[WX_SE_RECT_CURSOR].widget;
    rect->color = cursorColor;

    wxButton* button = group->widgets[WX_SE_BUTTON_MENU].widget;
    if (button->state == WIDGET_SELECTED) {
        systemSetState(STATE_MENU);
        return;
    }

    wxSlider* slider = group->widgets[WX_SE_SLIDER_RED].widget;
    if (slider->selected) cursorColor.x = slider->lerp;
    slider = group->widgets[WX_SE_SLIDER_GREEN].widget;
    if (slider->selected) cursorColor.y = slider->lerp;
    slider = group->widgets[WX_SE_SLIDER_BLUE].widget;
    if (slider->selected) cursorColor.z = slider->lerp;
    slider = group->widgets[WX_SE_SLIDER_ALPHA].widget;
    if (slider->selected) cursorColor.w = slider->lerp;
    slider = group->widgets[WX_SE_SLIDER_ZOOM].widget;
    if (slider->selected) scale = 0.5f + slider->lerp * SCALE_MULT;
    slider = group->widgets[WX_SE_SLIDER_RGB].widget;
    if (slider->selected) {
        cursorColor.x = slider->lerp;
        cursorColor.y = slider->lerp;
        cursorColor.z = slider->lerp;
        slidersReset();
    }

    if (glee_mouse_down(GLFW_MOUSE_BUTTON_LEFT)) {
        for (unsigned int i = WX_SE_RECT_BLACK; i <= WX_SE_RECT_WHITE; i++) {
            rect = group->widgets[i].widget;
            if (rect_point_overlap(rect->rect, mouse)) {
                memcpy(&cursorColor, &rect->color, sizeof(vec4));
                slidersReset();
                break;
            }
        }
    }
}

static void editorInput()
{
    if (glee_key_pressed(GLFW_KEY_ESCAPE)) {
        systemSetState(STATE_MENU);
    }

    if (glee_window_file_check()) {
        bmp_t tmp = bmp_load(glee_window_file_get());
        bmp_t tmp2 = bmp_resize_width(&tmp, 32);
        bmp_free(&tmp);
        tmp = bmp_resize_height(&tmp2, 32);
        bmp_free(&tmp2);
        bmp = bmp_flip_vertical(&tmp);
        bmp_free(&tmp);
    }

    bool mousePressed = glee_mouse_pressed(GLFW_MOUSE_BUTTON_LEFT);
    bool mouseDown = glee_mouse_down(GLFW_MOUSE_BUTTON_LEFT);
    
    wxButton* button = group->widgets[WX_SE_BUTTON_NEW].widget;
    if (button->state == WIDGET_HOVER && mousePressed) {
        wxField* field = group->widgets[WX_SE_FIELD_WIDTH].widget;
        sscanf(field->text, "%u", &fieldWidth);
        fieldWidth = fieldWidth * (fieldWidth <= 128) + 128 * (fieldWidth > 128);
        sprintf(field->text, "%u", fieldWidth);

        field = group->widgets[WX_SE_FIELD_HEIGHT].widget;
        sscanf(field->text, "%u", &fieldHeight);
        fieldHeight = fieldHeight * (fieldHeight <= 128) + 128 * (fieldHeight > 128);
        sprintf(field->text, "%u", fieldHeight);

        bmp_free(&bmp);
        bmp = bmp_new(fieldWidth, fieldHeight, 4);
        memset(bmp.pixels, 255, bmp.width * bmp.height * bmp.channels);
    }
    wxField* field = group->widgets[WX_SE_FIELD_DIR].widget;
    button = group->widgets[WX_SE_BUTTON_LOAD].widget;
    if (button->state == WIDGET_HOVER && mousePressed) {
        bmp_t tmp = bmp_load(field->text);
        if (tmp.pixels != NULL) {
            bmp_free(&bmp);
            bmp = tmp;
        }
    }
    button = group->widgets[WX_SE_BUTTON_SAVE].widget;
    if (button->state == WIDGET_HOVER && mousePressed) {
        bmp_t tmp = bmp_flip_vertical(&bmp);
        bmp_write(field->text, &tmp);
        bmp_free(&tmp);
    }
    button = group->widgets[WX_SE_BUTTON_SUBMIT].widget;
    if (button->state == WIDGET_HOVER && mousePressed) {
        bmp_t tmp = bmp_flip_vertical(&bmp);
        spriteCollectionSubmitCustom(&tmp);
        bmp_free(&tmp);
        currentPlayerSprite = SPRITE_CUSTOM;
        *(unsigned int*)entity_get(player, COMPONENT_SPRITE_COLLECTION) = currentPlayerSprite;
    }
    
    wxIcon* icon = group->widgets[WX_SE_ICON_BRUSH].widget;
    if (icon->frame.state == WIDGET_HOVER && mousePressed) {
        cursorMode = CURSOR_MODE_BRUSH;
    }
    icon = group->widgets[WX_SE_ICON_ERASER].widget;
    if (icon->frame.state == WIDGET_HOVER && mousePressed) {
        cursorMode = CURSOR_MODE_ERASER;
    }
    icon = group->widgets[WX_SE_ICON_TAR].widget;
    if (icon->frame.state == WIDGET_HOVER && mousePressed) {
        cursorMode = CURSOR_MODE_TAR;
    }
    icon = group->widgets[WX_SE_ICON_DROPPICKER].widget;
    if (icon->frame.state == WIDGET_HOVER && mousePressed) {
        cursorMode = CURSOR_MODE_DROPPICKER;
    }

    wxGroupUpdate(group, mouse, mousePressed, mouseDown);

    if (mouseDown) {
        Rect r = {
            0.5f * viewport.x / viewport.z, 
            0.5f * viewport.y / viewport.z, 
            (float)bmp.width * scale, 
            (float)bmp.height * scale
        };

#define _ftou(f) (uint8_t)((f) * 255.0f)
#define _utof(u) ((float)(u) / 255.0)

        if (rect_point_overlap(r, mouse)) {
            float dx = mouse.x - r.x + r.w * 0.5f;
            float dy = mouse.y - r.y + r.h * 0.5f;
            unsigned int x = (unsigned int)(int)clampf(dx / scale, 0.0f, bmp.width);
            unsigned int y = (unsigned int)(int)clampf(dy / scale, 0.0f, bmp.height);
            uint8_t p[] = {_ftou(cursorColor.x), _ftou(cursorColor.y), _ftou(cursorColor.z), _ftou(cursorColor.w)};
            uint8_t* ptr = px_at(&bmp, x, y);

            if (cursorMode == CURSOR_MODE_BRUSH) {
                memcpy(ptr, &p[0], bmp.channels);
            } else if (cursorMode == CURSOR_MODE_ERASER) {
                memset(ptr, 0, bmp.channels);
            } else if (cursorMode == CURSOR_MODE_TAR) {
                uint8_t stat[bmp.channels];
                memcpy(&stat[0], ptr, bmp.channels);
                floodFill(&bmp, &p[0], &stat[0], x, y);
            } else if (cursorMode == CURSOR_MODE_DROPPICKER) {
                memcpy(&p[0], ptr, bmp.channels);
                cursorColor = vec4_new(_utof(p[0]), _utof(p[1]), _utof(p[2]), _utof(p[3]));
                slidersReset();
            }
        }
    }
}

static void SEmouseDraw()
{
    static float camera[] = {0.0f, 0.0f, 1.0f, 0.0f};
    static vec4 white = {1.0f, 1.0f, 1.0f, 1.0f};
    vec4 col = {cursorColor.x, cursorColor.y, cursorColor.z, clampf(cursorColor.w, 0.3, 1.0)};
    glUseProgram(assetsGetShader(SHADER_TEXTURE));
    glBindVertexArray(quadVAO);
    glee_shader_uniform_set(assetsGetShader(SHADER_TEXTURE), 4, "camera", &camera[0]);
    switch (cursorMode) {
        case CURSOR_MODE_BRUSH: {
            drawTextureColor(*assetsGetTexture(TEXTURE_BRUSH), vec2_new(mouse.x + 6.0f, mouse.y + 6.0f), col);
            break;
        }
        case CURSOR_MODE_ERASER: {
            drawTextureColor(*assetsGetTexture(TEXTURE_ERASER), vec2_new(mouse.x + 6.0f, mouse.y + 6.0f), white);
            break;
        }
        case CURSOR_MODE_TAR: {
            drawTextureColor(*assetsGetTexture(TEXTURE_TAR), vec2_new(mouse.x + 6.0f, mouse.y + 6.0f), col);
            break;
        }
        case CURSOR_MODE_DROPPICKER: {
            drawTextureColor(*assetsGetTexture(TEXTURE_DROPPICKER), vec2_new(mouse.x + 6.0f, mouse.y + 6.0f), col);
            break;
        }
    }
}

static void spriteEditorDraw()
{
    float xscale = viewport.x / viewport.z;
    float yscale = viewport.y / viewport.z;
    bmpDraw(&bmp, xscale * 0.5f, yscale * 0.5f, scale);
    wxGroupDraw(group);
    SEmouseDraw();
}

static void getInput()
{
    editorInput();
    wxInput();
}

void spriteEditorStep()
{
    getInput();
    spriteEditorDraw();
}

void spriteEditorDirectoryReset()
{
    group = &wxDir.groups[WX_DIR_SPRITE_EDITOR];
}

void spriteEditorInit()
{
    spriteEditorDirectoryReset();
    uint8_t color[] = {255, 255, 255, 255};
    bmp = bmp_color(32, 32, 4, color);
    scale = 2.0f;
    cursorColor = vec4_new(0.0f, 0.0f, 0.0f, 1.0);
    cursorMode = CURSOR_MODE_BRUSH;
    slidersReset();
}