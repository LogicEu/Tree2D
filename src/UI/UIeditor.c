#include "UIcommon.h"
#include <stdio.h>
#include <string.h>

extern vec2 mouse;
static wxGroup* group;
static wxGroup* editorGroup;
static wxGroup* metaGroup;

typedef enum {
    WX_UIE_TITLE_MAIN,
    
    WX_UIE_BUTTON_MENU,
    WX_UIE_BUTTON_SAVE,
    WX_UIE_BUTTON_LOAD,
    WX_UIE_BUTTON_PREV,
    WX_UIE_BUTTON_NEXT
} wxUIeditorEnum;

typedef enum {
    WX_META_PREV,
    WX_META_NEXT
} wxMetaEnum;

static void wxEditSlider(wxSlider* slider, vec2 mouse)
{
    if (vec2_dist(slider->position, mouse) > 10.0f) return;
    slider->position = mouse;

    if (glee_key_down(GLFW_KEY_M)) {
        slider->scale += 0.01f;
    }
    if (glee_key_down(GLFW_KEY_N)) {
        slider->scale -= 0.02f;
    }

    if (glee_key_pressed(GLFW_KEY_SPACE)) {
        printf("Slider X: %f\n", slider->position.x);
        printf("Slider Y: %f\n", slider->position.y);
        printf("Slider Scale: %f\n", slider->scale);
        printf("Slider Value: %f\n", slider->lerp);
    }
}

static void wxEditRect(wxRect* rect, vec2 mouse)
{
    if (vec2_dist(mouse, *(vec2*)&rect->rect) > 10.0f) return;
    rect->rect.x = mouse.x;
    rect->rect.y = mouse.y;

    if (glee_key_down(GLFW_KEY_D)) {
        rect->rect.w += 0.2f;
    }
    if (glee_key_down(GLFW_KEY_A)) {
        rect->rect.w -= 0.2f;
    }
    if (glee_key_down(GLFW_KEY_W)) {
        rect->rect.h += 0.2f;
    }
    if (glee_key_down(GLFW_KEY_S)) {
        rect->rect.h -= 0.2f;
    }
    if (glee_key_down(GLFW_KEY_M)) {
        rect->rect.w += 0.01f;
        rect->rect.h += 0.01f;
    }
    if (glee_key_down(GLFW_KEY_N)) {
        rect->rect.w -= 0.01f;
        rect->rect.h -= 0.01f;
    }
}

static void wxEditTitle(wxTitle* title, vec2 mouse)
{
    if (vec2_dist(*(vec2*)&title->pos_scale, mouse) > 10.0f) return;
    memcpy(&title->pos_scale, &mouse, sizeof(vec2));

    if (glee_key_down(GLFW_KEY_M)) {
        title->pos_scale.z += 0.01f;
    }
    if (glee_key_down(GLFW_KEY_N)) {
        title->pos_scale.z -= 0.02f;
    }

    if (glee_key_pressed(GLFW_KEY_SPACE)) {
        printf("Title Text: %s\n", title->string);
        printf("Title X: %f\n", title->pos_scale.x);
        printf("Title Y: %f\n", title->pos_scale.y);
        printf("Title: %f\n", title->pos_scale.z);
        printf("Title Color: %f - %f - %f - %f\n", title->color.x, title->color.y, title->color.z, title->color.w);
    }
}

static void wxEditField(wxField* field, vec2 mouse)
{
    if (glee_key_down(GLFW_KEY_K)) {
        field->rect.x += 0.2f;
    }
    if (glee_key_down(GLFW_KEY_H)) {
        field->rect.x -= 0.2f;
    }
    if (glee_key_down(GLFW_KEY_U)) {
        field->rect.y += 0.2f;
    }
    if (glee_key_down(GLFW_KEY_J)) {
        field->rect.y -= 0.2f;
    }
    if (glee_key_down(GLFW_KEY_D)) {
        field->rect.w += 0.2f;
    }
    if (glee_key_down(GLFW_KEY_A)) {
        field->rect.w -= 0.2f;
    }
    if (glee_key_down(GLFW_KEY_W)) {
        field->rect.h += 0.2f;
    }
    if (glee_key_down(GLFW_KEY_S)) {
        field->rect.h -= 0.2f;
    }
    if (glee_key_pressed(GLFW_KEY_Z)) {
        field->rect.w *= 1.1f;
        field->rect.h *= 1.1f;
    }  
    if (glee_key_pressed(GLFW_KEY_X)) {
        field->rect.w *= 0.9f;
        field->rect.h *= 0.9f; 
    }  
    if (glee_key_down(GLFW_KEY_UP)) {
        field->text_offset.y += 0.01f;
    }
    if (glee_key_down(GLFW_KEY_DOWN)) {
        field->text_offset.y -= 0.01f;
    }
    if (glee_key_down(GLFW_KEY_RIGHT)) {
        field->text_offset.x += 0.01f;
    }
    if (glee_key_down(GLFW_KEY_LEFT)) {
        field->text_offset.x -= 0.01f;
    }
    if (glee_key_down(GLFW_KEY_M)) {
        field->text_offset.z += 0.01f;
    }
    if (glee_key_down(GLFW_KEY_N)) {
        field->text_offset.z -= 0.01f;
    }
    if (glee_key_pressed(GLFW_KEY_SPACE)) {
        printf("X: %f\n", field->rect.x);
        printf("Y: %f\n", field->rect.y);
        printf("Width: %f\n", field->rect.w);
        printf("Heigh: %f\n", field->rect.h);
        printf("Text Offset X: %f\n", field->text_offset.x);
        printf("Text Offset Y: %f\n", field->text_offset.y);
        printf("Text Scale: %f\n", field->text_offset.z);
        printf("Text: '%s'\n", field->text);
    }

    if (glee_key_pressed(GLFW_KEY_ENTER)) {
        for (unsigned int i = 0; i < group->used; i++) {
            wxPtr p = group->widgets[i];
            if (p.type == WIDGET_FIELD) {
                wxField* f = p.widget;
                f->rect.w = field->rect.w;
                f->rect.h = field->rect.h;
                f->text_offset = field->text_offset;
            }
        }
    }

    if (vec2_dist(*(vec2*)&field->rect, mouse) < 10.0f) {
        field->rect.x = mouse.x;
        field->rect.y = mouse.y;
    }
}

static void wxEditButton(wxButton* button, vec2 mouse)
{
    if (glee_key_down(GLFW_KEY_K)) {
        button->rect.x += 0.2f;
    }
    if (glee_key_down(GLFW_KEY_H)) {
        button->rect.x -= 0.2f;
    }
    if (glee_key_down(GLFW_KEY_U)) {
        button->rect.y += 0.2f;
    }
    if (glee_key_down(GLFW_KEY_J)) {
        button->rect.y -= 0.2f;
    }
    if (glee_key_down(GLFW_KEY_D)) {
        button->rect.w += 0.2f;
    }
    if (glee_key_down(GLFW_KEY_A)) {
        button->rect.w -= 0.2f;
    }
    if (glee_key_down(GLFW_KEY_W)) {
        button->rect.h += 0.2f;
    }
    if (glee_key_down(GLFW_KEY_S)) {
        button->rect.h -= 0.2f;
    }
    if (glee_key_pressed(GLFW_KEY_Z)) {
        button->rect.w *= 1.1f;
        button->rect.h *= 1.1f;
    }  
    if (glee_key_pressed(GLFW_KEY_X)) {
        button->rect.w *= 0.9f;
        button->rect.h *= 0.9f; 
    }  
    if (glee_key_down(GLFW_KEY_UP)) {
        button->text_offset.y += 0.01f;
    }
    if (glee_key_down(GLFW_KEY_DOWN)) {
        button->text_offset.y -= 0.01f;
    }
    if (glee_key_down(GLFW_KEY_RIGHT)) {
        button->text_offset.x += 0.01f;
    }
    if (glee_key_down(GLFW_KEY_LEFT)) {
        button->text_offset.x -= 0.01f;
    }
    if (glee_key_down(GLFW_KEY_M)) {
        button->text_offset.z += 0.01f;
    }
    if (glee_key_down(GLFW_KEY_N)) {
        button->text_offset.z -= 0.01f;
    }
    if (glee_key_pressed(GLFW_KEY_SPACE)) {
        printf("X: %f\n", button->rect.x);
        printf("Y: %f\n", button->rect.y);
        printf("Width: %f\n", button->rect.w);
        printf("Heigh: %f\n", button->rect.h);
        printf("Text Offset X: %f\n", button->text_offset.x);
        printf("Text Offset Y: %f\n", button->text_offset.y);
        printf("Text Scale: %f\n", button->text_offset.z);
    }

    if (glee_key_pressed(GLFW_KEY_ENTER)) {
        for (unsigned int i = 0; i < group->used; i++) {
            wxPtr p = group->widgets[i];
            if (p.type == WIDGET_BUTTON) {
                wxButton* b = p.widget;
                b->rect.w = button->rect.w;
                b->rect.h = button->rect.h;
                b->text_offset = button->text_offset;
            }
        }
    }

    if (vec2_dist(*(vec2*)&button->rect, mouse) < 10.0f) {
        button->rect.x = mouse.x;
        button->rect.y = mouse.y;
    }
}

static void prevNextDir(wxGroup* g, bool mousePressed, unsigned int prev_index)
{
    wxButton* button = g->widgets[prev_index].widget;
    if (button->state == WIDGET_HOVER && mousePressed) {
        if (wxDir.selected > 0) wxDir.selected--;
        else wxDir.selected = WX_DIR_META - 1;
        group = &wxDir.groups[wxDir.selected];
    }
    button = g->widgets[++prev_index].widget;
    if (button->state == WIDGET_HOVER && mousePressed) {
        if (wxDir.selected < WX_DIR_META - 1) wxDir.selected++;
        else wxDir.selected = WX_DIR_MAIN_MENU;
        group = &wxDir.groups[wxDir.selected];
    }
}

static void getInput()
{
    bool mousePressed = glee_mouse_pressed(GLFW_MOUSE_BUTTON_LEFT);
    bool mouseDown = glee_mouse_down(GLFW_MOUSE_BUTTON_LEFT);
    bool isMeta = false;
    
    wxButton* button;
    if (group == editorGroup) {
        prevNextDir(metaGroup, mousePressed, WX_META_PREV);
        wxGroupUpdate(metaGroup, mouse, mousePressed, mouseDown);
        wxGroupUpdate(editorGroup, mouse, mousePressed, mouseDown);
        wxGroupDraw(metaGroup);
        isMeta = true;
    } else wxGroupUpdate(group, mouse, mousePressed, mouseDown);

    if (!isMeta) {
        button = editorGroup->widgets[WX_UIE_BUTTON_MENU].widget;
        bool buttonPressed = button->state == WIDGET_HOVER && mousePressed;
        if (glee_key_pressed(GLFW_KEY_ESCAPE) || button->state == WIDGET_SELECTED) {
            systemSetState(STATE_MENU);
            button->state = WIDGET_UNSELECTED;
            return;
        }

        button = editorGroup->widgets[WX_UIE_BUTTON_SAVE].widget;
        buttonPressed = button->state == WIDGET_HOVER && mousePressed;
        if (glee_key_pressed(GLFW_KEY_P) || buttonPressed) {
            wxDirectorySave(FILE_MENU_UI_SAVE, &wxDir);
            return;
        }

        button = editorGroup->widgets[WX_UIE_BUTTON_LOAD].widget;
        buttonPressed = button->state == WIDGET_HOVER && mousePressed;
        if (glee_key_pressed(GLFW_KEY_L) || buttonPressed) {
            wxDirectory tmp = wxDirectoryLoad(FILE_MENU_UI_SAVE);
            if (tmp.groups != NULL) {
                wxDirectoryFree(&wxDir);
                wxDir = tmp;
                treeDirectoryReset();
            }
            return;
        }
        prevNextDir(editorGroup, mousePressed, WX_UIE_BUTTON_PREV);
    }

    wxGroupUpdate(editorGroup, mouse, mousePressed, mouseDown);
    if (!mouseDown) return;
    for (unsigned int i = 0; i < group->used; i++) {
        wxPtr p = group->widgets[i];
        if (p.type == WIDGET_TITLE) {
            wxEditTitle(p.widget, mouse);
        } else if (p.type == WIDGET_BUTTON) {
            button = p.widget;
            if (button->state == WIDGET_SELECTED) wxEditButton(button, mouse);
        } else if (p.type == WIDGET_SLIDER) {
            wxSlider* slider = p.widget;
            wxEditSlider(slider, mouse);
        } else if (p.type == WIDGET_RECT) {
            wxRect* rect = p.widget;
            wxEditRect(rect, mouse);
        } else if (p.type == WIDGET_FIELD) {
            wxField* field = p.widget;
            wxEditField(field, mouse);
        }
    }
}

void UIeditorDraw()
{
    wxGroupDraw(group);
    wxGroupDraw(editorGroup);
    mouseDraw();
}

void UIeditorDirectoryReset()
{
    group = &wxDir.groups[wxDir.selected];
    editorGroup = &wxDir.groups[WX_DIR_UI_EDITOR];
    metaGroup = &wxDir.groups[WX_DIR_META];
}

void UIeditorInit()
{
    UIeditorDirectoryReset();
}

void UIeditorStep()
{
    getInput();
    cameraBackgroundSlide();
    UIeditorDraw();
}
