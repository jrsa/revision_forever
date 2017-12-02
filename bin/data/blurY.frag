#version 330 core

uniform sampler2DRect tex0;
uniform float blurAmnt;

in vec2 texcoord_v;

out vec4 frag_color;

void main()
{
    vec2 texCoord0 = texcoord_v;

    frag_color = 1.0 * texture(tex0, texCoord0 + vec2(0.0, blurAmnt * -4.0));
    frag_color += 2.0 * texture(tex0, texCoord0 + vec2(0.0, blurAmnt * -3.0));
    frag_color += 3.0 * texture(tex0, texCoord0 + vec2(0.0, blurAmnt * -2.0));
    frag_color += 4.0 * texture(tex0, texCoord0 + vec2(0.0, blurAmnt * -1.0));
    
    frag_color += 5.0 * texture(tex0, texCoord0 + vec2(0.0, blurAmnt));
    
    frag_color += 4.0 * texture(tex0, texCoord0 + vec2(0.0, blurAmnt * 1.0));
    frag_color += 3.0 * texture(tex0, texCoord0 + vec2(0.0, blurAmnt * 2.0));
    frag_color += 2.0 * texture(tex0, texCoord0 + vec2(0.0, blurAmnt * 3.0));
    frag_color += 1.0 * texture(tex0, texCoord0 + vec2(0.0, blurAmnt * 4.0));
    
    frag_color /= 25.0;
}