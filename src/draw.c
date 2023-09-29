#include "Tree.h"
#include <string.h>

extern Entity player;
extern unsigned int quadVAO;
extern unsigned int currentPlayerSprite;
extern Mesh mesh;
extern vec4 cam;

static float white[] = {1.0f, 1.0f, 1.0f, 1.0f};
static float scale_rot[] = {1.0f, 0.0f};

void drawSetCamera(float* camera)
{
    glee_shader_uniform_set(assetsGetShader(SHADER_TEXTURE), 4, "camera", camera);
}

void drawTextureColor(texture_t t, vec2 pos, vec4 c)
{
    unsigned int shader = assetsGetShader(SHADER_TEXTURE);
    glee_shader_uniform_set(shader, 2, "scale_rot", &scale_rot[0]);
    float trans[] = {pos.x, pos.y, t.width, t.height};
    glBindTexture(GL_TEXTURE_2D, t.id);
    glee_shader_uniform_set(shader, 4, "color", &c.x);
    glee_shader_uniform_set(shader, 4, "trans", &trans[0]);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glee_shader_uniform_set(shader, 4, "color", &white[0]);
}

void drawTextureIdColor(unsigned int id, Rect rect, vec4 c)
{
    unsigned int shader = assetsGetShader(SHADER_TEXTURE);
    glee_shader_uniform_set(shader, 2, "scale_rot", &scale_rot[0]);
    glBindTexture(GL_TEXTURE_2D, id);
    glee_shader_uniform_set(shader, 4, "color", &c.x);
    glee_shader_uniform_set(shader, 4, "trans", &rect.x);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glee_shader_uniform_set(shader, 4, "color", &white[0]);
}

void drawTextureTransformed(texture_t t, vec2 pos, vec4 c, float scale, float rot)
{
    unsigned int shader = assetsGetShader(SHADER_TEXTURE);
    float sr[] = {scale, rot};
    Rect r = {pos.x, pos.y, (float)t.width, (float)t.height};
    glee_shader_uniform_set(shader, 2, "scale_rot", &sr[0]);
    glee_shader_uniform_set(shader, 4, "color", &c.x);
    glBindTexture(GL_TEXTURE_2D, t.id);
    glee_shader_uniform_set(shader, 4, "trans", &r.x);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void drawParallax(vec4 cam)
{
    vec2 center = {(viewport.x / viewport.z) * 0.5f, (viewport.y / viewport.z) * 0.5f};
    cam.z = 1.0f;
    unsigned int shader = assetsGetShader(SHADER_TEXTURE);
    glee_shader_uniform_set(shader, 4, "camera", &cam.x);
    vec4 c = vec4_uni(1.0);
    texture_t* t = assetsGetParallax(PARALLAX_3);
    float x = cam.x + t->width * 0.33f / viewport.z;
    drawTextureTransformed(*t, vec2_new(x / 1.5f + center.x, cam.y / 1.015f + center.y), c, 0.6f, 0.0f);
    t--;
    drawTextureTransformed(*t, vec2_new(x / 3.0f + center.x, cam.y / 1.03f + center.y), c, 0.6f, 0.0f);
    t--;
    drawTextureTransformed(*t, vec2_new(x / 4.5f + center.x, cam.y / 1.045f + center.y), c, 0.6f, 0.0f);
}

static void drawChar(unsigned int id, float* trans, float* scaleRot, float* color)
{   
    unsigned int shader = assetsGetShader(SHADER_FONT);
    glBindTexture(GL_TEXTURE_2D, id);
    glee_shader_uniform_set(shader, 4, "scale_rot", scaleRot);
    glee_shader_uniform_set(shader, 4, "color", color);
    glee_shader_uniform_set(shader, 4, "trans", trans);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void drawString(const char* text, font_t* font, float* color, float x, float y, float scale, float rot)
{
    const int size = strlen(text);
    float xpos[size], ypos[size], w[size], h[size];
    for (int i = 0; i < size; i++) {
        font_t ch = *(font + text[i]);
        xpos[i] = x + ch.bearing_x * scale;
        ypos[i] = y + (ch.size_y + ch.bearing_y);
        w[i] = ch.size_x * scale;
        h[i] = ch.size_y * scale;
        x += (ch.advance >> 6) * scale; 

        float trans[] = {xpos[i], ypos[i], w[i], h[i]};
        float scaleRot[] = {scale, rot, x, y};
        drawChar((font + text[i])->id, trans, scaleRot, color);
    }
}

void drawStringCentered(const char* text, font_t* font, float* color, float x, float y, float scale, float rot)
{
    const float xx = x;
    const int size = strlen(text);
    float xpos[size], ypos[size], w[size], h[size], acc = x;
    for (int i = 0; i < size; i++) {
        font_t ch = *(font + text[i]);
        xpos[i] = x + ch.bearing_x * scale;
        ypos[i] = y + (ch.size_y + ch.bearing_y);
        w[i] = ch.size_x * scale;
        h[i] = ch.size_y * scale;
        x += (ch.advance >> 6) * scale;
        acc += ((ch.advance >> 6) + ch.bearing_x + ch.size_x);
    }

    float hlength = (acc - xx) * scale / 4.0f;
    for (int i = 0; i < size; i++) {
        float trans[] = {xpos[i] - hlength, ypos[i], w[i], h[i]};
        float scaleRot[] = {scale, rot, x, y};
        drawChar((font + text[i])->id, trans, scaleRot, color);
    }
}

void drawFramebuffer()
{
    framebuffer_bind(0);
    glee_screen_clear();
    unsigned int id = assetsGetFramebuffer(FRAMEBUFFER_BLACK_AND_WHITE)->texture.id;
    glUseProgram(assetsGetShader(SHADER_FRAMEBUFFER_BYN));
    glBindTexture(GL_TEXTURE_2D, id);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void drawRect(Rect rect, vec4 color)
{
    unsigned int shader = assetsGetShader(SHADER_TEXTURE);
    glBindTexture(GL_TEXTURE_2D, assetsGetTexture(TEXTURE_WHITE)->id);
    glee_shader_uniform_set(shader, 2, "scale_rot", &scale_rot[0]);
    glUniform4f(glGetUniformLocation(shader, "trans"), rect.x, rect.y, rect.w * 2, rect.h * 2);
    glUniform4f(glGetUniformLocation(shader, "color"), color.x, color.y, color.z, color.w);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

/*************
COMPONENT DRAW
*************/

static void drawEntity(Entity e)
{
    static const Rect rectZero = {0.0f, 0.0f, 0.0f, 0.0f};
    unsigned int shader = assetsGetShader(SHADER_TEXTURE);

    float sr[] = {1.0f, 0.0f};
    float color[] = {1.0f, 1.0f, 1.0f, 1.0f};

    Rect* r = (Rect*)entity_get(e, COMPONENT_GL_RECT);
    if (!memcmp(r, &rectZero, sizeof(Rect))) return;

    unsigned int* t = (unsigned int*)entity_get(e, COMPONENT_TEX_ID);
    float* scale = (float*)entity_get(e, COMPONENT_SCALE);
    float* rot = (float*)entity_get(e, COMPONENT_ROTATION);
    float* alpha = (float*)entity_get(e, COMPONENT_ALPHA);

    if (scale) sr[0] = *scale;
    if (rot) sr[1] = *rot;
    if (alpha) color[3] = *alpha;
    glee_shader_uniform_set(shader, 2, "scale_rot", &sr[0]);
    glee_shader_uniform_set(shader, 4, "color", &color[0]);

    if (t) {
        glBindTexture(GL_TEXTURE_2D, *(unsigned int*)entity_get(e, COMPONENT_TEX_ID));
        glee_shader_uniform_set(shader, 4, "trans", &r->x);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    } else {
        unsigned int k;
        if (e == player) k = currentPlayerSprite;
        else k = *(unsigned int*)entity_get(e, COMPONENT_SPRITE_COLLECTION);
        sprite_t* s = assetsGetSprite(k, *(unsigned int*)entity_get(e, COMPONENT_SPRITE_ID));
        texture_t* t = s->textures + s->current_frame;
        Rect rect = {r->x, r->y, t->width * signf(r->w), t->height * signf(r->h)};
        glBindTexture(GL_TEXTURE_2D, t->id);
        glee_shader_uniform_set(shader, 4, "trans", &rect.x);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
}

static void drawComponent(Component component)
{
    unsigned int count = component_entity_count(component);
    for (unsigned int i = 0; i < count; i++) {
        drawEntity(entity_find(component, i));
    }
}

static void drawMesh()
{
    unsigned int shader = assetsGetShader(SHADER_MESH);
    glUseProgram(shader);
    glee_shader_uniform_set(shader, 4, "camera", (float*)&cam);
    glBindVertexArray(mesh.id);
    glBindTexture(GL_TEXTURE_2D, assetsGetTexture(TEXTURE_TILE_ATLAS)->id);
    glDrawArrays(GL_TRIANGLES, 0, mesh.vertices->used);

    glBindVertexArray(quadVAO);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(assetsGetShader(SHADER_TEXTURE));
}

void drawComponents()
{
    if (component_entity_count(COMPONENT_RIGID_COLLIDER)) drawMesh();
    drawComponent(COMPONENT_FIREBARREL);
    drawComponent(COMPONENT_BOX);
    drawComponent(COMPONENT_SMOKE);
    drawComponent(COMPONENT_JETPACK);
    drawComponent(COMPONENT_SHADOW);
    drawComponent(COMPONENT_SPRITE_ID);
    drawComponent(COMPONENT_BULLET);
    drawComponent(COMPONENT_GUN_CONTROLLER);
    drawComponent(COMPONENT_GRANADE);
    drawComponent(COMPONENT_EXPLOTION);
}
