uniform vec2 zoom;

void main() {
	vec4 pos = vec4(zoom.x, zoom.y, 1.0, 1.0);

    gl_Position	= gl_ModelViewProjectionMatrix * gl_Vertex * pos; 
    gl_FrontColor = gl_Color;
    gl_TexCoord[0] = gl_MultiTexCoord0;
}