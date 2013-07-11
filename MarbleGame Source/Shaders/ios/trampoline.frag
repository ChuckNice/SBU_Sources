precision mediump float;

varying vec4 ECPosition;
varying vec3 ECNormal;
varying vec4 color;

uniform vec4 ECLight0Position;

void main(void) {
	float lightIntensity = 0.0;

	// calculate ambient lighting
	lightIntensity = 0.2; //ambient
    
	// calculate diffuse lighting
	float diffuseVal = max(dot(normalize(ECLight0Position.xyz - ECPosition.xyz), ECNormal), 0.0);
	lightIntensity += diffuseVal * 1.0; //diffuse
    
    // apply lighting to color
    vec4 lit_color = clamp(color * lightIntensity, 0.0, 1.0);
    gl_FragColor = lit_color;
}
