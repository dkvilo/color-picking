#version 110

varying vec2 texCoord;

uniform vec3 color;
uniform float uTime;

void main()
{
  gl_FragColor = vec4(color, 1.0);
}
