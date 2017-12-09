#version 330 core

uniform mat4 textureMatrix;

in vec2 texcoord_v;
out vec4 color;

void main(void){
    color = vec4(noise3(vec3(texcoord_v, 1.0) * 10.0) * 3.0, 1.0);
}
