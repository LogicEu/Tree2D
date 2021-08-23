#include "UIcommon.h"
#include <string.h>
#include <stdio.h>

extern vec3 viewport;
extern unsigned int randSeed;

wxDirectory wxDir;

static void universeUI()
{
    universeInit(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_SCALE);

    vec4 primary[] = {
        {0.3f, 0.3f, 1.0f, 1.0f}, 
        {0.0f, 0.0f, 1.0f, 1.0f},
        {1.0f, 0.0f, 0.0f, 1.0f}
    };
    vec4 secondary[] = {
        {1.0f, 0.3f, 0.3f, 1.0f}, 
        {1.0f, 0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f, 1.0f}
    };
    vec4 terciary[3] = {
        {0.3f, 0.3f, 0.3f, 1.0f},
        {0.0f, 0.0f, 0.0f, 1.0f},
        {1.0f, 1.0f, 1.0f, 1.0f}
    };

    universeThemeSet(primary, secondary, terciary);
    universeFontSet(fontLoad("assets/fonts/Pixels.ttf", 32));
    universeFrameSet(texture_load("assets/sprites/frame16.png"));
    universeSliderSet(
        texture_load("assets/sprites/slider.png"),
        texture_load("assets/sprites/marker.png")
    );
    universeSwitchSet(
        texture_load("assets/sprites/switch_down.png"),
        texture_load("assets/sprites/switch_up.png")
    );
}

static wxGroup mainMenuGroup(float xscale)
{
    float x = xscale * 0.5f;
    wxGroup group = wxGroupCreate();
    
    wxTitle title = wxTitleCreate("TREE", vec3_new(100.0f, 170.0f, 2.5f), vec4_new(0.3f, 1.0f, 0.3f, 1.0f));
    wxGroupPush(&group, &title, WIDGET_TITLE);
    title = wxTitleCreate("Main Menu", vec3_new(100.0f, 155.0f, 0.8f), vec4_new(1.0f, 0.0f, 0.0f, 1.0f));
    wxGroupPush(&group, &title, WIDGET_TITLE);
    
    wxButton button = wxButtonCreate("Play", rect_new(x, 130.0f, 40.0f, 15.0f));
    button.text_offset = vec3_new(2.0f, 2.0f, 2.5f);
    wxGroupPush(&group, &button, WIDGET_BUTTON);
    button = wxButtonCreate("Level Editor", rect_new(x, 90.0f, 108.0f, 15.0f));
    button.text_offset = vec3_new(2.0f, 2.0f, 2.5f);
    wxGroupPush(&group, &button, WIDGET_BUTTON);
    button = wxButtonCreate("Quit", rect_new(x, 10.0f, 50.0f, 15.0f));
    button.text_offset = vec3_new(2.0f, 2.0f, 2.5f);
    wxGroupPush(&group, &button, WIDGET_BUTTON);
    button = wxButtonCreate("UI Editor", rect_new(x, 50.0f, 90.0f, 15.0f));
    button.text_offset = vec3_new(2.0f, 2.0f, 2.5f);
    wxGroupPush(&group, &button, WIDGET_BUTTON);
    button = wxButtonCreate("Sprite Editor", rect_new(x, 70.0f, 130.0f, 15.0f));
    button.text_offset = vec3_new(2.0f, 2.0f, 2.5f);
    wxGroupPush(&group, &button, WIDGET_BUTTON);
    button = wxButtonCreate("Options", rect_new(x, 30.0f, 80.0f, 15.0f));
    button.text_offset = vec3_new(2.0f, 2.0f, 2.5f);
    wxGroupPush(&group, &button, WIDGET_BUTTON);
    button = wxButtonCreate("Online", rect_new(x, 110.0f, 60.0f, 15.0f));
    button.text_offset = vec3_new(2.0f, 2.0f, 2.5f);
    wxGroupPush(&group, &button, WIDGET_BUTTON);
    
    return group;
}

static wxGroup spriteEditorGroup(float xscale, float yscale)
{
   
    wxGroup group = wxGroupCreate();
    
    wxTitle title = wxTitleCreate("Sprite Editor Mode", vec3_new(4.0f, 185.0f, 1.0f), vec4_new(1.0f, 0.0f, 0.0f, 1.0f));
    wxGroupPush(&group, &title, WIDGET_TITLE);
    title = wxTitleCreate("R", vec3_new(xscale - 42.0f, 4.0f, 0.8f), vec4_new(1.0f, 0.0f, 0.0f, 1.0f));
    wxGroupPush(&group, &title, WIDGET_TITLE);
    title = wxTitleCreate("G", vec3_new(xscale - 32.0f, 4.0f, 0.8f), vec4_new(0.0f, 1.0f, 0.0f, 1.0f));
    wxGroupPush(&group, &title, WIDGET_TITLE);
    title = wxTitleCreate("B", vec3_new(xscale - 22.0f, 4.0f, 0.8f), vec4_new(0.0f, 0.0f, 1.0f, 1.0f));
    wxGroupPush(&group, &title, WIDGET_TITLE);
    title = wxTitleCreate("A", vec3_new(xscale - 12.0f, 4.0f, 0.8f), vec4_new(0.5f, 0.5f, 0.5f, 1.0f));
    wxGroupPush(&group, &title, WIDGET_TITLE);
    title = wxTitleCreate("Zoom", vec3_new(xscale - 48.0f, 90.0f, 0.8f), vec4_new(0.5f, 0.5f, 0.5f, 1.0f));
    wxGroupPush(&group, &title, WIDGET_TITLE);

    wxButton button = wxButtonCreate("Menu", rect_new(32.0f, yscale - 32.0f, 50.0f, 15.0f));
    button.text_offset = vec3_new(2.0f, 2.0f, 2.5f);
    wxGroupPush(&group, &button, WIDGET_BUTTON);
    button = wxButtonCreate("Save", rect_new(32.0f, yscale - 52.0f, 50.0f, 15.0f));
    button.text_offset = vec3_new(2.0f, 2.0f, 2.5f);
    wxGroupPush(&group, &button, WIDGET_BUTTON);
    button = wxButtonCreate("Load", rect_new(32.0f, yscale - 72.0f, 50.0f, 15.0f));
    button.text_offset = vec3_new(2.0f, 2.0f, 2.5f);
    wxGroupPush(&group, &button, WIDGET_BUTTON);
    button = wxButtonCreate("New", rect_new(32.0f, yscale - 92.0f, 50.0f, 15.0f));
    button.text_offset = vec3_new(2.0f, 2.0f, 2.5f);
    wxGroupPush(&group, &button, WIDGET_BUTTON);
    button = wxButtonCreate("Submit", rect_new(32.0f, yscale - 112.0f, 50.0f, 15.0f));
    button.text_offset = vec3_new(1.8f, 2.0f, 1.9f);
    wxGroupPush(&group, &button, WIDGET_BUTTON);
    
    wxField field = wxFieldCreate(rect_new(xscale * 0.5f, 8.0f, 160.0f, 8.0f), 32);
    field.text_offset = vec3_new(2.0f, 0.2f, 2.5f);
    strcpy(field.text, "sprite.png");
    field.strMark = strlen(field.text);
    wxGroupPush(&group, &field, WIDGET_FIELD);
    field = wxFieldCreate(rect_new(xscale * 0.5f - 16, 24.0f, 20.0f, 8.0f), 3);
    field.text_offset = vec3_new(2.0f, 0.2f, 2.5f);
    strcpy(field.text, "32");
    field.strMark = strlen(field.text);
    wxGroupPush(&group, &field, WIDGET_FIELD);
    field = wxFieldCreate(rect_new(xscale * 0.5f + 16, 24.0f, 20.0f, 8.0f), 3);
    field.text_offset = vec3_new(2.0f, 0.2f, 2.5f);
    strcpy(field.text, "32");
    field.strMark = strlen(field.text);
    wxGroupPush(&group, &field, WIDGET_FIELD);
    
    wxSlider slider = wxSliderCreate(vec2_new(xscale - 40.0f, 32.0f), 1.0f, true);
    wxGroupPush(&group, &slider, WIDGET_SLIDER);
    slider = wxSliderCreate(vec2_new(xscale - 30.0f, 32.0f), 1.0f, true);
    wxGroupPush(&group, &slider, WIDGET_SLIDER);
    slider = wxSliderCreate(vec2_new(xscale - 20.0f, 32.0f), 1.0f, true);
    wxGroupPush(&group, &slider, WIDGET_SLIDER);
    slider = wxSliderCreate(vec2_new(xscale - 10.0f, 32.0f), 1.0f, true);
    wxGroupPush(&group, &slider, WIDGET_SLIDER);
    slider = wxSliderCreate(vec2_new(xscale - 10.0f, 96.0f), 1.0f, true);
    wxGroupPush(&group, &slider, WIDGET_SLIDER);
    slider = wxSliderCreate(vec2_new(xscale - 50.0f, 32.0f), 1.0f, true);
    wxGroupPush(&group, &slider, WIDGET_SLIDER);
    
    wxRect rect = wxRectCreate(rect_new(xscale - 32.0f, yscale - 16.0f, 16.0f, 16.0f), vec4_new(0.0f, 0.0f, 0.0f, 1.0f));
    wxGroupPush(&group, &rect, WIDGET_RECT);
    rect = wxRectCreate(rect_new(xscale - 16.0f, yscale - 16.0f, 16.0f, 16.0f), vec4_new(1.0f, 0.0f, 0.0f, 1.0f));
    wxGroupPush(&group, &rect, WIDGET_RECT);
    rect = wxRectCreate(rect_new(xscale - 32.0f, yscale - 32.0f, 16.0f, 16.0f), vec4_new(0.0f, 1.0f, 0.0f, 1.0f));
    wxGroupPush(&group, &rect, WIDGET_RECT);
    rect = wxRectCreate(rect_new(xscale - 16.0f, yscale - 32.0f, 16.0f, 16.0f), vec4_new(0.0f, 0.0f, 1.0f, 1.0f));
    wxGroupPush(&group, &rect, WIDGET_RECT);
    rect = wxRectCreate(rect_new(xscale - 32.0f, yscale - 48.0f, 16.0f, 16.0f), vec4_new(1.0f, 1.0f, 0.0f, 1.0f));
    wxGroupPush(&group, &rect, WIDGET_RECT);
    rect = wxRectCreate(rect_new(xscale - 16.0f, yscale - 48.0f, 16.0f, 16.0f), vec4_new(0.0f, 1.0f, 1.0f, 1.0f));
    wxGroupPush(&group, &rect, WIDGET_RECT);
    rect = wxRectCreate(rect_new(xscale - 32.0f, yscale - 64.0f, 16.0f, 16.0f), vec4_new(1.0f, 0.0f, 1.0f, 1.0f));
    wxGroupPush(&group, &rect, WIDGET_RECT);
    rect = wxRectCreate(rect_new(xscale - 16.0f, yscale - 64.0f, 16.0f, 16.0f), vec4_new(1.0f, 1.0f, 1.0f, 1.0f));
    wxGroupPush(&group, &rect, WIDGET_RECT);
    rect = wxRectCreate(rect_new(16.0f, 16.0f, 16.0f, 16.0f), vec4_uni(0.0f));
    wxGroupPush(&group, &rect, WIDGET_RECT);

    wxIcon icon = wxIconCreate(*assetsGetTexture(TEXTURE_BRUSH), vec2_new(xscale * 0.5 - 32.0f, yscale - 32.0f), 1.0f);
    wxGroupPush(&group, &icon, WIDGET_WX_ICON);
    icon = wxIconCreate(*assetsGetTexture(TEXTURE_ERASER), vec2_new(xscale * 0.5 - 16.0f, yscale - 32.0f), 1.0f);
    wxGroupPush(&group, &icon, WIDGET_WX_ICON);
    icon = wxIconCreate(*assetsGetTexture(TEXTURE_TAR), vec2_new(xscale * 0.5 + 16.0f, yscale - 32.0f), 1.0f);
    wxGroupPush(&group, &icon, WIDGET_WX_ICON);
    icon = wxIconCreate(*assetsGetTexture(TEXTURE_DROPPICKER), vec2_new(xscale * 0.5 + 32.0f, yscale - 32.0f), 1.0f);
    wxGroupPush(&group, &icon, WIDGET_WX_ICON);
    
    return group;
}

static wxGroup optionsGroup(float yscale)
{
    wxGroup group = wxGroupCreate();
    wxTitle title = wxTitleCreate("Options", vec3_new(4.0f, 185.0f, 1.0f), vec4_new(1.0f, 0.0f, 0.0f, 1.0f));
    wxGroupPush(&group, &title, WIDGET_TITLE);
    title = wxTitleCreate("Mouse", vec3_new(85.0f, 110.0f, 0.8f), vec4_new(1.0f, 0.0f, 0.0f, 1.0f));
    wxGroupPush(&group, &title, WIDGET_TITLE);
    title = wxTitleCreate("Fullscreen", vec3_new(75.0f, 70.0f, 0.8f), vec4_new(1.0f, 0.0f, 0.0f, 1.0f));
    wxGroupPush(&group, &title, WIDGET_TITLE);

    wxButton button = wxButtonCreate("Menu", rect_new(32.0f, yscale - 32.0f, 50.0f, 15.0f));
    button.text_offset = vec3_new(2.0f, 2.0f, 2.5f);
    wxGroupPush(&group, &button, WIDGET_BUTTON);

    wxSwitch sw = wxSwitchCreate(vec2_new(160.0f, 115.0f), 1.0f);
    wxGroupPush(&group, &sw, WIDGET_SWITCH);
    sw = wxSwitchCreate(vec2_new(160.0f, 75.0f), 1.0f);
    wxGroupPush(&group, &sw, WIDGET_SWITCH);
    return group;
}

static wxGroup netMenuGroup(float xscale, float yscale)
{

    wxGroup group = wxGroupCreate();
    wxTitle title = wxTitleCreate("Online", vec3_new(4.0f, 185.0f, 1.0f), vec4_new(1.0f, 0.0f, 0.0f, 1.0f));
    wxGroupPush(&group, &title, WIDGET_TITLE);
    title = wxTitleCreate("Enter Server's IP:", vec3_new(80.0f, yscale * 0.5f + 48.0f, 0.8f), vec4_new(1.0f, 0.0f, 0.0f, 1.0f));
    wxGroupPush(&group, &title, WIDGET_TITLE);
    title = wxTitleCreate("Enter User Name:", vec3_new(80.0f, yscale * 0.5f - 16.0f, 0.8f), vec4_new(1.0f, 0.0f, 0.0f, 1.0f));
    wxGroupPush(&group, &title, WIDGET_TITLE);

    wxField field = wxFieldCreate(rect_new(xscale * 0.5f, yscale * 0.5f + 32.0f, 154.0f, 16.0f), 15);
    field.text_offset = vec3_new(2.0f, 2.0f, 2.5f);
    wxGroupPush(&group, &field, WIDGET_FIELD);
    field = wxFieldCreate(rect_new(xscale * 0.5f, yscale * 0.5f - 32.0f, 154.0f, 16.0f), 15);
    field.text_offset = vec3_new(2.0f, 2.0f, 2.5f);
    wxGroupPush(&group, &field, WIDGET_FIELD);

    wxButton button = wxButtonCreate("Menu", rect_new(32.0f, yscale - 32.0f, 50.0f, 15.0f));
    button.text_offset = vec3_new(2.0f, 2.0f, 2.5f);
    wxGroupPush(&group, &button, WIDGET_BUTTON);
    button = wxButtonCreate("Connect", rect_new(xscale * 0.5, 16.0f, 80.0f, 16.0f));
    button.text_offset = vec3_new(2.0f, 2.0f, 2.5f);
    wxGroupPush(&group, &button, WIDGET_BUTTON);
    return group;
}

static wxGroup levelEditorGroup(float xscale, float yscale)
{
    wxGroup group = wxGroupCreate();

    wxTitle title = wxTitleCreate("Level Editor Mode", vec3_new(4.0f, 185.0f, 1.0f), vec4_new(1.0f, 0.0f, 0.0f, 1.0f));
    wxGroupPush(&group, &title, WIDGET_TITLE);
    title = wxTitleCreate("Zoom", vec3_new(xscale - 48.0f, 90.0f, 0.8f), vec4_new(0.5f, 0.5f, 0.5f, 1.0f));
    wxGroupPush(&group, &title, WIDGET_TITLE);
    title = wxTitleCreate("Width", vec3_new(17.0f, 28.0f, 0.6f), vec4_new(1.0f, 0.0f, 0.0f, 1.0f));
    wxGroupPush(&group, &title, WIDGET_TITLE);
    title = wxTitleCreate("Height", vec3_new(57.0f, 28.0f, 0.6f), vec4_new(1.0f, 0.0f, 0.0f, 1.0f));
    wxGroupPush(&group, &title, WIDGET_TITLE);
    title = wxTitleCreate("Rough", vec3_new(97.0f, 28.0f, 0.6f), vec4_new(1.0f, 0.0f, 0.0f, 1.0f));
    wxGroupPush(&group, &title, WIDGET_TITLE);
    title = wxTitleCreate("Noise", vec3_new(137.0f, 28.0f, 0.6f), vec4_new(1.0f, 0.0f, 0.0f, 1.0f));
    wxGroupPush(&group, &title, WIDGET_TITLE);
    title = wxTitleCreate("Smooth", vec3_new(177.0f, 28.0f, 0.6f), vec4_new(1.0f, 0.0f, 0.0f, 1.0f));
    wxGroupPush(&group, &title, WIDGET_TITLE);
    title = wxTitleCreate("Items", vec3_new(217.0f, 28.0f, 0.6f), vec4_new(1.0f, 0.0f, 0.0f, 1.0f));
    wxGroupPush(&group, &title, WIDGET_TITLE);
    title = wxTitleCreate("Seed", vec3_new(xscale - 24.0f, yscale - 16.0f, 0.6f), vec4_new(1.0f, 0.0f, 0.0f, 1.0f));
    wxGroupPush(&group, &title, WIDGET_TITLE);

    wxButton button = wxButtonCreate("Menu", rect_new(32.0f, yscale - 32.0f, 50.0f, 15.0f));
    button.text_offset = vec3_new(2.0f, 2.0f, 2.5f);
    wxGroupPush(&group, &button, WIDGET_BUTTON);
    button = wxButtonCreate("Save", rect_new(32.0f, yscale - 52.0f, 50.0f, 15.0f));
    button.text_offset = vec3_new(2.0f, 2.0f, 2.5f);
    wxGroupPush(&group, &button, WIDGET_BUTTON);
    button = wxButtonCreate("Load", rect_new(32.0f, yscale - 72.0f, 50.0f, 15.0f));
    button.text_offset = vec3_new(2.0f, 2.0f, 2.5f);
    wxGroupPush(&group, &button, WIDGET_BUTTON);
    button = wxButtonCreate("New", rect_new(32.0f, yscale - 92.0f, 50.0f, 15.0f));
    button.text_offset = vec3_new(2.0f, 2.0f, 2.5f);
    wxGroupPush(&group, &button, WIDGET_BUTTON);
    button = wxButtonCreate("Rand", rect_new(32.0f, yscale - 112.0f, 50.0f, 15.0f));
    button.text_offset = vec3_new(2.0f, 2.0f, 2.5f);
    wxGroupPush(&group, &button, WIDGET_BUTTON);
    button = wxButtonCreate("Play", rect_new(32.0f, yscale - 132.0f, 50.0f, 15.0f));
    button.text_offset = vec3_new(2.0f, 2.0f, 2.5f);
    wxGroupPush(&group, &button, WIDGET_BUTTON);

    wxField field = wxFieldCreate(rect_new(32.0f, 16.0f, 32.0f, 15.0f), 3);
    field.text_offset = vec3_new(2.0f, 2.0f, 2.5f);
    strcpy(field.text, "30");
    field.strMark = strlen(field.text);
    wxGroupPush(&group, &field, WIDGET_FIELD);
    field = wxFieldCreate(rect_new(72.0f, 16.0f, 32.0f, 15.0f), 3);
    field.text_offset = vec3_new(2.0f, 2.0f, 2.5f);
    strcpy(field.text, "20");
    field.strMark = strlen(field.text);
    wxGroupPush(&group, &field, WIDGET_FIELD);
    field = wxFieldCreate(rect_new(112.0f, 16.0f, 32.0f, 15.0f), 3);
    field.text_offset = vec3_new(2.0f, 2.0f, 2.5f);
    strcpy(field.text, "5");
    field.strMark = strlen(field.text);
    wxGroupPush(&group, &field, WIDGET_FIELD);
    field = wxFieldCreate(rect_new(152.0f, 16.0f, 32.0f, 15.0f), 3);
    field.text_offset = vec3_new(2.0f, 2.0f, 2.5f);
    strcpy(field.text, "40");
    field.strMark = strlen(field.text);
    wxGroupPush(&group, &field, WIDGET_FIELD);
    field = wxFieldCreate(rect_new(192.0f, 16.0f, 32.0f, 15.0f), 3);
    field.text_offset = vec3_new(2.0f, 2.0f, 2.5f);
    strcpy(field.text, "2");
    field.strMark = strlen(field.text);
    wxGroupPush(&group, &field, WIDGET_FIELD);
    field = wxFieldCreate(rect_new(232.0f, 16.0f, 32.0f, 15.0f), 3);
    field.text_offset = vec3_new(2.0f, 2.0f, 2.5f);
    strcpy(field.text, "25");
    field.strMark = strlen(field.text);
    wxGroupPush(&group, &field, WIDGET_FIELD);
    field = wxFieldCreate(rect_new(xscale - 48.0f, yscale - 28.0f, 70.0f, 10.0f), 10);
    field.text_offset = vec3_new(1.0f, 1.0f, 2.5f);
    sprintf(field.text, "%u", randSeed);
    field.strMark = strlen(field.text);
    wxGroupPush(&group, &field, WIDGET_FIELD);

    wxSlider slider = wxSliderCreate(vec2_new(xscale - 10.0f, 96.0f), 1.0f, true);
    wxGroupPush(&group, &slider, WIDGET_SLIDER);
    return group;
}

static wxGroup UIeditorGroup(float xscale, float yscale)
{
    wxGroup group = wxGroupCreate();
    wxTitle title = wxTitleCreate("UI Editor Mode", vec3_new(4.0f, 185.0f, 0.8f), vec4_new(0.3f, 1.0f, 0.3f, 1.0f));
    wxGroupPush(&group, &title, WIDGET_TITLE);
    
    wxButton button = wxButtonCreate("Menu", rect_new(32.0f, yscale - 32.0f, 50.0f, 15.0f));
    button.text_offset = vec3_new(2.0f, 2.0f, 2.5f);
    wxGroupPush(&group, &button, WIDGET_BUTTON);
    button = wxButtonCreate("Save", rect_new(32.0f, yscale - 52.0f, 50.0f, 15.0f));
    button.text_offset = vec3_new(2.0f, 2.0f, 2.5f);
    wxGroupPush(&group, &button, WIDGET_BUTTON);
    button = wxButtonCreate("Load", rect_new(32.0f, yscale - 72.0f, 50.0f, 15.0f));
    button.text_offset = vec3_new(2.0f, 2.0f, 2.5f);
    wxGroupPush(&group, &button, WIDGET_BUTTON);
    button = wxButtonCreate("Prev", rect_new(32.0f, 16.0f, 50.0f, 15.0f));
    button.text_offset = vec3_new(2.0f, 2.0f, 2.5f);
    wxGroupPush(&group, &button, WIDGET_BUTTON);
    button = wxButtonCreate("Next", rect_new(xscale - 32.0f, 16.0f, 50.0f, 15.0f));
    button.text_offset = vec3_new(2.0f, 2.0f, 2.5f);
    wxGroupPush(&group, &button, WIDGET_BUTTON);
    return group;
}

static wxGroup metaGroup(float xscale, float yscale)
{
    wxGroup group = wxGroupCreate();

    float x = xscale * 0.5f, y = yscale * 0.5f;
    wxButton button = wxButtonCreate("Prev", rect_new(x - 32.0f, y, 50.0f, 15.0f));
    button.text_offset = vec3_new(2.0f, 2.0f, 2.5f);
    wxGroupPush(&group, &button, WIDGET_BUTTON);
    button = wxButtonCreate("Next", rect_new(x + 32.0f, y, 50.0f, 15.0f));
    button.text_offset = vec3_new(2.0f, 2.0f, 2.5f);
    wxGroupPush(&group, &button, WIDGET_BUTTON);
    return group;
}

static wxGroup gameGroup(float xscale, float yscale)
{
    wxGroup group = wxGroupCreate();
    wxTitle title = wxTitleCreate("Game Mode", vec3_new(4.0f, 185.0f, 1.0f), vec4_new(1.0f, 0.0f, 0.0f, 1.0f));
    wxGroupPush(&group, &title, WIDGET_TITLE);

    wxRect rect = wxRectCreate(rect_new(64.0f, yscale - 24.0f, 50.0f, 8.0f), vec4_new(1.0f, 0.0f, 0.0f, 1.0f));
    wxGroupPush(&group, &rect, WIDGET_RECT);

    Icon icon = iconCreate(*assetsGetTexture(TEXTURE_GUN), vec2_new(xscale - 16.0f, yscale - 16.0f), 1.0f, 0.0f);
    wxGroupPush(&group, &icon, WIDGET_ICON);
    icon = iconCreate(*assetsGetTexture(TEXTURE_JETPACK), vec2_new(xscale - 16.0f, yscale - 32.0f), 1.0f, 0.0f);
    wxGroupPush(&group, &icon, WIDGET_ICON);
    icon = iconCreate(*assetsGetTexture(TEXTURE_GRANADE), vec2_new(xscale - 16.0f, yscale - 48.0f), 1.0f, 0.0f);
    wxGroupPush(&group, &icon, WIDGET_ICON);
    icon = iconCreate(*assetsGetTexture(TEXTURE_HEART), vec2_new(16.0f, yscale - 32.0f), 1.0f, 0.0f);
    wxGroupPush(&group, &icon, WIDGET_ICON);
    return group;
}

static wxDirectory UIassetsDirectory()
{
    float xscale = viewport.x / viewport.z;
    float yscale = viewport.y / viewport.z;

    wxDirectory dir = wxDirectoryCreate();
    wxGroup group;

    group = mainMenuGroup(xscale);
    wxDirectoryPushGroup(&dir, &group);

    group = gameGroup(xscale, yscale);
    wxDirectoryPushGroup(&dir, &group);

    group = levelEditorGroup(xscale, yscale);
    wxDirectoryPushGroup(&dir, &group);

    group = UIeditorGroup(xscale, yscale);
    wxDirectoryPushGroup(&dir, &group);

    group = spriteEditorGroup(xscale, yscale);
    wxDirectoryPushGroup(&dir, &group);

    group = optionsGroup(yscale);
    wxDirectoryPushGroup(&dir, &group);

    group = netMenuGroup(xscale, yscale);
    wxDirectoryPushGroup(&dir, &group);

    group = metaGroup(xscale, yscale);
    wxDirectoryPushGroup(&dir, &group);

    dir.selected = WX_DIR_MAIN_MENU;
    return dir;
}

void UIassetsLoad()
{
    universeUI();
    //wxDir = wxDirectoryLoad(FILE_MENU_UI_SAVE);
    wxDir = UIassetsDirectory();
}