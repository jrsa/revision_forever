#version 330 core

uniform mat4 modelViewProjectionMatrix;
uniform mat4 textureMatrix;

in vec4 position;
in vec2 texcoord;

out vec2 texcoord_v;

void main()
{
    // perform standard transform on vertex
    gl_Position = modelViewProjectionMatrix * position;

    // transform texcoords
	texcoord_v = texcoord;
}