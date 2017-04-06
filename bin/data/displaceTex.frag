uniform sampler2DRect tex1;
uniform sampler2DRect tex2;

uniform float time;
uniform float displaceAmt;

uniform vec2 resolution;

varying vec2 TexCoord;
varying vec3 N;
varying vec3 P;
varying vec3 V;
varying vec3 L;

void main(){
    vec2 displace_uv = vec2(TexCoord.x + time, TexCoord.y + time);
    if (displace_uv.x > resolution.x){
    	displace_uv.x -= resolution.x;
    }
    if (displace_uv.y > resolution.y){
    	displace_uv.y -= resolution.y;
    }

    vec2 displace = texture2DRect(tex2, displace_uv).xy;
    displace = ((displace * (2.0 * resolution.x)) 
    					  - (1.0 * resolution.x)) * (displaceAmt*0.0001);

    gl_FragColor = texture2DRect(tex1, TexCoord + displace);

    //////////////////////////////////////////////////////

    vec3 AmbientColour = texture2DRect(tex1, TexCoord + displace).rgb;
    vec3 DiffuseColour = vec3(0.5, 0.5, 0.4);
    vec3 SpecularColour = vec3(0.9, 0.9, 0.9);
    float AmbientIntensity = 1.0;
    float DiffuseIntensity = 0.4;
    float SpecularIntensity = 0.2;

    vec3 l = normalize(L);
    vec3 n = normalize(N);
    vec3 v = normalize(V);
    vec3 h = normalize(l+v);

    vec3 E = normalize(V);
    vec3 R = reflect(-l, n);
    float specular = pow(max(dot(R, E), 0.0), 
                        SpecularIntensity);

    float diffuse = dot(l,n);

    vec4 finalColor = vec4(AmbientColour * AmbientIntensity, 1.0);

    if (diffuse > -1.0){ //remove if (or check low number) for inverse diffuse color on inverted normals
        finalColor.rgb += vec3(DiffuseColour * diffuse * DiffuseIntensity +
                               SpecularColour* specular* SpecularIntensity);
    }

    gl_FragColor = finalColor;
}
