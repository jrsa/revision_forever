//generates a normal map based on edge detection
uniform vec2 mouse;
uniform vec2 res;
uniform sampler2DRect tex0;
uniform float lightWidth;
uniform float lightBrightness;
uniform float normalStrength;
uniform float shinAmnt;

//vec2 texelWidth = 1.0/res;
vec2 texelWidth = vec2(1.0, 1.0); 

void main(){

	vec2 uv = gl_TexCoord[0].xy; //tex coords
    vec4 sourceColor = texture2DRect(tex0, uv);

	float step = 1.0;
    float tl = abs(texture2DRect(tex0, uv + texelWidth * vec2(-step, -step)).x);  // top left
    float  l = abs(texture2DRect(tex0, uv + texelWidth * vec2(-step,   0.0)).x);   // left
    float bl = abs(texture2DRect(tex0, uv + texelWidth * vec2(-step,  step)).x);  // bottom left
    float  t = abs(texture2DRect(tex0, uv + texelWidth * vec2( 0.0,  -step)).x);   // top
    float  b = abs(texture2DRect(tex0, uv + texelWidth * vec2( 0.0,   step)).x);   // bottom
    float tr = abs(texture2DRect(tex0, uv + texelWidth * vec2( step, -step)).x);  // top right
    float  r = abs(texture2DRect(tex0, uv + texelWidth * vec2( step,   0.0)).x);   // right
    float br = abs(texture2DRect(tex0, uv + texelWidth * vec2( step,  step)).x);  // bottom right

    //edge detect with sobel kernel
	float mult = 0.01;
	float dX = tr*mult + 2.0*r*mult + br*mult -tl*mult - 2.0*l*mult - bl*mult;
    float dY = bl*mult + 2.0*b*mult + br*mult -tl*mult - 2.0*t*mult - tr*mult;

    //color like a normal map
    vec3 color = normalize(vec3(dX, dY, 1.0/normalStrength));

    color *=0.5;
    color +=0.5;

    //vec3 lightDir = vec3(vec2(mouse.x / res.x, 1.0-mouse.y / res.y) - (uv.xy / res), 1.0);
    vec3 lightDir = vec3(vec2(mouse.x+lightWidth/2.0, (res.y-mouse.y)+lightWidth/2.0) - uv, lightWidth);

    float D = length(lightDir.xyz);

    vec3 N = normalize(color);
    vec3 L = normalize(lightDir); //light direction
    //vec3 H = normalize(lightDir); //half normal
    vec3 H = normalize(lightDir * vec3(0.5, 0.5, 0.5)); //half normal

    //phong lighting equation
    vec3 diffuse = (sourceColor.rgb * sourceColor.a) * max(dot(N, L), 0.0);

    float shin = 300.0;
    float sf = max(0.0, dot(N, H));
    sf = pow(sf, shin);
    sf *= shinAmnt;

    //normal map output
    //gl_FragColor = vec4(color.rgb, 1.0);

    //ferriss output
	// gl_FragColor = vec4(vec3(diffuse + sf), 1.0);

    //////////////////////////
    //added from ezmill shader
    vec4 ambientColor = vec4(vec3(sourceColor.rgb*lightBrightness), 0.5);
    vec3 ambient = ambientColor.rgb * ambientColor.a;

    vec3 falloff = vec3(0.02, 0.5, 1.5);
    //vec3 falloff = vec3(1.0, 3.0, 20.5);

    float attenuation = 1.0 / (falloff.x + (falloff.y * D) + (falloff.z * D * D));
    vec3 intensity = ambient+(diffuse+sf) * attenuation;
    vec3 finalColor = (sourceColor.rgb * intensity);
    
    vec3 col = ambient + (finalColor+sf);

    //ezmill output
    gl_FragColor = vec4(col, 1.0);
    //////////////////////////
}