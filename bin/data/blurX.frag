uniform sampler2DRect tex0;
uniform float blurAmnt;

vec4 outputColor;

void main()
{
    vec2 texCoord0 = gl_TexCoord[0].xy;
    vec4 color;
    
    color = 1.0 * texture2DRect(tex0, texCoord0 + vec2(blurAmnt * -4.0, 0.0));
    color += 2.0 * texture2DRect(tex0, texCoord0 + vec2(blurAmnt * -3.0, 0.0));
    color += 3.0 * texture2DRect(tex0, texCoord0 + vec2(blurAmnt * -2.0, 0.0));
    color += 4.0 * texture2DRect(tex0, texCoord0 + vec2(blurAmnt * -1.0, 0.0));
    
    color += 5.0 * texture2DRect(tex0, texCoord0 + vec2(blurAmnt, 0.0));
    
    color += 4.0 * texture2DRect(tex0, texCoord0 + vec2(blurAmnt * 1.0, 0.0));
    color += 3.0 * texture2DRect(tex0, texCoord0 + vec2(blurAmnt * 2.0, 0.0));
    color += 2.0 * texture2DRect(tex0, texCoord0 + vec2(blurAmnt * 3.0, 0.0));
    color += 1.0 * texture2DRect(tex0, texCoord0 + vec2(blurAmnt * 4.0, 0.0));
    
    color /= 25.0;
    
    gl_FragColor = color;
}