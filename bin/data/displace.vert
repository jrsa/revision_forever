/*
//all uniforms provided by OF
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 modelViewProjectionMatrix;
uniform mat4 textureMatrix;
uniform vec4 globalColor;;
*/

#define TWO_PI 6.283185308

uniform sampler2DRect tex0;
uniform sampler2DRect tex1;

uniform float time;

uniform float downsampleAmt;
uniform float displaceAmt;

uniform mat4 camPosition;

varying vec2 TexCoord;
varying vec3 N;
varying vec3 P;
varying vec3 V;
varying vec3 L;

mat2 rotate2d(float _angle){
    return mat2(cos(_angle),-sin(_angle),
                sin(_angle),cos(_angle));
}

mat3 rotate3d(float _angle1, float _angle2, float _angle3){
    return mat3(cos(_angle1),-sin(_angle1), 0.0,
                sin(_angle2), cos(_angle2), 0.0,
                sin(_angle3),-cos(_angle3), 1.0);
}

mat3 rotate3d(float _angle1, float _angle2, float _angle3, float _t){
    _angle1  = (_angle1 * TWO_PI*2.0) - TWO_PI;
    _angle2  = (_angle2 * TWO_PI*2.0) - TWO_PI;
    _angle3  = (_angle3 * TWO_PI*2.0) - TWO_PI;
    return mat3(cos(_angle1 + _t),-sin(_angle1 + _t), 0.0,
                sin(_angle2 + _t), cos(_angle2 + _t), 0.0,
                sin(_angle3 + _t),-cos(_angle3 + _t), 1.0);
}

vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}


void main() {
    TexCoord = gl_MultiTexCoord0.xy;

	vec2 uv = gl_MultiTexCoord0.xy; //tex coords
	uv *= mat2(downsampleAmt, 0.0, 0.0, downsampleAmt);

	vec4 sourceColor = texture2DRect(tex0, uv);
    float df = (0.30*sourceColor.r + 0.59*sourceColor.g + 0.11*sourceColor.b);
    df = (df * 2.0) - 1.0;

    vec3 hsv = rgb2hsv(sourceColor.rgb);

    vec4 vert = gl_Vertex;
    vert.xyz *= rotate3d(((hsv.r * 2.0) - 1.0) *  -0.2, 
                         ((hsv.r * 2.0) - 1.0) *   0.4, 
                         ((hsv.r * 2.0) - 1.0) *   0.2);
    vert.xy *= mat2(((hsv.g * 2.0) - 1.0) * -0.2 + 1.0, 0.0, 0.0, 
                    ((hsv.b * 2.0) - 1.0) *  0.4 + 1.0);

    vec3 norms = gl_Normal;

  	//vec4 newVertexPos = vert + vec4(norms * df * displaceAmt*0.25, 0.0);
    vec4 newVertexPos = vert;
    //newVertexPos.z += norms.z * df * displaceAmt*0.2;

  	gl_Position = gl_ModelViewProjectionMatrix * newVertexPos;

    gl_FrontColor = gl_Color;
    gl_TexCoord[0] = gl_MultiTexCoord0;

    ////////////////////////////////////////////////////////
    norms = (hsv * 2.0) - 1.0;
    norms *= rotate3d(hsv.r, hsv.g, hsv.b, time * 0.025);

    N = normalize(gl_NormalMatrix * norms); 
    P = newVertexPos.xyz;
    V = -vec3(gl_ModelViewMatrix * newVertexPos);
    L = vec3(gl_ModelViewMatrix * (vec4(0.0, 0.0, 0.0, 0.0) - newVertexPos));
}