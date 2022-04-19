#ifndef _utils_h_
#define _utils_h_

#include "bl/libbl.h" // shader_*, cc_t, GL_*
#include <stdio.h>    // printf, free
#include <time.h>     // time, time_t

void
reload_program(shader_program_t* p,
               cc_t* vertex_shader_path,
               cc_t* fragment_shader_path,
               cc_t* vertex_shader_out,
               cc_t* fragment_shader_out)
{
  time_t begin = time(NULL);
  printf("[SHADER] Reloading Shader ... ");

  vertex_shader_out = read_file_content(vertex_shader_path);
  fragment_shader_out = read_file_content(fragment_shader_path);

  detach_program(&p);
  create_program(p,
                 compile_shader(vertex_shader_out, GL_VERTEX_SHADER),
                 compile_shader(fragment_shader_out, GL_FRAGMENT_SHADER));

  free(vertex_shader_out);
  free(fragment_shader_out);
  attach_program(p);
  time_t end = time(NULL);

  printf("%f ms Done.\n", difftime(end, begin) * 1000);
}

#endif // _utils_h_
