#ifndef GLEE_GRAPHICS_H
#define GLEE_GRAPHICS_H

#ifdef __cplusplus
extern "C" {
#endif

/*

======================================================

>>>>>>>>>>>>>>  GLEE OPENGL FRAMEWORK   >>>>>>>>>>>>>>

Simple wrap around glfw3 and glew, ment to be platform
independent and easy to use.

============================================  @eulogic

*/

#ifndef __APPLE__
    #include <GL/glew.h>
#else
    #define GLFW_INCLUDE_GLCOREARB
#endif

#include <GLFW/glfw3.h>

/*
-----------
 -> glee <- 
-----------
*/

void glee_init();
void glee_deinit();
void glee_set_3d_mode();
void glee_set_2d_mode();
void glee_set_2d_scale(unsigned int scale);
unsigned int glee_get_2d_scale();
void glee_get_size(unsigned int* width, unsigned int* height);

/*
-------------
 -> window <- 
-------------
*/

GLFWwindow* glee_window_get();
void glee_window_create(const char* title, int width, int height, unsigned int fullscreen, unsigned int resizable);
void glee_window_set_size(int width, int height);
void glee_window_get_size(int* width, int* height);
void glee_window_set_position(int x, int y);
void glee_window_get_position(int* x, int* y);
unsigned int glee_window_is_open();

/*
-------------
 -> files <- 
-------------
*/

char* glee_file_read(const char* path);
unsigned int glee_window_file_check();
char* glee_window_file_get();

/*
-------------
 -> screen <- 
-------------
*/

void glee_screen_color(float r, float g, float b, float a);
void glee_screen_clear();
void glee_screen_refresh();

/*
--------------
 -> shaders <- 
--------------
*/

char* glee_shader_file_read(const char* path);
char* glee_shader_parse(const char* path);
void glee_shader_compile(const GLchar* buffer, unsigned int shader);
void glee_shader_link(GLuint shader, unsigned int vshader, unsigned int fshader);
unsigned int glee_shader_load(const char *vpath, const char *fpath);
void glee_shader_uniform_set(unsigned int shader, unsigned int float_count, const char* uniform, float* data);

/*
---------------------
 -> vertex buffers <- 
---------------------
*/

unsigned int glee_buffer_id();
void glee_buffer_attribute_set(unsigned int attribute, unsigned int float_pack, unsigned int stride, size_t offset);
void glee_buffer_create(unsigned int id, void* buffer, unsigned int size);
void glee_buffer_create_indexed(unsigned int id, void* buffer, unsigned int buffer_size, unsigned int* indices, unsigned int indices_size);
unsigned int glee_buffer_quad_create();
unsigned int glee_buffer_voxel_create();
unsigned int glee_buffer_skybox_create();
void glee_buffer_voxel_texcoords(unsigned int id);

/*
-----------------
 -> time input <- 
-----------------
*/

float glee_time_get();
float glee_time_delta(float* last_time);

/*
------------------
 -> mouse input <- 
------------------
*/

void glee_mouse_pos(float* x, float* y);
void glee_mouse_pos_3d(float* x, float* y);
unsigned int glee_mouse_down(unsigned int button);
unsigned int glee_mouse_pressed(unsigned int button);
unsigned int glee_mouse_released(unsigned int button);
void glee_mouse_mode(unsigned int mouse_lock);

/*
---------------------
 -> keyboard input <- 
---------------------
*/

unsigned int glee_key_down(unsigned int k);
unsigned int glee_key_pressed(unsigned int k);
unsigned int glee_key_released(unsigned int k);
char glee_key_char();

#ifdef __cplusplus
}
#endif
#endif
