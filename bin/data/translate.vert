#version 330 core

uniform vec2 zoom;

uniform mat4 modelViewProjectionMatrix;
uniform mat4 textureMatrix;

in vec4 position;
in vec2 texcoord;

out vec2 texcoord_v;

void main() {
	vec4 pos = vec4(zoom.x, zoom.y, 1.0, 1.0);
    gl_Position	= modelViewProjectionMatrix * position * pos; 
    texcoord_v = texcoord;
}