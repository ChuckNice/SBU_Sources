#version 120

uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;

attribute vec3 a_Vertex;
attribute vec4 a_Color;

varying vec4 color;

void main(void) 
{
	vec4 pos = modelview_matrix * vec4(a_Vertex, 1.0);	
	color = a_Color;
	
	gl_Position = projection_matrix * pos;		
}
