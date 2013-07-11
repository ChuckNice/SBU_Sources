#version 120

varying vec4 ECPosition;
varying vec3 ECNormal;
varying vec2 texCoord0;

uniform vec4 ECLight0Position;
uniform vec4 ECCamPosition;
uniform float ambient;
uniform float diffuse;
uniform int specular;
uniform vec4 diffuseColor;
uniform sampler2D texture0;

void main(void) {
	vec3 PosToLight = ECLight0Position.xyz - ECPosition.xyz;
	float distToLight = length(PosToLight);
	PosToLight = normalize(PosToLight);
	vec3 CamToPos = normalize(ECPosition.xyz - ECCamPosition.xyz);
    
	// calculate diffuse lighting
	float diffuseVal = max(dot(PosToLight, ECNormal), 0.0);
	float lightIntensity = diffuseVal * diffuse; //diffuse

	// calculate specular lighting
	if (specular > 0) {
		vec3 ECReflection = normalize(2.0 * dot(ECNormal, -PosToLight) * ECNormal + PosToLight);
		float specularVal = max(dot(ECReflection, CamToPos), 0.0);
		lightIntensity += pow(specularVal, float(specular));
	}

	// factor in linear attenuation (distance to light)
	float minDist = 100.0;
	float maxDist = 15.0;
	lightIntensity *= max(minDist - distToLight, 0.0) / minDist;

	// calculate ambient lighting that is unaffected by attenuation
	lightIntensity += ambient;

	// get texel for this fragment
	vec4 texColor = texture2D(texture0, texCoord0.st);
    
    // apply lighting and texture to color
    vec4 lit_color = clamp(diffuseColor * texColor * lightIntensity, 0.0, 1.0);
	lit_color.a = diffuseColor.a;
    gl_FragColor = lit_color;
}
