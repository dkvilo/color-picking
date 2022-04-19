#version 110

varying vec2 texCoord;

uniform vec3 u_color;
uniform sampler2D tex0;
uniform sampler2D tex1;

uniform float u_time;

uniform float u_pick_mode;
uniform vec3 u_pick_color;

void main()
{
  vec4 first_texture = texture2D(tex0, texCoord);
  vec4 second_texture = texture2D(tex1, texCoord);
  gl_FragColor = first_texture * second_texture * vec4(u_color, 1.0);
  
  if (u_pick_mode == 1.0) {
    gl_FragColor = vec4(u_pick_color, 1.0);
  }
}
