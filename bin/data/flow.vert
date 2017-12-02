#version 330 core

uniform mat4 modelViewProjectionMatrix;
uniform mat4 textureMatrix;

in vec4 position;
in vec2 texcoord;

out vec2 texcoord0;
out vec2 texcoord1;

void main()
{
    gl_Position = modelViewProjectionMatrix * position; 
    texcoord0 = texcoord;
    texcoord1 = texcoord;
}