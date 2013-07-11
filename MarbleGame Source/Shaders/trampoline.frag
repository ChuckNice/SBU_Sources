#version 120

//precision highp float;

varying vec4 ECPosition;
varying vec3 ECNormal;
varying vec4 color;
varying vec2 texCoord0;

uniform vec4 ECLight0Position;
uniform sampler2D texture0;

void main(void) {
    vec3 PosToLight = ECLight0Position.xyz - ECPosition.xyz;
	float distToLight = length(PosToLight);
    
	// calculate diffuse lighting
	float diffuseVal = max(dot(normalize(ECLight0Position.xyz - ECPosition.xyz), ECNormal), 0.0);
	float lightIntensity = diffuseVal * 1.0; //diffuse
    
    // factor in linear attenuation (distance to light)
	float minDist = 100.0;
	float maxDist = 15.0;
	lightIntensity *= max(minDist - distToLight, 0.0) / minDist;
    
    lightIntensity += 0.2; //ambient
    
    // get texel for this fragment
	vec4 texColor = texture2D(texture0, texCoord0.st);
    
    // apply lighting to color
    vec4 lit_color = clamp(color * texColor * lightIntensity, 0.0, 1.0);
    gl_FragColor = lit_color;
}
