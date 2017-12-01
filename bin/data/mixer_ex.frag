uniform sampler2DRect tex0;
uniform sampler2DRect tex1;
uniform sampler2DRect tex2;

uniform float mixAmnt;

vec4 outputColor;

void main()
{
    vec2 texCoord0 = gl_TexCoord[0].xy;

    vec4 tex0_color = texture2DRect(tex0, texCoord0);
    vec4 tex1_color = texture2DRect(tex1, texCoord0);

    vec4 flow_map = texture2DRect(tex2, texCoord0);
    
    gl_FragColor = mix(tex0_color, tex1_color, mixAmnt * flow_map.a);
}
