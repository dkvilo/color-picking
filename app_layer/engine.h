#ifndef _engine_h_
#define _engine_h_

#define LIBBL_STB_IMPLEMENTATION
#define LIBBL_USE_APPLE_OPENGL
#include "bl/libbl.h"

#include <cglm/cglm.h>

#define VERTEX_SIZE 4
#define INDICES_SIZE 6
#define ENTITIES_SIZE 512

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define ARRAY_SIZE_2D(a) (sizeof(a) / sizeof(a[0][0]))

#define internal static
#define inlined __attribute__((always_inline)) inline
#define external extern
#define global_variable static
#define compile_time const

// @Entity
typedef bl_ent_t ent_t;

#if 0
// @Mesh
typedef struct mesh
{
  bl_vertex_t vertex[VERTEX_SIZE];
  bl_vertex_index_t indices[INDICES_SIZE];
} mesh_t;

void
make_mesh(mesh_t* mesh);

// @Game
typedef struct game
{
  mesh_t mesh[ENTITIES_SIZE];
  ent_t entities[ENTITIES_SIZE];
} game_t;

// IMPLEMENTATION //

void
make_mesh(mesh_t* mesh)
{
  compile_time bl_vertex_t _vertices[VERTEX_SIZE] = {
    // @Position             //@Color              //@UV
    { { 0.5f, 0.5f, 0.0f }, { 1.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } },
    { { 0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } },
    { { -0.5f, -0.5f, 0.0 }, { 1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } },
    { { -0.5f, 0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f } },
  };

  //@Indexies
  compile_time bl_vertex_index_t _indices[INDICES_SIZE] = {
    { 0, 1, 2 },
    { 0, 2, 3 },
  };

  memcpy(mesh->vertex, _vertices, sizeof(_vertices));
  memcpy(mesh->indices, _indices, sizeof(_indices));
}
#endif

typedef struct
{
  bl_vertex_t* vertices;
  bl_vertex_index_t* indices;
} mesh_t;

typedef struct
{
  mesh_t* mesh;
  bl_ent_t* entity;
} renderable_entity_t;

renderable_entity_t*
make_renderable_entity(mesh_t* mesh, bl_ent_t* ent)
{

  renderable_entity_t* renderable = malloc(sizeof(renderable_entity_t));
  assert(renderable != NULL);

  renderable->mesh = mesh;
  assert(renderable->mesh != NULL);

  renderable->entity = ent;
  assert(renderable->entity != NULL);

  return renderable;
}

void
free_renderable_ent(renderable_entity_t* renderable)
{
  assert(renderable != NULL);
  free(renderable);
}

void
glm_update_projection(f32_t left,
                      f32_t right,
                      f32_t bottom,
                      f32_t top,
                      f32_t nearVal,
                      f32_t farVal,
                      mat4 dest)
{
  glm_ortho(left, right, bottom, top, nearVal, farVal, dest);
}

f64_t
to_radian(f64_t degree)
{
  return (degree * PI_DEG);
}

// TODO(dk): fix scaling factor based on resolution
i32_t
check_collision_AABB(bl_ent_t* a, bl_ent_t* b)
{
  vec2 size = { 1.8f, 1.8f };
  if (
    // X Axis
    (a->position[_x_] + size[_y_] >= b->position[_x_] &&
     b->position[_x_] + size[_x_] >= a->position[_x_]) &&
    // Y Axis
    (a->position[_y_] + size[_y_] >= b->position[_y_] &&
     b->position[_y_] + size[_y_] >= a->position[_y_])) {
    return 1;
  }
  return -1;
}

#endif
