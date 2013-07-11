precision mediump float;

uniform vec4 color;
varying vec2 texCoord0;

void main(void) {
	// calculate alpha with radial gradient
	vec2 fragCoord = texCoord0 - 0.5;
    float radial = (0.5 - length(fragCoord)) * 2.0;

	gl_FragColor = vec4(color.xyz, color.a * radial);
}
