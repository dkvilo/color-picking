/**
 * @file main.c
 * @author David Kviloria (dkviloria@gmail.com)
 * @brief Color Picking using OpenGL pixel sampling
 * @version 0.1
 * @date 10/11/20.
 *
 * @copyright Copyright (c) 2020
 *
 */
#define GL_SILENCE_DEPRECATION
#include <math.h> // sin
#include <stdio.h>
#include <stdlib.h>

#include "engine.h"
#include "utils.h"
#include "window.h"

#define WINDOW_WIDTH 1080
#define WINDOW_HEIGHT 720

compile_time global_variable i32_t bl_selected_NONE = 0xFF00;
global_variable i32_t bl_selected_entity = bl_selected_NONE;

#define v_width 1.0f
#define v_height 1.0f
#define v_scale 1.0f

compile_time global_variable bl_vertex_t global_vertices[4] = {
  // @Position                                        // @UV
  { { v_width * v_scale, v_height* v_scale, -0.0f }, { 0.0f, 0.0f } },
  { { v_width * v_scale, -v_height* v_scale, -0.0f }, { 0.0f, 1.0f } },
  { { -v_width * v_scale, -v_height* v_scale, -0.0f }, { 1.0f, 1.0f } },
  { { -v_width * v_scale, v_height* v_scale, -0.0f }, { 1.0f, 0.0f } },
};

//@Indexies
compile_time internal bl_vertex_index_t global_indices[6 * 2] = {
  { 0, 1, 2 },
  { 0, 2, 3 },
};

internal f64_t scroll_x, scroll_y;
internal void
scroll_callback(GLFWwindow* window, f64_t xoffset, f64_t yoffset)
{
  scroll_x = xoffset;
  scroll_y = yoffset;
}

internal f64_t cursor_posx, cursor_posy;
internal void
cursor_position_callback(GLFWwindow* window, f64_t xpos, f64_t ypos)
{
  cursor_posx = xpos;
  cursor_posy = ypos;
}

void
mouse_to_world(f32_t mouse_x, f32_t mouse_y, f32_t* world_x, f32_t* world_y)
{
  f32_t x = (mouse_x / (f32_t)WINDOW_WIDTH) * 2.0f - 1.0f;
  f32_t y = (mouse_y / (f32_t)WINDOW_HEIGHT) * 2.0f - 1.0f;
  *world_x = x;
  *world_y = y;
}

// convert x, y from screen space to world space
void
screen_to_world(window_t* window,
                f32_t x,
                f32_t y,
                f32_t* world_x,
                f32_t* world_y)
{
  f32_t screen_width = window->width;
  f32_t screen_height = window->height;

  f32_t half_width = screen_width / 2.0f;
  f32_t half_height = screen_height / 2.0f;

  *world_x = (x - half_width) / half_width;
  *world_y = (y - half_height) / half_height;
}

internal f32_t
clamp(f32_t min, f32_t max, f32_t value)
{
  if (value < min)
    return min;
  if (value > max)
    return max;
  return value;
}

internal f32_t
lerp(f32_t a, f32_t b, f32_t f)
{
  return a + f * (b - a);
}

internal f32_t
inverse_lerp(f32_t a, f32_t b, f32_t value)
{
  return (value - a) / (b - a);
}

internal void
write_to_buffer(u8_t* buffer,
                u32_t size,
                u32_t* offset,
                u8_t* data,
                u32_t data_size)
{
  u32_t i;
  for (i = 0; i < data_size; ++i) {
    buffer[(*offset)++] = data[i];
  }
}

i32_t
main(i32_t argc, const char* argv[])
{

#if 0
	bl_ent_t test_ent = {
     .position = { 100.0f, 100.0f, 200.0f },
     .scale    = {1.0f, 1.0f, 1.0f},
     .index    = 0xFF123D,
     .name     = "Test Entity",
     .rotation = to_radian(25.0f)
  };

  mesh_t test_mesh = {
     .vertices = global_vertices,
     .indices  = global_indices,
  };

  renderable_entity_t* enttt = make_renderable_entity(&test_mesh, &test_ent);

  free_renderable_ent(&rent);
  printf("Posixtion X %f\n", enttt->entity->position[_x_]);
#endif

  i32_t model_location, view_location, projection_location;

  if (init_glfw() == -1) {
    printf("Error: Unable to initialize GLFW\n");
  }

  make_window(&global_window, WINDOW_WIDTH, WINDOW_HEIGHT);

  bl_ent_t camera = { .name = "Camera",
                      .index = 30,
                      .position = { 0.0f, 0.0f, 0.0f } };

  bl_register_entity(&camera, global_manager);

  bl_ent_t obj = { .name = "Obj",
                   .index = 20,
                   .position = { 0.0f, 0.0f, 0.0f } };

  bl_register_entity(&obj, global_manager);

  vertex_buffer_t vbo;
  gen_vertex_buffer(&vbo);
  bind_vertex_buffer(&vbo);
  vertex_buffer_data(sizeof(global_vertices), global_vertices);

  indice_buffer_t ebo;
  gen_indice_buffer(&ebo);
  bind_indice_buffer(&ebo);

  vertex_array_t vao;
  gen_and_bind_vertex_array(&vao);

  bind_vertex_buffer(&vbo);
  vertex_buffer_data(sizeof(global_vertices), global_vertices);

  bind_indice_buffer(&ebo);
  indice_buffer_data(sizeof(global_indices), global_indices);

  cc_t* vert_src = read_file_content("shaders/base_vertex_shader.vert");
  cc_t* frag_src = read_file_content("shaders/base_texture_fragment.frag");

  shader_program_t program;
  create_program(&program,
                 compile_shader(vert_src, GL_VERTEX_SHADER),
                 compile_shader(frag_src, GL_FRAGMENT_SHADER));

  free(vert_src);
  free(frag_src);

  u32_t texture_id;
  load_and_create_texture("./assets/mario.png", &texture_id, 0, NULL);

  u32_t texture_id2;
  load_and_create_texture("./assets/base.png", &texture_id2, 1, NULL);

  model_location = glGetUniformLocation(program.m_program_id, "model");
  view_location = glGetUniformLocation(program.m_program_id, "view");
  projection_location =
    glGetUniformLocation(program.m_program_id, "projection");

  u32_t timeUni = glGetUniformLocation(program.m_program_id, "u_time");
  u32_t tex0Uni = glGetUniformLocation(program.m_program_id, "tex0");
  u32_t tex1Uni = glGetUniformLocation(program.m_program_id, "tex1");
  u32_t colorUni = glGetUniformLocation(program.m_program_id, "u_color");
  u32_t pickModeUni = glGetUniformLocation(program.m_program_id, "u_pick_mode");
  u32_t pickColorUni =
    glGetUniformLocation(program.m_program_id, "u_pick_color");

  // position attribute
  glVertexAttribPointer(0,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(global_vertices[0]),
                        (void*)offsetof(bl_vertex_t, position));
  glEnableVertexAttribArray(0);

  // texture coord attribute
  glVertexAttribPointer(1,
                        2,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(bl_vertex_t),
                        (void*)offsetof(bl_vertex_t, uv));
  glEnableVertexAttribArray(1);

  f32_t ratio;
  u32_t width, height;

  mat4 model, view, projection;

  f64_t deltaTime, previousTime, elapsedTime;
  f32_t p_speed = 1.1f;

  bl_ent_t* ent_camera = bl_get_entity("Camera", global_manager);
  assert(ent_camera != NULL);

  bl_ent_t* ent_obj = bl_get_entity("Obj", global_manager);
  assert(ent_obj != NULL);

  u16_t press_time = 0;

  f32_t zoom_level = 12.0f;
  i32_t total_frames = 0;

  while (!window_should_close(&global_window)) {

    window_pull_events();

    total_frames++;
    deltaTime = glfwGetTime();
    previousTime = deltaTime;
    elapsedTime = deltaTime - previousTime;

    glfwGetFramebufferSize(
      global_window.ctx, &global_window.width, &global_window.height);
    ratio = (f32_t)global_window.width / (f32_t)global_window.height;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glViewport(0, 0, global_window.width, global_window.height);

    glClearColor(47.0f / 255.0f, 54.0f / 255.0f, 64.0f / 255.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

    glm_mat4_identity(model);
    glm_mat4_identity(view);

    i32_t state = glfwGetMouseButton(global_window.ctx, GLFW_MOUSE_BUTTON_LEFT);
    if (state == GLFW_PRESS) {
      zoom_level += sin(scroll_y * (deltaTime / 10.0f));
    } else {
      scroll_y = 0.0f;
    }

    glm_update_projection(-ratio * zoom_level,
                          ratio * zoom_level,
                          -zoom_level,
                          zoom_level,
                          -1.0f,
                          1.0f,
                          projection);

    // Render Quads
    for (sz_t x = 0; x < ARRAY_SIZE(global_manager); x++) {
      // Don't draw NULL entities from maganger
      if (global_manager[x] == NULL)
        continue;

      bl_ent_t* ent = global_manager[x];

      if (ent->index == x && ent->name == "Camera") {
        glm_translate(view, ent_camera->position);
        glUniform3fARB(colorUni, 1.0f, 1.0f, 1.0f);
        glUniform1i(tex0Uni, 0);
      }

      if (ent->index == x && ent->name == "Obj") {
        glm_translate(model, ent_obj->position);
        if (check_collision_AABB(ent_camera, ent_obj) == 1) {
          glUniform3fARB(colorUni, 1.0f, 0.0f, 0.0f);
        } else {
          glUniform3fARB(colorUni, 1.0f, 1.0f, 1.0f);
        }
        glUniform1i(tex0Uni, 1);
      }

      if (ent->index == bl_selected_entity) {
        glUniform3fARB(pickColorUni, 0.0f, 1.0f, 0.0f);
      } else {
        i32_t r = (ent->index & 0x000000FF) >> 0;
        i32_t g = (ent->index & 0x0000FF00) >> 8;
        i32_t b = (ent->index & 0x00FF0000) >> 16;
        glUniform3fARB(pickColorUni, r / 255.0f, g / 255.0f, b / 255.0f);
      }

#if 0
  // translate model in the world space by 5 units in the x axis
  glm_translate(model, (vec3){ent->position[_x_] * 0.0f, ent->position[_y_], ent->position[_z_]});
  glm_translate(model, ent->position);
#endif

      glUniformMatrix4fv(model_location, 1, GL_FALSE, model);
      glUniformMatrix4fv(view_location, 1, GL_FALSE, view);
      glUniformMatrix4fv(projection_location, 1, GL_FALSE, projection);

      glUniform1f(timeUni, deltaTime);
      glUniform1i(tex1Uni, 1);

      if (glfwGetKey(global_window.ctx, GLFW_KEY_TAB) == GLFW_PRESS) {
        if (++press_time == 1) {
          glUniform1f(pickModeUni, 1.0f);
        }
      } else {
        glUniform1f(pickModeUni, 0.0f);
        press_time = 0;
      }

      attach_program(&program);

      if (glfwGetKey(global_window.ctx, GLFW_KEY_R) == GLFW_PRESS) {
        if (++press_time == 1) {
          reload_program(&program,
                         "shaders/base_vertex_shader.vert",
                         "shaders/base_color_shader.glsl",
                         vert_src,
                         frag_src);
        }
      } else {
        press_time = 0;
      }

      draw_triangles(sizeof(global_vertices));
      glDeleteTextures(0, &texture_id);
      glDeleteTextures(1, &texture_id2);
      detach_program(&program);
    }

    if (glfwGetKey(global_window.ctx, GLFW_KEY_TAB) == GLFW_PRESS) {
      if (glfwGetMouseButton(global_window.ctx, GLFW_MOUSE_BUTTON_LEFT) ==
          GLFW_PRESS) {

        glFlush();
        glFinish();

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        f64_t x = global_window.meta->cursor_pos[_x_];
        f64_t y = global_window.height - global_window.meta->cursor_pos[_y_];

        u8_t data[3];
        glReadPixels(x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, data);

        i32_t pickedID = data[_r_] + data[_g_] * 256 + data[_b_] * 256;
        if (bl_selected_entity != pickedID && pickedID != bl_selected_NONE) {
          bl_selected_entity = pickedID;
        }
        printf("SELECTED_MESH ID[%d]\n", bl_selected_entity);
      }
    }

    vec2 mouse_pos = { global_window.meta->cursor_pos[_x_],
                       global_window.height -
                         global_window.meta->cursor_pos[_y_] };

    // set ent position to mouse position
    if (glfwGetMouseButton(global_window.ctx, GLFW_MOUSE_BUTTON_LEFT) ==
        GLFW_PRESS) {
      if (glfwGetKey(global_window.ctx, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {

        glfwSetInputMode(global_window.ctx, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        f64_t x =
          global_window.meta->cursor_pos[_x_] - (global_window.width / 2);
        f64_t y = global_window.meta->cursor_pos[_y_];

        // convert x and y to -1 to 1 range
        // x = (x / global_window.width) * 2.0f - 1.0f;
        // y = (y / global_window.height) * 2.0f - 1.0f;

        // clamp x and y to -1 to 1 range
        // x = clamp(-1.0f, 1.0f, x);
        // y = clamp(-1.0f, 1.0f, y);

        {
          // convert x and y to the view space
          vec4 view_pos = { x, y, 0.0f, 1.0f };
          vec4 world_pos = { ent_obj->position[_x_],
                             ent_obj->position[_y_],
                             ent_obj->position[_z_],
                             1.0f };

          // convert view_pos to world_pos
          glm_vec4_mul(view_pos, projection, world_pos);
          glm_vec4_mul(world_pos, view, world_pos);
          glm_vec4_mul(world_pos, model, world_pos);

          // set ent position to world_pos
          ent_obj->position[_x_] = world_pos[_x_];
        }

        // prit ent_obj position
        printf("ENT_OBJ POSITION (%f, %f, %f)\n",
               ent_obj->position[_x_],
               ent_obj->position[_y_],
               ent_obj->position[_z_]);
      }
    } else {
      glfwSetInputMode(global_window.ctx, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

#if 0
    printf("mouse_pos[%f, %f]\n", mouse_pos[_x_], mouse_pos[_y_]);
#endif

    // Camera Controls
    if (glfwGetKey(global_window.ctx, GLFW_KEY_D) == GLFW_PRESS) {
      ent_camera->position[_x_] = ent_camera->position[_x_] +=
        p_speed / zoom_level;
    }

    if (glfwGetKey(global_window.ctx, GLFW_KEY_A) == GLFW_PRESS) {
      ent_camera->position[_x_] = ent_camera->position[_x_] -=
        p_speed / zoom_level;
    }

    if (glfwGetKey(global_window.ctx, GLFW_KEY_W) == GLFW_PRESS) {
      ent_camera->position[_y_] = ent_camera->position[_y_] +=
        p_speed / zoom_level;
    }

    if (glfwGetKey(global_window.ctx, GLFW_KEY_S) == GLFW_PRESS) {
      ent_camera->position[_y_] = ent_camera->position[_y_] -=
        p_speed / zoom_level;
    }

    if (global_window.meta->current_key == GLFW_KEY_Q &&
        global_window.meta->current_action == window_key_press) {
      glfwSetWindowShouldClose(global_window.ctx, GLFW_TRUE);
    }

    window_swap_buffers(&global_window);
  }

  free_window(&global_window);

  return EXIT_SUCCESS;
}
