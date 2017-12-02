#version 330 core

//Andrew Benson - andrewb@cycling74.com
//2009

// texcoords
in vec2 texcoord0;
in vec2 texcoord1;

out vec4 color;

// samplers
uniform sampler2DRect tex0;
uniform sampler2DRect tex1;

//variables
uniform vec2 scale;
uniform vec2 offset;
uniform float lambda;

const vec4 lumcoeff = vec4(0.212, 0.7152, 0.0722, 0.0); //hd
// const vec4 lumcoeff = vec4(0.299, 0.587, 0.114, 0.0); //sd

// entry point
void main()
{   
	vec4 a = texture(tex0, texcoord0);
	vec4 b = texture(tex1, texcoord1);

	//?luma convert?
	a = vec4(vec3(dot(a, lumcoeff)), 1.0);
	b = vec4(vec3(dot(b, lumcoeff)), 1.0);

	vec2 x1 = vec2(offset.x,0.);
	vec2 y1 = vec2(0.,offset.y);

	//get the difference
	vec4 curdif = b-a;
	
	//calculate the gradient
	vec4 gradx = texture(tex1, texcoord1+x1)-texture(tex1, texcoord1-x1);
	gradx += texture(tex0, texcoord0+x1)-texture(tex0, texcoord0-x1);
	
	vec4 grady = texture(tex1, texcoord1+y1)-texture(tex1, texcoord1-y1);
	grady += texture(tex0, texcoord0+y1)-texture(tex0, texcoord0-y1);
	
	vec4 gradmag = sqrt((gradx*gradx)+(grady*grady)+vec4(lambda));

	vec4 vx = curdif*(gradx/gradmag);
	float vxd = vx.r;//assumes greyscale
	//format output for flowrepos, out(-x,+x,-y,+y)
	vec2 xout = vec2(max(vxd,0.),abs(min(vxd,0.)))*scale.x;

	vec4 vy = curdif*(grady/gradmag);
	float vyd = vy.r;//assumes greyscale
	//format output for flowrepos, out(-x,+x,-y,+y)
	vec2 yout = vec2(max(vyd,0.),abs(min(vyd,0.)))*scale.y;
	
	color = vec4(xout.xy,yout.xy);
}
