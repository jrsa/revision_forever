vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

uniform sampler2DRect tex0;

uniform float hue;
uniform float sv;

void main(void) {
  vec2 texCoord0 = gl_TexCoord[0].xy;

  vec4 color = texture2DRect(tex0, texCoord0);
  vec4 hsv = vec4(rgb2hsv(color.xyz), 1.0);

  if (hsv.x < 1.0){
    hsv.x += hue;
  } else {
    hsv.x = 0.0;
  }

  hsv.y *= sv;
  hsv.z *= sv;

  vec4 rgb = vec4(hsv2rgb(hsv.xyz), 1.0);

  gl_FragColor = rgb;
}