#version 120

varying vec4 ECPosition;
varying vec3 ECNormal;

uniform vec4 ECLight0Position;
uniform vec4 ECCamPosition;
uniform float ambient;
uniform float diffuse;
uniform int specular;
uniform vec4 diffuseColor;

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

	// add ambient light that is unaffected by attenuation
	lightIntensity += ambient;
    
    // apply lighting to color
    vec4 lit_color = clamp(diffuseColor * lightIntensity, 0.0, 1.0);
	lit_color.a = diffuseColor.a;
    gl_FragColor = lit_color;
}
