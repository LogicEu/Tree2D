#include "Tree.h"
#include <stdio.h>

unsigned int hp = 100;
bool stateWallSliding = false;
bool stateDashing = false;
bool stateJetpacking = false;

Entity usedWeapon = 0;
Entity jetpack = 0;
Entity granades[GRANADE_MAX];
unsigned int granadeCount = 0;

extern unsigned int currentPlayerSprite;
extern Entity player;
extern vec4 cam;
extern vec2 mouse;
extern bool blackAndWhite;
extern vec2 spawnPoint;
extern unsigned int currentPlayerSprite;
extern unsigned int playerLives;

extern Entity netUsedWeapon;
extern Entity netJetpack;
extern Entity netGranadePick;
extern Entity netGranadeThrow;
extern queue_t* netGranadeDrop;

extern void cameraTriggerAlarm();

static void playerDrawGUI()
{
    static vec4 red = {1.0f, 0.0f, 0.0f, 0.8f};
    static vec4 white = {1.0f, 1.0f, 1.0f, 1.0f};
    static float camera[] = {0.0f, 0.0f, 1.0f, 0.0f};

    char string[16];
    sprintf(string, "%u", hp);
    glUseProgram(assetsGetShader(SHADER_FONT));
    drawStringCentered(
        string, 
        assetsGetFont(FONT_1), 
        &red.x,
        -8.0f,
        (viewport.y / viewport.z) - 50.0f, 
        0.5f, 
        0.0f
    );

    if (granadeCount) {
        glUseProgram(assetsGetShader(SHADER_TEXTURE));
        texture_t* t = assetsGetTexture(TEXTURE_GRANADE);
        vec2 v = {(viewport.x / viewport.z) * 0.5f + 90.0f, (viewport.y / viewport.z) - 36.0f};
        sprintf(string, "%u", granadeCount);
        glee_shader_uniform_set(assetsGetShader(SHADER_TEXTURE), 4, "camera", &camera[0]);
        drawTextureColor(*t, v, white);
        glee_shader_uniform_set(assetsGetShader(SHADER_TEXTURE), 4, "camera", &cam.x);

        glUseProgram(assetsGetShader(SHADER_FONT));
        drawStringCentered(
            string, 
            assetsGetFont(FONT_1), 
            &red.x,
            (viewport.x / viewport.z) * 0.5 + 98.0f,
            (viewport.y / viewport.z) - 70.0f, 
            0.5f, 
            0.0f
        );
    }

    if (usedWeapon) {
        glUseProgram(assetsGetShader(SHADER_TEXTURE));
        Rect r = *(Rect*)entity_get(usedWeapon, COMPONENT_GL_RECT);
        if (r.w < 0.0f) r.w *= -1.0f;
        if (r.h < 0.0f) r.h *= -1.0f;
        Rect v = {(viewport.x / viewport.z) * 0.5f + 90.0f, (viewport.y / viewport.z) - 24.0f, r.w, r.h};
        unsigned int ammo = ((GunType*)entity_get(usedWeapon, COMPONENT_GUN_CONTROLLER))->ammo;
        sprintf(string, "%u", ammo);
        glee_shader_uniform_set(assetsGetShader(SHADER_TEXTURE), 4, "camera", &camera[0]);
        drawTextureIdColor(*(unsigned int*)entity_get(usedWeapon, COMPONENT_TEX_ID), v, white);
        glee_shader_uniform_set(assetsGetShader(SHADER_TEXTURE), 4, "camera", &cam.x);

        glUseProgram(assetsGetShader(SHADER_FONT));
        drawStringCentered(
            string, 
            assetsGetFont(FONT_1), 
            &red.x,
            (viewport.x / viewport.z) * 0.5 + 98.0f,
            (viewport.y / viewport.z) - 55.0f, 
            0.5f, 
            0.0f
        );
    }

    if (jetpack) {
        glUseProgram(assetsGetShader(SHADER_TEXTURE));
        Rect r = *(Rect*)entity_get(jetpack, COMPONENT_GL_RECT);
        if (r.w < 0.0f) r.w *= -1.0f;
        if (r.h < 0.0f) r.h *= -1.0f;
        Rect v = {(viewport.x / viewport.z) * 0.5f + 90.0f, (viewport.y / viewport.z) - 12.0f, r.w, r.h};
        unsigned int fuel = *(unsigned int*)entity_get(jetpack, COMPONENT_AMMO);
        sprintf(string, "%u", fuel);
        glee_shader_uniform_set(assetsGetShader(SHADER_TEXTURE), 4, "camera", &camera[0]);
        drawTextureIdColor(*(unsigned int*)entity_get(jetpack, COMPONENT_TEX_ID), v, white);
        glee_shader_uniform_set(assetsGetShader(SHADER_TEXTURE), 4, "camera", &cam.x);

        glUseProgram(assetsGetShader(SHADER_FONT));
        drawStringCentered(
            string, 
            assetsGetFont(FONT_1), 
            &red.x,
            (viewport.x / viewport.z) * 0.5 + 98.0f,
            (viewport.y / viewport.z) - 40.0f, 
            0.5f, 
            0.0f
        );
    }
}

static void playerDrawDead()
{
    static unsigned int sprite = SPRITE_DEAD;
    entity_set(player, COMPONENT_SPRITE_ID, &sprite);
    Rect* rPhi = entity_get(player, COMPONENT_PHI_RECT);
    rPhi->w = 24.0f;
    rPhi->h = 12.0f;
}

static void pickObject()
{
    Entity gun = checkGunCollision(player);
    Entity jet = checkJetpackCollision(player);
    Entity granade = checkGranadeCollision(player);
    bool ret = false;

    if (granade && granadeCount < GRANADE_MAX - 1) {
        granadeCollect(granade);
        netGranadePick = granade;
        granades[granadeCount] = granade;
        granadeCount++;
        ret = true;
    }
    if (jet) {
        if (jetpack) jetpackDrop(jetpack);
        else netJetpack = jet;
        jetpackPick(jet);
        jetpack = jet;
        ret = true;
    }  
    if (gun) {
        if (usedWeapon) gunDrop(usedWeapon);
        else netUsedWeapon = gun;
        gunPick(gun) ;
        usedWeapon = gun;
        ret = true;
    } 
    if (ret) return;

    if (usedWeapon) {
        gunDrop(usedWeapon);
        usedWeapon = 0;
    } else if (jetpack) {
        jetpackDrop(jetpack);
        jetpack = 0;
    }
}

void playerReset()
{
    static unsigned int k = SPRITE_IDLE;
    static vec2 vecZero = {0.0f, 0.0f};

    sprite_t* sprite = assetsGetSprite(currentPlayerSprite, k);
    Rect r = {spawnPoint.x, spawnPoint.y, sprite->textures->width, sprite->textures->height};
    Rect hitbox = {spawnPoint.x, spawnPoint.y, 12.0f, 24.0f};
    entity_set(player, COMPONENT_GL_RECT, &r);
    entity_set(player, COMPONENT_PHI_RECT, &hitbox);
    entity_set(player, COMPONENT_VEL_VEC2, &vecZero);
    entity_set(player, COMPONENT_SPRITE_ID, &k);
    hp = 100;
}

static void playerDeadStep(float deltaTime)
{
    static float deadTimer = 1.0f;
    deadTimer -= deltaTime * 0.3f;
    if (deadTimer < 0.0f) {
        deadTimer = 1.0f;
        playerReset();
        blackAndWhite = false;
        if (playerLives) playerLives--;
        else playerLives = 3;
    } else playerDrawDead();
}

void playerKill()
{
    if (usedWeapon) {
        gunDrop(usedWeapon);
        usedWeapon = 0;
    }
    if (jetpack) {
        jetpackDrop(jetpack);
        jetpack = 0;
    }
    while(granadeCount > 0) {
        if (netGranadeDrop) queue_push(netGranadeDrop, &granades[granadeCount - 1]);
        granadeDrop(granades[--granadeCount], *(vec2*)entity_get(player, COMPONENT_PHI_RECT));
    }
}

void playerGameStep(float deltaTime)
{
    if (hp == 0) {
        playerKill();
        blackAndWhite = true;
        playerDeadStep(deltaTime);
        return;
    }

    static const float speed = 100.0f;
    static const float jump = 200.0f;
    static const float wallSlideSpeed = -40.0f;
    static const float wallJumpDash = 300.0f;
    static const float wallJumpForce = 170.0f;
    static const float doubleJumpForce = 150.0f;
    static const float dashSpeed = 500.0f;
    static const float doubleClick = 0.2f;
    static const float dashWait = 0.15f;

    static bool doubleJump = true;
    static bool canDash = true; 
    static float isDashing = 0.0f;

    static float doubleKeyTimer = 0.2f;
    static int doubleKeyDir = 0;

    unsigned int sprite = SPRITE_IDLE;
    Rect* const playerPhi = (Rect*)entity_get(player, COMPONENT_PHI_RECT);
    Rect* const playerTex = (Rect*)entity_get(player, COMPONENT_GL_RECT);
    vec2* const vel = (vec2*)entity_get(player, COMPONENT_VEL_VEC2);

    bool keyGranade = glee_mouse_pressed(GLFW_MOUSE_BUTTON_RIGHT);
    bool keySpace = glee_key_pressed(GLFW_KEY_SPACE);
    bool keyShift = glee_key_down(GLFW_KEY_LEFT_SHIFT);
    bool keyUp = (glee_key_pressed(GLFW_KEY_UP) || glee_key_pressed(GLFW_KEY_W));
    bool keyRight = (glee_key_down(GLFW_KEY_RIGHT) || glee_key_down(GLFW_KEY_D));
    bool keyLeft = (glee_key_down(GLFW_KEY_LEFT) || glee_key_down(GLFW_KEY_A));
    bool keyRightPressed = (glee_key_pressed(GLFW_KEY_RIGHT) || glee_key_pressed(GLFW_KEY_D));
    bool keyLeftPressed = (glee_key_pressed(GLFW_KEY_LEFT) || glee_key_pressed(GLFW_KEY_A));
    int keyDir = 0;

    bool isGrounded = checkRigidCollision(player, vec2_new(0.0f, -1.0f));
    bool camAlarm = false;
    bool wallSliding = false;

    //Input Logic
    if (keyRight) keyDir++;
    if (keyLeft) keyDir--;
    if (keyRightPressed) {
        doubleKeyDir++;
        doubleKeyTimer = doubleClick;
    }
    if (keyLeftPressed) {
        doubleKeyDir--;
        doubleKeyTimer = doubleClick;
    }
    if (doubleKeyTimer < 0.0f) doubleKeyDir = 0;
    else doubleKeyTimer -= deltaTime;

    if (keySpace) pickObject();
    if (keyGranade && granadeCount) {
        vec2 pos = {playerPhi->x, playerPhi->y};
        float rot = vec2_to_rad(vec2_sub(mouse, pos));
        granadeThrow(granades[--granadeCount], pos, rot);
        netGranadeThrow = granades[granadeCount];
    }
    if (keyShift && jetpack) {
        float mark = vel->y;
        jetpackUse(jetpack);
        if (vel->y > mark) stateJetpacking = true;
    } else stateJetpacking = false;

    unsigned int dammage = checkBlastCollision(player);
    hp = (hp - dammage) * (dammage <= hp);

    //Lateral Movement
    if (keyDir) {
        if (!checkRigidCollision(player, vec2_new((float)keyDir * speed * deltaTime, 1.0f))) {
            playerPhi->x += (float)keyDir * deltaTime * speed;
            playerTex->x = playerPhi->x;
            sprite = SPRITE_RUNNING;
            playerTex->w = (float)keyDir;
            camAlarm = true;
            if (isGrounded) {
                smokeEmit(vec2_new(playerPhi->x, playerPhi->y - 12.0f), TEXTURE_SMOKE);
            }
        } 
        //WallSlide
        else if (!isGrounded) {
            wallSliding = true;
            if (vel->y < wallSlideSpeed) {
                vel->y = wallSlideSpeed;
                smokeEmit(vec2_new(playerPhi->x + (float)keyDir * 8.0f, playerPhi->y - 12.0f), TEXTURE_SMOKE);
            }
            if (keyUp && !checkRigidCollision(player, vec2_new(-(float)keyDir * wallJumpDash * deltaTime, wallJumpForce * deltaTime))) {
                vel->y = wallJumpForce;
                vel->x = -(float)keyDir * wallJumpDash;
                doubleJump = true;
                smokeEmit(vec2_new(playerPhi->x + (float)keyDir * 8.0f, playerPhi->y - 12.0f), TEXTURE_SMOKE);
            }
        }
    }

    //Dashing
    if (doubleKeyDir && doubleKeyDir % 2 == 0 && canDash) {
        float dir = (float)(doubleKeyDir / 2);
        if (!checkRigidCollision(player, vec2_new(dir * dashSpeed * deltaTime, 1.0f))) {
            vel->x = dir * dashSpeed;
            canDash = false;
            isDashing = dashWait;
        }
        doubleKeyDir = 0;
    }
    if (isDashing > 0.0f) {
        vel->y = 0.0f;
        sprite = SPRITE_JUMPING;
        camAlarm = true;
        isDashing -= deltaTime;
        shadowEmit(player, vel->x);
    }
    else if (isGrounded || wallSliding) {
        canDash = true;
        doubleJump = true;
    }

    //Vertical Phi
    if (isGrounded) {
        if (keyUp && !checkRigidCollision(player, vec2_new(0.0f, jump * deltaTime))) {
            vel->y = jump;
            camAlarm = true;
            smokeEmit(vec2_new(playerPhi->x, playerPhi->y - 12.0f), TEXTURE_SMOKE);
        }
    } else {
        camAlarm = true;
        if (keyUp && !wallSliding && doubleJump &&
            !checkRigidCollision(player, vec2_new(0.0f, doubleJumpForce * deltaTime))) {
            vel->y = doubleJumpForce;
            doubleJump = false;
            smokeEmit(vec2_new(playerPhi->x, playerPhi->y - 12.0f), TEXTURE_SMOKE);
        }
        if (vel->y > 0.0f) sprite = SPRITE_JUMPING;
        else sprite = SPRITE_FALLING;
    } 

    //Horizontal Phi
    if (vel->x > 0.1f || vel->x < -0.1f) {
        float lerp = lerpf(vel->x, 0.0f, 0.1f);
        if (!checkRigidCollision(player, vec2_new(lerp * deltaTime, 1.0f))) {
            vel->x = lerp;
        } else vel->x = -lerp * 0.5f;
    } else vel->x = 0.0f;

    //Set calculated values
    if (playerPhi->y < -600.0f) playerReset();
    entity_set(player, COMPONENT_SPRITE_ID, &sprite);
    sprite_frame_update(assetsGetSprite(currentPlayerSprite, sprite));
    if (camAlarm) cameraTriggerAlarm();

    stateWallSliding = wallSliding && vel->y < 0.0f;
    stateDashing = isDashing > 0.0f;
    playerDrawGUI();
}
