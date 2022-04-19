#version 110

// uniform mat4 MVP;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

attribute vec2 vPos;
attribute vec2 vUV;

varying vec2 texCoord;

void main(){
  gl_Position = projection * view * model * vec4(vPos, 0.0, 1.0);
  texCoord = vUV;
}
