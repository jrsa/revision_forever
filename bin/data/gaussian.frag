uniform sampler2DRect tex0;

vec4 outputColor;

void main(void) {
  vec2 texCoord0 = gl_TexCoord[0].xy;

  vec2 tc0 = texCoord0 + vec2(-1.0, -1.0);
  vec2 tc1 = texCoord0 + vec2( 0.0, -1.0);
  vec2 tc2 = texCoord0 + vec2( 1.0, -1.0);
  vec2 tc3 = texCoord0 + vec2(-1.0,  0.0);
  vec2 tc4 = texCoord0 + vec2( 0.0,  0.0);
  vec2 tc5 = texCoord0 + vec2( 1.0,  0.0);
  vec2 tc6 = texCoord0 + vec2(-1.0,  1.0);
  vec2 tc7 = texCoord0 + vec2( 0.0,  1.0);
  vec2 tc8 = texCoord0 + vec2( 1.0,  1.0);

  vec4 col0 = texture2DRect(tex0, tc0);
  vec4 col1 = texture2DRect(tex0, tc1);
  vec4 col2 = texture2DRect(tex0, tc2);
  vec4 col3 = texture2DRect(tex0, tc3);
  vec4 col4 = texture2DRect(tex0, tc4);
  vec4 col5 = texture2DRect(tex0, tc5);
  vec4 col6 = texture2DRect(tex0, tc6);
  vec4 col7 = texture2DRect(tex0, tc7);
  vec4 col8 = texture2DRect(tex0, tc8);

  vec4 sum = (1.0 * col0 + 2.0 * col1 + 1.0 * col2 +  
              2.0 * col3 + 4.0 * col4 + 2.0 * col5 +
              1.0 * col6 + 2.0 * col7 + 1.0 * col8) / 16.0;  

  gl_FragColor = vec4(sum.rgb, 1.0);
}
