#version 330 core

uniform sampler2D tex0;
uniform float blurAmt;
uniform vec2 dim;

in vec2 texcoord_v;

out vec4 color;

void main(void) {
  vec2 texCoord0 = texcoord_v;

  vec2 step = 1.0 / dim;

  vec2 tc0 = texCoord0 + vec2(-blurAmt, -blurAmt) * step;
  vec2 tc1 = texCoord0 + vec2( 0.0, -blurAmt) * step;
  vec2 tc2 = texCoord0 + vec2( blurAmt, -blurAmt) * step;
  vec2 tc3 = texCoord0 + vec2(-blurAmt,  0.0) * step;
  vec2 tc4 = texCoord0 + vec2( 0.0,  0.0) * step;
  vec2 tc5 = texCoord0 + vec2( blurAmt,  0.0) * step;
  vec2 tc6 = texCoord0 + vec2(-blurAmt,  blurAmt) * step;
  vec2 tc7 = texCoord0 + vec2( 0.0,  blurAmt) * step;
  vec2 tc8 = texCoord0 + vec2( blurAmt,  blurAmt) * step;

  vec4 col0 = texture(tex0, tc0);
  vec4 col1 = texture(tex0, tc1);
  vec4 col2 = texture(tex0, tc2);
  vec4 col3 = texture(tex0, tc3);
  vec4 col4 = texture(tex0, tc4);
  vec4 col5 = texture(tex0, tc5);
  vec4 col6 = texture(tex0, tc6);
  vec4 col7 = texture(tex0, tc7);
  vec4 col8 = texture(tex0, tc8);

  color = (col4 + col2 + col0 + col6 + col8) * 0.2;
}
