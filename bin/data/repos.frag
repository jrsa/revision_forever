#version 330 core

in vec2 texcoord_v;

out vec4 frag_color;

uniform vec2 amt;
uniform vec2 boundmode;
uniform sampler2D tex0; // footage
uniform sampler2D tex1; // blurred flow map

vec3 rgb2hsv(vec3 c) {
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 color) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(color.xxx + K.xyz) * 6.0 - K.www);
    vec3 rgb = vec3(color.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), color.y));

    return rgb;
}


void main()
{
    // hsv lookup of input pixel
    vec2 tc = texcoord_v;
    vec3 pixel = texture(tex0, tc).rgb;
    vec3 s = rgb2hsv(pixel);

    float scale_factor = 0.995;
    float fixangle = 0.001;
    tc -= vec2(0.5);
    tc *= mat2(scale_factor, 0.0, 0.0, scale_factor);
    tc *= mat2(cos(fixangle), sin(fixangle), -sin(fixangle), cos(fixangle));
    tc += vec2(0.5);

    // flow map lookup
    vec4 look = texture(tex1, texcoord_v);
    vec3 flow = rgb2hsv(look.rgb) / 5.;
    vec2 offs = vec2(look.g - look.r, look.a - look.b);
    
    // get a neighboring pixel based on hsv input
    float d = length(s.bg);
    float e = length(look.rb);
    vec4 prelook = texture(tex0, tc + (vec2(-d, -e) * 3.2));
    // don't look at me, idk man
    d *= prelook.b;
    d += length(prelook) / 4.0;
    d -= length(s) / 4.0;

    // second texture fetch which we will use for the output
    vec2 displace_amount = vec2(-0.001);
    tc += vec2(d, e) * displace_amount;
    tc += (offs * amt);

    frag_color = texture(tex0,  tc);

    // frag_color += 0.0187;
    // frag_color.b += 0.0175 * length(look.ba);
    // frag_color.r += 0.0175 * length(look.rg);

    // frag_color += vec4(hsv2rgb(vec3(look.r * look.g, look.b * look.a, 1.0)), 0.0) / 60.0;
    frag_color += vec4(hsv2rgb(vec3(1.67 - (2.0 * length(look.rb)), 1.0, 1.0)), 1.0) * 0.0187;

    // spatial differencing using intermediate pixel value (`prelook`)
    frag_color -= (prelook / 20.0);
}
