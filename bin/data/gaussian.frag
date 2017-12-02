#version 330 core

uniform sampler2DRect tex0;
uniform float blurAmt;

in vec2 texcoord_v;

out vec4 color;

void main(void) {
  // vec2 texCoord0 = gl_TexCoord[0].xy;
  vec2 texCoord0 = texcoord_v;

  vec2 tc0 = texCoord0 + vec2(-blurAmt, -blurAmt);
  vec2 tc1 = texCoord0 + vec2( 0.0, -blurAmt);
  vec2 tc2 = texCoord0 + vec2( blurAmt, -blurAmt);
  vec2 tc3 = texCoord0 + vec2(-blurAmt,  0.0);
  vec2 tc4 = texCoord0 + vec2( 0.0,  0.0);
  vec2 tc5 = texCoord0 + vec2( blurAmt,  0.0);
  vec2 tc6 = texCoord0 + vec2(-blurAmt,  blurAmt);
  vec2 tc7 = texCoord0 + vec2( 0.0,  blurAmt);
  vec2 tc8 = texCoord0 + vec2( blurAmt,  blurAmt);

  vec4 col0 = texture(tex0, tc0);
  vec4 col1 = texture(tex0, tc1);
  vec4 col2 = texture(tex0, tc2);
  vec4 col3 = texture(tex0, tc3);
  vec4 col4 = texture(tex0, tc4);
  vec4 col5 = texture(tex0, tc5);
  vec4 col6 = texture(tex0, tc6);
  vec4 col7 = texture(tex0, tc7);
  vec4 col8 = texture(tex0, tc8);

  vec4 sum = (1.0 * col0 + 2.0 * col1 + 1.0 * col2 +  
              2.0 * col3 + 4.0 * col4 + 2.0 * col5 +
              1.0 * col6 + 2.0 * col7 + 1.0 * col8) / 16.0;  

  color = vec4(sum.rgb, 1.0);
}
