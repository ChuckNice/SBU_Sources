
attribute vec3 a_Vertex;
attribute vec3 a_Normal;
attribute vec2 a_TexCoord0;

varying vec4 ECPosition;
varying vec3 ECNormal;
varying vec2 texCoord0;

uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;
uniform mat3 normal_matrix;

void main()
{
	// calculate eye space position and normal for fragment shader
    ECPosition = modelview_matrix * vec4(a_Vertex, 1.0);
    ECNormal = normalize(normal_matrix * a_Normal);

	// pass on vertex position and texcoord
	gl_Position = projection_matrix * ECPosition;
	texCoord0 = a_TexCoord0;
}
