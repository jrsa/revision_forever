#version 330 core

uniform sampler2DRect tex0;
uniform sampler2DRect tex1;
uniform sampler2DRect tex2;

uniform float mixAmnt;

in vec2 texcoord_v;

out vec4 frag_color;

void main()
{
    vec4 tex0_color = texture(tex0, texcoord_v);
    vec4 tex1_color = texture(tex1, texcoord_v);

    vec4 flow_map = texture(tex2, texcoord_v);
    
    frag_color = mix(tex0_color, tex1_color, mixAmnt * flow_map.a);
}
