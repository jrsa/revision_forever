uniform sampler2DRect tex0;

uniform float contrast;

void main(void) {

  vec2 texCoord0 = gl_TexCoord[0].xy;

  vec4 color = texture2DRect(tex0, texCoord0);

  const vec4 lum = vec4(0.2125, 0.7154, 0.0721, 1.0);
  vec4 avg = vec4(0.5, 0.5, 0.5, 1.0);
  vec4 intensity = vec4(dot(color, lum));

  vec4 sat = mix(intensity, color, 1.0);
  vec4 con = mix(avg, sat, contrast);

  gl_FragColor = con;
}