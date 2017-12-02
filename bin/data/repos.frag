#version 330 core

in vec2 texcoord_v;

out vec4 frag_color;

uniform vec2 amt;
uniform vec2 boundmode;
uniform sampler2DRect tex0; // footage
uniform sampler2DRect tex1; // blurred flow map

vec3 rgb2hsv(vec3 c) {
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}


void main()
{
    vec4 look = texture(tex1, texcoord_v);//sample repos texture
    vec3 flow = rgb2hsv(look.rgb) / 5.;
    // vec2 offs = vec2(look.g - look.r, look.a - look.b)*amt;
    vec2 offs = vec2(-flow.r, look.a - flow.g)*amt;
    vec2 coord = offs + texcoord_v;//relative coordinates
    //coord = mod(coord,texcoord_v);
    vec4 repos = texture(tex0, coord);
    
    // output texture
    frag_color = repos;//*scale+bias;
}