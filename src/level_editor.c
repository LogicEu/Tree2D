#include "Tree.h"
#include "UI/UIcommon.h"
#include <ctype.h>
#include <stdio.h>

#define FILE_ECS_MODULE "assets/data/module.bin"
#define FILE_MAP_MODULE "assets/data/world.map"
#define SCALE_MULT 4.0f

extern vec4 cam;
extern vec2 mouse;
extern wxDirectory wxDir;
extern Entity player;
extern unsigned int randSeed;
extern unsigned int currentPlayerSprite;

extern unsigned int usedWeapon;
extern unsigned int jetpack;
extern unsigned int granadeCount;

vec2 spawnPoint;
Mesh mesh;

static vec2 cursor;
static Entity selected;
static wxGroup* group;

static unsigned int mapWidth;
static unsigned int mapHeight;
static unsigned int mapStatic;
static unsigned int mapNoise;
static unsigned int mapSmoothStep;
static unsigned int mapItem;

typedef enum {
    WX_LE_TITLE_MAIN,
    WX_LE_TITLE_ZOOM,
    WX_LE_TITLE_WIDTH,
    WX_LE_TITTLE_HEIGHT,
    WX_LE_TITTLE_STATIC,
    WX_LE_TITTLE_NOISE,
    WX_LE_TITLE_SMOOTH,
    WX_LE_TITLE_ITEMS,
    WX_LE_TITLE_SEED,

    WX_LE_BUTTON_MENU,
    WX_LE_BUTTON_SAVE,
    WX_LE_BUTTON_LOAD,
    WX_LE_BUTTON_NEW,
    WX_LE_BUTTON_GEN,
    WX_LE_BUTTON_PLAY,

    WX_LE_FIELD_WIDTH,
    WX_LE_FIELD_HEIGHT,
    WX_LE_FIELD_STATIC,
    WX_LE_FIELD_NOISE,
    WX_LE_FIELD_SMOOTH,
    WX_LE_FIELD_ITEM,
    WX_LE_FIELD_SEED,

    WX_LE_SLIDER_ZOOM
} wxLevelEditorEnum;

typedef enum {
    ARCH_SWITCH_TILE,
    ARCH_SWITCH_GUN,
    ARCH_SWITCH_SHOTGUN,
    ARCH_SWITCH_RIFLE,
    ARCH_SWITCH_MACHINEGUN,
    ARCH_SWITCH_FLAMETHROWER,
    ARCH_SWITCH_BAZOOKA,
    ARCH_SWITCH_FIREBARREL,
    ARCH_SWITCH_JETPACK,
    ARCH_SWITCH_GRANADE,
    ARCH_SWITCH_BOX,
} archSwitchEnum;

static archSwitchEnum selectedIndex;

static void str_to_var(char* str, unsigned int* var)
{
    int i = atoi(str);
    if (i < 0) i = 0;
    else if (i > 100) i = 100;
    *var = (unsigned int)i;
    sprintf(str, "%u", *var);
}

static texture_t* textureGet()
{
    switch (selectedIndex) {
        case ARCH_SWITCH_TILE:
            return assetsGetTexture(TEXTURE_TILE);
        case ARCH_SWITCH_GUN:
            return assetsGetTexture(TEXTURE_GUN);
        case ARCH_SWITCH_SHOTGUN:
            return assetsGetTexture(TEXTURE_SHOTGUN);
        case ARCH_SWITCH_RIFLE:
            return assetsGetTexture(TEXTURE_RIFLE);
        case ARCH_SWITCH_MACHINEGUN:
            return assetsGetTexture(TEXTURE_MACHINEGUN);
        case ARCH_SWITCH_FLAMETHROWER:
            return assetsGetTexture(TEXTURE_FLAMETHROWER);
        case ARCH_SWITCH_BAZOOKA:
            return assetsGetTexture(TEXTURE_BAZOOKA);
        case ARCH_SWITCH_FIREBARREL:
            return assetsGetTexture(TEXTURE_FIREBARREL);
        case ARCH_SWITCH_JETPACK:
            return assetsGetTexture(TEXTURE_JETPACK);
        case ARCH_SWITCH_GRANADE:
            return assetsGetTexture(TEXTURE_GRANADE);
        case ARCH_SWITCH_BOX:
            return assetsGetTexture(TEXTURE_BOX);
        break;
    }
    return NULL;
}

static Entity entityCreate()
{
    switch (selectedIndex) {
        case ARCH_SWITCH_TILE: {
            Entity a = archetypeTerrainTile(TEXTURE_TILE, cursor);
            //terrainRecalcSingleTexture(a);
            return a;
        }
        case ARCH_SWITCH_GUN:
            return gunCreate(cursor, GUN_KIND_GUN);
        case ARCH_SWITCH_SHOTGUN:
            return gunCreate(cursor, GUN_KIND_SHOTGUN);
        case ARCH_SWITCH_RIFLE:
            return gunCreate(cursor, GUN_KIND_RIFLE);
        case ARCH_SWITCH_MACHINEGUN:
            return gunCreate(cursor, GUN_KIND_MACHINEGUN);
        case ARCH_SWITCH_FLAMETHROWER:
            return gunCreate(cursor, GUN_KIND_FLAMETHROWER);
        case ARCH_SWITCH_BAZOOKA:
            return gunCreate(cursor, GUN_KIND_BAZOOKA);
        case ARCH_SWITCH_FIREBARREL:
            return archetypeFirebarrel(cursor);
        case ARCH_SWITCH_JETPACK:
            return archetypeJetpackController(cursor);
        case ARCH_SWITCH_GRANADE:
            return archetypeGranade(cursor);
        case ARCH_SWITCH_BOX:
            return archetypeBox(cursor);
        break;
    }
    return 0;
}

void levelGenerate()
{
    levelReset();

    map_t map = map_generate(mapWidth, mapHeight, mapStatic, mapNoise, mapSmoothStep, mapItem);
    module_from_map(&map);
    meshDestroy(&mesh);
    mesh = meshFromMap(map);

    spawnPoint = map_spawn(map);
    while (spawnPoint.x < 40.0f && spawnPoint.y < 40.0f) {
        spawnPoint = map_spawn(map);
    }

    map_destroy(map);
    playerReset();
    cam.x = spawnPoint.x - (viewport.x / viewport.z) * 0.5;
    cam.y = spawnPoint.y - (viewport.y / viewport.z) * 0.5;
}

static void getMouseInput()
{
    const float tileSize = 32.0f;
    vec2 center = {(viewport.x / viewport.z) * 0.5f, (viewport.y / viewport.z) * 0.5f};

    bool mouseDown = glee_mouse_down(GLFW_MOUSE_BUTTON_LEFT);
    bool mousePressed = glee_mouse_pressed(GLFW_MOUSE_BUTTON_LEFT);
    
    wxButton* button = group->widgets[WX_LE_BUTTON_SAVE].widget;
    if (button->state == WIDGET_HOVER && mousePressed) {
        module_save(FILE_ECS_MODULE, module_current());
        return;
    }
    button = group->widgets[WX_LE_BUTTON_LOAD].widget;
    if (button->state == WIDGET_HOVER && mousePressed) {
        module_destroy(module_current());
        module_load(FILE_ECS_MODULE);
        Rect r = *(Rect*)entity_get(player, COMPONENT_GL_RECT);
        cam.x = r.x - center.x;
        cam.y = r.y - center.y;
        cam.z = 1.0f;
        return;
    }
    button = group->widgets[WX_LE_BUTTON_NEW].widget;
    if (button->state == WIDGET_HOVER && mousePressed) {
        levelReset();
        return;
    }
    button = group->widgets[WX_LE_BUTTON_GEN].widget;
    if (button->state == WIDGET_HOVER && mousePressed) {
        levelGenerate();
        return;
    }
    button = group->widgets[WX_LE_BUTTON_PLAY].widget;
    if (button->state == WIDGET_HOVER && mousePressed) {
        systemSetState(STATE_PLAY);
        return;
    }
    
    wxGroupUpdate(group, mouse, mousePressed, mouseDown);

    mouse = vec2_add(vec2_add(vec2_div(vec2_sub(mouse, center), cam.z), center), *(vec2*)&cam);
    cursor = vec2_add(vec2_mult(ivec2_to_vec2(vec2_to_ivec2(vec2_div(mouse, tileSize))), tileSize), vec2_uni(tileSize * 0.5f));

    button = group->widgets[WX_LE_BUTTON_MENU].widget;
    if (button->state == WIDGET_SELECTED) {
        systemSetState(STATE_MENU);
        return;
    }

    wxField* field = group->widgets[WX_LE_FIELD_WIDTH].widget;
    if (field->state == WIDGET_SELECTED) {
        str_to_var(field->text, &mapWidth);
        return;
    }
    field = group->widgets[WX_LE_FIELD_HEIGHT].widget;
    if (field->state == WIDGET_SELECTED) {
        str_to_var(field->text, &mapHeight);
        return;
    }
    field = group->widgets[WX_LE_FIELD_STATIC].widget;
    if (field->state == WIDGET_SELECTED) {
        str_to_var(field->text, &mapStatic);
        return;
    }
    field = group->widgets[WX_LE_FIELD_NOISE].widget;
    if (field->state == WIDGET_SELECTED) {
        str_to_var(field->text, &mapNoise);
        return;
    }
    field = group->widgets[WX_LE_FIELD_SMOOTH].widget;
    if (field->state == WIDGET_SELECTED) {
        str_to_var(field->text, &mapSmoothStep);
        return;
    }
    field = group->widgets[WX_LE_FIELD_ITEM].widget;
    if (field->state == WIDGET_SELECTED) {
        str_to_var(field->text, &mapItem);
        return;
    }
    field = group->widgets[WX_LE_FIELD_SEED].widget;
    if (field->state == WIDGET_SELECTED) {
        sscanf(field->text, "%u", &randSeed);
        srand(randSeed);
        rand_seed(randSeed);
        return;
    }

    wxSlider* slider = group->widgets[WX_LE_SLIDER_ZOOM].widget;
    if (slider->selected) {
        cam.z = 0.6f + slider->lerp * SCALE_MULT;
        return;
    }

    Entity hover = 0;
    for (Entity e = 1; e < entity_count(); e++) {
        Rect* r = (Rect*)entity_get(e, COMPONENT_PHI_RECT);
        if (!r) continue;
        if (rect_point_overlap(cursor, *r)) {
            hover = e;
            break;
        }
    }

    if (hover) {
        glUseProgram(assetsGetShader(SHADER_TEXTURE));
        drawRect(*(Rect*)entity_get(hover, COMPONENT_GL_RECT), vec4_uni(0.5f));
        if (mousePressed) {
            selected = hover;
        }
    } else if (mousePressed) {
        entityCreate();
    }

    if (selected) {
        Rect* texRect = (Rect*)entity_get(selected, COMPONENT_GL_RECT);
        Rect r = {cursor.x, cursor.y, texRect->w, texRect->h};
        entity_set(selected, COMPONENT_PHI_RECT, &r);
        entity_set(selected, COMPONENT_GL_RECT, &r);
        if (glee_key_pressed(GLFW_KEY_BACKSPACE)) {
            entity_destroy(selected);
            selected = 0;
        }
        if (glee_mouse_released(GLFW_MOUSE_BUTTON_LEFT)) selected = 0;
    }
}

static void getKeyboardInput(float deltaTime)
{
    static const float camSpeed = 50.0f;

    if (glee_key_pressed(GLFW_KEY_ESCAPE)) {
        systemSetState(STATE_MENU);
        return;
    }
    if (glee_key_pressed(GLFW_KEY_L)) {
        levelReset();
        map_t m = map_load(FILE_MAP_MODULE);
        module_from_map(&m);
        map_destroy(m);
    }
    if (glee_key_pressed(GLFW_KEY_O)) {
        levelGenerate();
    }
    if (glee_key_down(KEY_MOD) && glee_key_pressed(GLFW_KEY_P)) {
        entity_destroy(player);
        module_save(FILE_ECS_MODULE, module_current());
        player = archetypePlayer(currentPlayerSprite);
        printf("Without character!\n");
    }
    if (glee_key_pressed(GLFW_KEY_P)) {
        map_t m = map_from_module();
        map_save(FILE_MAP_MODULE, m);
        map_destroy(m);
    }
    if (glee_key_pressed(GLFW_KEY_R)) {
        levelReset();
    }
    if (glee_key_pressed(GLFW_KEY_C)) {
        if (selectedIndex < ARCH_SWITCH_BOX) selectedIndex ++;
        else selectedIndex = ARCH_SWITCH_TILE;
    }
    if (glee_key_pressed(GLFW_KEY_V)) {
        if (selectedIndex > ARCH_SWITCH_TILE) selectedIndex --;
        else selectedIndex = ARCH_SWITCH_BOX;
    }
    if (glee_key_down(GLFW_KEY_D)) cam.x += deltaTime * camSpeed;
    if (glee_key_down(GLFW_KEY_A)) cam.x -= deltaTime * camSpeed;
    if (glee_key_down(GLFW_KEY_W)) cam.y += deltaTime * camSpeed;
    if (glee_key_down(GLFW_KEY_S)) cam.y -= deltaTime * camSpeed;
    if (glee_key_down(GLFW_KEY_Z)) cam.z = clampf(cam.z + deltaTime, 0.01f, 10.0f);
    if (glee_key_down(GLFW_KEY_X)) cam.z = clampf(cam.z - deltaTime, 0.01f, 10.0f);
}

static void levelEditorDraw()
{
    static vec4 alpha = {1.0f, 1.0f, 1.0f, 0.5f};

    wxGroupDraw(group);

    glBindVertexArray(quadVAO);
    glUseProgram(assetsGetShader(SHADER_TEXTURE));
    if (!selected) drawTextureColor(*textureGet(), cursor, alpha);
    drawTextureColor(*assetsGetTexture(TEXTURE_MOUSE_CURSOR), vec2_new(mouse.x + 4.0f, mouse.y - 4.0f), vec4_uni(1.0f));
}

static void getInput(float deltaTime)
{
    getKeyboardInput(deltaTime);
    getMouseInput();
}

void levelEditorStep(float deltaTime)
{
    getInput(deltaTime);
    levelEditorDraw();
}

void levelEditorDirectoryReset()
{
    group = &wxDir.groups[WX_DIR_LEVEL_EDITOR];
}

void levelReset()
{
    module_destroy(module_current());
    moduleInit();
    player = archetypePlayer(currentPlayerSprite);
    usedWeapon = 0;
    jetpack = 0;
    granadeCount = 0;
    playerReset();
}

void levelEditorInit()
{
    levelEditorDirectoryReset();
    selectedIndex = ARCH_SWITCH_TILE;
    selected = 0;

    wxField* field = group->widgets[WX_LE_FIELD_WIDTH].widget;
    str_to_var(field->text, &mapWidth);
    field = group->widgets[WX_LE_FIELD_HEIGHT].widget;
    str_to_var(field->text, &mapHeight);
    field = group->widgets[WX_LE_FIELD_STATIC].widget;
    str_to_var(field->text, &mapStatic);
    field = group->widgets[WX_LE_FIELD_NOISE].widget;
    str_to_var(field->text, &mapNoise);
    field = group->widgets[WX_LE_FIELD_SMOOTH].widget;
    str_to_var(field->text, &mapSmoothStep);
    field = group->widgets[WX_LE_FIELD_ITEM].widget;
    str_to_var(field->text, &mapItem);

    mesh = meshCreate();
    levelGenerate();
}