uniform sampler2DRect tex0;
uniform sampler2DRect tex1;

uniform float hueAngleOffset;
uniform float flowScaleScale;

// uniform vec2 step;
uniform float kernel[9];

vec2 offset[9];
vec4 sum = vec4(0.0);
float kernelWeight = 0.0;

vec3 rgb2hsv(vec3 c) {
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
	vec2 uv = gl_TexCoord[0].xy;

	vec3 hsvIn = rgb2hsv(texture2DRect(tex0, uv).rgb);
    vec3 flow = rgb2hsv(texture2DRect(tex1, uv).rgb);
	float angle = (flow.r + hueAngleOffset) * 0.009;
    
    float scaleAmt = flow.g * flowScaleScale;
    mat2 scale = mat2(1., scaleAmt, scaleAmt, 1.);
	mat2 rot = mat2(cos(angle), sin(angle),  -sin(angle), cos(angle));
	vec2 warpedUv = uv * rot;
    warpedUv *= scale;

	vec2 step = vec2(hsvIn.g);// * 2.0;// * step;

	offset[0] = vec2(-step.x, -step.y); // top left
	offset[1] = vec2(    0.0, -step.y); // top middle
	offset[2] = vec2( step.x, -step.y); // top right
	offset[3] = vec2(-step.x,     0.0); // middle left
	offset[4] = vec2(    0.0,     0.0); //middle
	offset[5] = vec2( step.x,     0.0); //middle right
	offset[6] = vec2(-step.x,  step.y); //bottom left
	offset[7] = vec2(    0.0,  step.y); //bottom middle
	offset[8] = vec2( step.x,  step.y); //bottom right

	for(int i = 0; i<9; i++){
		//sample a 3x3 grid around each pixel and multiply by values
		vec4 color = texture2DRect(tex0, warpedUv + offset[i]);
		sum += color * kernel[i];
		kernelWeight += kernel[i];
	}

	// if sum of the kernel values is <= 0  set it to 1
	kernelWeight = kernelWeight <= 0.0 ? 1.0 : kernelWeight;
		
	gl_FragColor = vec4(sum.rgb/kernelWeight, 1.0);
}