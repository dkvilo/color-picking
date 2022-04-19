#ifndef _window_h_
#define _window_h_

#include "bl/libbl.h"
#include <GLFW/glfw3.h>

typedef struct
{
  vec2 cursor_pos;
  vec2 scroll_pos;

  u32_t current_key;
  u32_t current_scancode;
  u32_t current_action;
  u32_t current_mods;
} window_meta_t;

typedef struct
{
  GLFWwindow* ctx;
  u32_t width, height;
  cc_t* title;
  window_meta_t* meta;
} window_t;

external window_meta_t window_meta = { 0 };
external window_t
  global_window = { NULL, 1080.f, 720.f, "Color Picking Demo", &window_meta };

typedef enum
{
  cb_cursor_pos = 0,
  cb_scroll = 1,
  cb_key = 2
} win_cb_fun_t;

typedef enum
{
  window_key_press = GLFW_PRESS,
} window_action_t;

void
CB_CURSOR_POSITION(GLFWwindow* window, f64_t xpos, f64_t ypos)
{
  global_window.meta->cursor_pos[_x_] = xpos;
  global_window.meta->cursor_pos[_y_] = ypos;
}

void
CB_SCROLL_POSITION(GLFWwindow* window, f64_t xoffset, f64_t yoffset)
{
  global_window.meta->scroll_pos[_x_] = xoffset;
  global_window.meta->scroll_pos[_y_] = yoffset;
}

void
CB_KEY(GLFWwindow* window, u32_t key, u32_t scancode, u32_t action, u32_t mods)
{
  global_window.meta->current_key = key;
  global_window.meta->current_scancode = scancode;
  global_window.meta->current_action = action;
  global_window.meta->current_mods = mods;
}

void*
map_cb_enum_to_func(win_cb_fun_t cb_type)
{
  switch (cb_type) {
    case cb_cursor_pos:
      return &CB_CURSOR_POSITION;
      break;
    case cb_scroll:
      return &CB_SCROLL_POSITION;
      break;
    case cb_key:
      return &CB_KEY;
      break;
  }
}

i32_t
init_glfw()
{
  if (!glfwInit())
    return -1;
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_SAMPLES, 4);
  return 0;
}

void
make_window(window_t* win, f32_t w, f32_t h)
{
  win->width = w;
  win->height = h;
  win->ctx = glfwCreateWindow(win->width, win->height, win->title, NULL, NULL);

  glfwSetCursorPosCallback(global_window.ctx,
                           map_cb_enum_to_func(cb_cursor_pos));
  glfwSetScrollCallback(global_window.ctx, map_cb_enum_to_func(cb_scroll));
  glfwSetKeyCallback(global_window.ctx, map_cb_enum_to_func(cb_key));

  glfwMakeContextCurrent(win->ctx);
  glfwSwapInterval(1);
}

void
window_swap_buffers(window_t* win)
{
  glfwSwapBuffers(win->ctx);
}

i32_t
window_should_close(window_t* win)
{
  return glfwWindowShouldClose(win->ctx);
}

void
window_pull_events()
{
  glfwPollEvents();
}

void
free_window(window_t* win)
{
  glfwDestroyWindow(win->ctx);
  glfwTerminate();
}

#endif // _window_h_
