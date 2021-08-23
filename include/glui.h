#ifndef GLUI_GLEE_EXT_H
#define GLUI_GLEE_EXT_H

#ifdef __cplusplus
extern "C" {
#endif

/*****************
 * glui OpenGL GUI
 * **************/


#include <gleex.h>
#include <photon.h>

typedef struct Theme {
    vec4 primary[3], secondary[3], terciary[3];
} Theme;

typedef struct Font {
    unsigned int id;
    ivec2 size;
    ivec2 bearing; 
    unsigned int advance;
} Font;

typedef struct Icon {
    texture_t texture;
    vec4 color;
    vec2 position;
    float scale, rotation;
} Icon;

typedef enum wxEnum {
    WIDGET_TITLE,
    WIDGET_RECT,
    WIDGET_ICON,
    WIDGET_FRAME,
    WIDGET_WX_ICON,
    WIDGET_BUTTON,
    WIDGET_SWITCH,
    WIDGET_SLIDER,
    WIDGET_FIELD,
    WIDGET_GROUP
} wxEnum;

typedef struct wxRect {
    Rect rect;
    vec4 color;
} wxRect;

typedef struct wxTitle {
    vec3 pos_scale;
    char string[128];
    vec4 color;
} wxTitle;

typedef enum wxState {
    WIDGET_UNSELECTED,
    WIDGET_HOVER,
    WIDGET_SELECTED
} wxState;

typedef struct {
    vec2 position;
    wxState state;
    float scale;
} wxFrame;

typedef struct {
    texture_t texture;
    wxFrame frame;
} wxIcon;

typedef struct wxSwitch {
    vec2 position;
    bool activated;
    float scale;
} wxSwitch;

typedef struct wxSlider {
    vec2 position;
    bool selected;
    float lerp;
    float scale;
    bool rot;
} wxSlider;

typedef struct wxButton {
    wxState state;
    Rect rect;
    vec3 text_offset;
    char text[128];
} wxButton;

typedef struct wxField {
    wxState state;
    Rect rect;
    vec3 text_offset;
    char text[256];
    unsigned int maxSize, strMark;
} wxField;

typedef struct wxPtr {
    wxEnum type;
    void* widget;
} wxPtr;

typedef struct wxGroup {
    wxPtr* widgets;
    unsigned int size, used;
} wxGroup;

typedef struct wxDirectory {
    wxGroup* groups;
    unsigned int size, used, selected;
} wxDirectory;

extern Font* globalFont;
extern texture_t tSwitch[2], tSlider[2], tFrame;
extern Theme globalTheme;
extern unsigned int widgetShader, fontShader, textureShader;
extern unsigned int fontVAO, fontVBO, widgetVAO, widgetVBO, textureVAO, textureVBO;
extern unsigned int scrWidth, scrHeight, scrScale, scrXscale, scrYscale;

void universeInit(unsigned int width, unsigned int height, unsigned int scale);
void universeThemeSet(vec4 primary[3], vec4 secondary[3], vec4 terciary[3]);
void universeSwitchSet(texture_t tActive, texture_t tInactive);
void universeFrameSet(texture_t frame);
void universeSliderSet(texture_t slider, texture_t marker);
void universeFontSet(Font* font);

Theme* universeThemeGet();
Font* universeFontGet();

size_t wxWidgetSize(wxEnum e);
void wxDirectorySave(const char* path, wxDirectory* dir);
wxDirectory wxDirectoryLoad(const char* path);

Font* fontLoad(const char* path, unsigned int size);
void fontDrawText(Font* font, const char* text, float x, float y, float scale, vec3 color);

Icon iconCreate(texture_t texture, vec2 position, float scale, float rotation);
void iconDraw(Icon* icon);

void bmpDraw(bmp_t* bmp, float xPos, float yPos, float scale);

wxRect wxRectCreate(Rect r, vec4 color);
void wxRectDraw(wxRect _rect);

wxTitle wxTitleCreate(const char* str, vec3 pos_scale, vec4 color);
void wxTitleDraw(wxTitle* title);

wxButton wxButtonCreate(const char* text, Rect rect);
void wxButtonDraw(wxButton* w);
void wxButtonUpdate(wxButton* w, vec2 mouse, bool pressed);

wxSwitch wxSwitchCreate(vec2 position, float scale);
void wxSwitchUpdate(wxSwitch* s, vec2 mouse, bool pressed);
void wxSwitchDraw(wxSwitch* s);

wxSlider wxSliderCreate(vec2 position, float scale, bool rot);
void wxSliderUpdate(wxSlider* s, vec2 mouse, bool pressed);
void wxSliderDraw(wxSlider* s);

wxField wxFieldCreate(Rect rect, unsigned int maxSize);
void wxFieldUpdate(wxField* field, vec2 mouse, bool pressed);
void wxFieldDraw(wxField* field);

wxFrame wxFrameCreate(vec2 position, float scale);
void wxFrameUpdate(wxFrame* f, vec2 mouse, bool pressed);
void wxFrameDraw(wxFrame* f);

wxIcon wxIconCreate(texture_t texture, vec2 position, float scale);
void wxIconUpdate(wxIcon* f, vec2 mouse, bool pressed);
void wxIconDraw(wxIcon* f);

wxGroup wxGroupCreate();
void wxGroupDraw(wxGroup* group);
void wxGroupUpdate(wxGroup* group, vec2 mouse, bool pressed, bool down);
void wxGroupPush(wxGroup* group, void* widget, wxEnum type);
void wxGroupFree(wxGroup* group);

wxDirectory wxDirectoryCreate();
void wxDirectoryPushGroup(wxDirectory* dir, wxGroup* group);
void wxDirectoryPushWidget(wxDirectory* dir, void* widget, wxEnum type);
void wxDirectoryFree(wxDirectory* dir);
void wxDirectoryDraw(wxDirectory* dir);
void wxDirectoryUpdate(wxDirectory* dir, vec2 mouse, bool pressed, bool down);
wxGroup* wxDirectoryGroup(wxDirectory* dir);
void wxDirectorySelect(wxDirectory* dir, unsigned int index);

#ifdef __cplusplus
}
#endif
#endif
