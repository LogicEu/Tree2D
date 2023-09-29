#ifndef TREE_FRAMEWORK_H
#define TREE_FRAMEWORK_H

#ifdef __cplusplus
extern "C" {
#endif

/********************
=====================
TREE FRAMEWORK HEADER
=====================
*********************/

#include <glui.h>
#include <mass.h>
#include <ethnic.h>
#include <nano.h>

#define FULLSCREEN 0
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define SCREEN_SCALE 3

#define SCREEN_XSCALE SCREEN_WIDTH / SCREEN_SCALE
#define SCREEN_YSCALE SCREEN_HEIGHT / SCREEN_SCALE

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#define KEY_MOD GLFW_KEY_LEFT_ALT
#else
#define KEY_MOD GLFW_KEY_LEFT_CONTROL
#endif

#define GRANADE_MAX 4

typedef enum {
    STATE_MENU,
    STATE_PLAY,
    STATE_PAUSE,
    STATE_LEVEL_EDITOR,
    STATE_SPRITE_EDITOR,
    STATE_UI_EDITOR,
    STATE_OPTIONS,
    STATE_NET_MENU,
    STATE_NET_PLAY,
    STATE_LOAD
} stateEnum;

typedef enum {
    ARCHETYPE_PLAYER,
    ARCHETYPE_TERRAIN,
    ARCHETYPE_GUN,
    ARCHETYPE_BULLET,
    ARCHETYPE_BLAST,
    ARCHETYPE_SMOKE,
    ARCHETYPE_SHADOW,
    ARCHETYPE_FIREBARREL,
    ARCHETYPE_JETPACK,
    ARCHETYPE_GRANADE,
    ARCHETYPE_BOX,
} archetypeEnum;

typedef enum assetEnum {
    ASSET_SHADER,
    ASSET_TEXTURE,
    ASSET_PARALLAX,
    ASSET_FONT,
    ASSET_FRAMEBUFFER,
    ASSET_SPRITE
} assetEnum;

typedef enum {
    SHADER_TEXTURE,
    SHADER_FONT,
    SHADER_COLOR,
    SHADER_FRAMEBUFFER_BYN,
    SHADER_MESH,
    SHADER_WX_FONT,
    SHADER_WX_COLOR,
    SHADER_WX_TEXTURE,
} shaderEnum;

typedef enum componentEnum {
    COMPONENT_PHI_RECT,
    COMPONENT_GL_RECT,
    COMPONENT_TEX_ID,
    COMPONENT_SPRITE_ID,
    COMPONENT_SPRITE_COLLECTION,
    COMPONENT_VEL_VEC2,
    COMPONENT_GRAVITY,
    COMPONENT_RIGID_COLLIDER,
    COMPONENT_GUN_CONTROLLER,
    COMPONENT_AMMO,
    COMPONENT_ROTATION,
    COMPONENT_SCALE,
    COMPONENT_ALPHA,
    COMPONENT_BULLET,
    COMPONENT_EXPLOTION,
    COMPONENT_SMOKE,
    COMPONENT_SHADOW,
    COMPONENT_FIREBARREL,
    COMPONENT_JETPACK,
    COMPONENT_GRANADE,
    COMPONENT_BOX,
} componentEnum;

typedef enum {
    TEXTURE_WHITE,
    TEXTURE_TILE,
    TEXTURE_TILE_ATLAS,
    TEXTURE_AIM,
    TEXTURE_BRUSH,
    TEXTURE_ERASER,
    TEXTURE_TAR,
    TEXTURE_DROPPICKER,
    TEXTURE_GUN,
    TEXTURE_SHOTGUN,
    TEXTURE_REDBALL,
    TEXTURE_EXPLOTION,
    TEXTURE_CLOUD,
    TEXTURE_SMOKE,
    TEXTURE_SMOKE2,
    TEXTURE_SMOKE3,
    TEXTURE_JETPACK,
    TEXTURE_GRANADE,
    TEXTURE_BLOOD,
    TEXTURE_BOX,
    TEXTURE_FIREBARREL,
    TEXTURE_MOUSE_CURSOR,
    TEXTURE_RIFLE,
    TEXTURE_MACHINEGUN,
    TEXTURE_FLAMETHROWER,
    TEXTURE_BAZOOKA,
    TEXTURE_ROCKET,
    TEXTURE_FIRE,
    TEXTURE_HEART,
    TEXTURE_MEDKIT,
    TEXTURE_AMMO
} textureEnum;

typedef enum {
    SPRITE_IDLE,
    SPRITE_RUNNING,
    SPRITE_JUMPING,
    SPRITE_FALLING,
    SPRITE_DEAD
} spriteAnimationEnum;

typedef struct {
    sprite_t idle, running, jumping, falling, dead;
} spriteCollection;

typedef enum {
    SPRITE_KID,
    SPRITE_CUSTOM
} spriteCollectionEnum;

typedef enum {
    PARALLAX_1,
    PARALLAX_2,
    PARALLAX_3
} parallaxEnum;

typedef enum {
    FONT_1
} fontEnum;

typedef enum {
    FRAMEBUFFER_BLACK_AND_WHITE
} framebufferEnum;

/******************/

typedef enum {
    GRANADE_COLLECTED,
    GRANADE_LOOSE,
    GRANADE_THROWED
} GranadeEnum;

typedef enum {
    GUN_STATE_LOOSE,
    GUN_STATE_COLLECTED,
    GUN_STATE_USED
} GunState;

typedef enum {
    GUN_KIND_GUN,
    GUN_KIND_SHOTGUN,
    GUN_KIND_RIFLE,
    GUN_KIND_MACHINEGUN,
    GUN_KIND_FLAMETHROWER,
    GUN_KIND_BAZOOKA
} GunKind;

typedef enum {
    BULLET_KIND_BALL,
    BULLET_KIND_ROCKET
} BulletKind;

typedef enum {
    JETPACK_LOOSE,
    JETPACK_USED,
    JETPACK_COLLECTED
} jetpackStateEnum;

typedef enum {
    BOX_STATE_NULL,
    BOX_STATE_LOOSE,
    BOX_STATE_OPEN
} BoxState;

typedef struct {
    unsigned int width, height;
    uint8_t* data;
} map_t;

typedef struct Atlas {
    bmp_t bmp;
    texture_t texture;
    unsigned int count;
} Atlas;

typedef struct {
    GranadeEnum state;
    float timer;
} GranadeComponent;

typedef struct {
    GunState state;
    GunKind kind;
    unsigned int ammo; 
    float offset;
    float latencyTimer, latencySpeed;
} GunType;

typedef struct Vertex {
    vec2 pos, uv;
} Vertex;

typedef struct Mesh {
    unsigned int id;
    unsigned int vbo;
    array_t* vertices;
} Mesh;

typedef enum {
    MAP_TILE_NULL,
    MAP_TILE_BLOCK,
    MAP_TILE_STATIC,
    MAP_TILE_GUN,
    MAP_TILE_SHOTGUN,
    MAP_TILE_RIFLE,
    MAP_TILE_MACHINEGUN,
    MAP_TILE_FLAMETHROWER,
    MAP_TILE_BAZOOKA,
    MAP_TILE_FIREBARREL,
    MAP_TILE_JETPACK,
    MAP_TILE_GRANADE
} tile_t;

/********************
=====================
TREE FRAMEWORK HEADER
=====================
*********************/

extern vec3 viewport;
extern unsigned int quadVAO;

void systemInit(unsigned int startState);
void systemStep(float deltaTime);
void systemSetState(unsigned int newState);
void systemExit();

Module moduleInit();

unsigned int shaderLoadTexture();
unsigned int shaderLoadFont();
unsigned int shaderLoadFramebuffer();
unsigned int shaderLoadColor();
unsigned int shaderLoadMesh();

void spriteCollectionSubmitCustom(bmp_t* bmp);
unsigned int spriteCollectionSubmit(bmp_t* bmp);

void assetsLoad();
void assetsFree();
texture_t* assetsGetTexture(unsigned int index);
texture_t* assetsGetParallax(unsigned int index);
unsigned int assetsGetShader(unsigned int index);
font_t* assetsGetFont(unsigned int index);
framebuffer_t* assetsGetFramebuffer(unsigned int index);
sprite_t* assetsGetSprite(unsigned int spriteIndex, unsigned int spriteState);

void treeInit();
void treeDirectoryReset();
void gameDirectoryReset();
void menuDirectoryReset();
void levelEditorDirectoryReset();
void spriteEditorDirectoryReset();
void UIeditorDirectoryReset();
void optionsDirectoryReset();
void netMenuDirectoryReset();

void treeNetInit(const char* username, const char* ip);
void treeNetExit();
void netGameStep(float deltaTime);

void gameStep(float deltaTime);
void gameInit();

Entity playerInit();
void playerReset();
void playerGameStep(float deltaTime);
void playerKill();

void levelEditorStep(float deltaTime);
void levelEditorInit();
void levelReset();

void menuStep();
void menuInit();

void netMenuInit();
void netMenuStep();

void optionsStep();
void optionsInit();

void spriteEditorInit();
void spriteEditorStep();

void UIeditorStep();
void UIeditorInit();

void loaderStep();

void UIassetsLoad();

/************
 * COMPONENTS
 * *********/

void gravityStep(float deltaTime);
void velocityStep(float deltaTime);

Entity gunCreate(vec2 position, GunKind gunKind);
void gunControllerStep(float deltaTime);
void gunPick(Entity gun);
void gunDrop(Entity gun);
void gunShoot(Entity gun);

void blastStep(float deltaTime);

void bulletStep(float deltaTime);

void smokeStep(float deltaTime);
void smokeEmit(vec2 position, unsigned int textureIndex);

void shadowStep(float deltaTime);
void shadowEmit(Entity e, float side);

void firebarrelStep();

void jetpackControllerStep();
void jetpackPick(Entity jetpack);
void jetpackDrop(Entity jetpack);
void jetpackUse(Entity jetpack);

void granadeStep(float deltaTime);
void granadeCollect(Entity granade);
void granadeDrop(Entity granade, vec2 pos);
void granadeThrow(Entity e, vec2 position, float rot);

void boxStep();
void boxOpen(Entity box);

bool checkRigidCollision(Entity entity, vec2 off);
Entity checkGunCollision(Entity entity);
Entity checkFirebarrelCollision(Entity entity);
Entity checkJetpackCollision(Entity entity);
Entity checkGranadeCollision(Entity entity);
unsigned int checkBlastCollision(Entity entity);
Entity checkPhiCollision(Entity entity, Component component);
Entity checkPhiScaledCollision(Entity entity, Component component);

void drawSetCamera(float* cam);
void drawParallax(vec4 cam);
void drawComponents();
void drawTextureColor(texture_t t, vec2 pos, vec4 c);
void drawTextureIdColor(unsigned int id, Rect rect, vec4 c);
void drawString(const char* text, font_t* font, float* color, float x, float y, float scale, float rot);
void drawStringCentered(const char* text, font_t* font, float* color, float x, float y, float scale, float rot);
void drawFramebuffer();
void drawRect(Rect rect, vec4 color);

void terrainReduce();
void terrainRecalcTextures();
void terrainRecalcSingleTexture(Entity e);
void terraingGenRand(unsigned int width, unsigned int height);

void map_save(const char* path, map_t map);
map_t map_load(const char* path);
map_t map_create(unsigned int w, unsigned int h);
void map_destroy(map_t m);
map_t map_from_module();
void module_from_map(map_t* map);
map_t map_generate(unsigned int w, unsigned int h, unsigned int stat, unsigned int block, unsigned int steps, unsigned int item);
vec2 map_spawn(map_t map);
uint8_t* map_tile(map_t m, unsigned int x, unsigned int y);

Mesh meshFromMap(map_t map);
void meshBind(Mesh* mesh);
void meshDestroy(Mesh* mesh);
void meshUpdate(const Mesh* mesh);
void meshAddQuad(Mesh* mesh, float x, float y, unsigned int atlasSize, unsigned int atlasIndex);
Mesh meshCreate();

Atlas atlasLoad(char** paths, unsigned int count);
void atlasFree(Atlas* atlas);

void archetypesInit();
Entity archetypePlayer(unsigned int sprite);
Entity archetypeTerrainTile(unsigned int texture_index, vec2 position);
Entity archetypeSmoke(vec2 position, unsigned int textureIndex);
Entity archetypeBlast(vec2 position, float explotionForce);
Entity archetypeFire(vec2 position, vec2 vel);
Entity archetypeBullet(vec2 position, vec2 vel, BulletKind kind);
Entity archetypeShadow(vec2 position, unsigned int sprite, float side);
Entity archetypeFirebarrel(vec2 position);
Entity archetypeJetpackController(vec2 position);
Entity archetypeGranade(vec2 position);
Entity archetypeBox(vec2 position);

#ifdef __cplusplus
}
#endif
#endif
