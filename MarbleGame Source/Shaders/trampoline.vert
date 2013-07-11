#version 120

attribute vec3 a_Vertex;
attribute vec4 a_Color;
attribute vec3 a_Normal;
attribute vec2 a_TexCoord0;

varying vec4 ECPosition;
varying vec3 ECNormal;
varying vec4 color;
varying vec2 texCoord0;

uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;
uniform mat3 normal_matrix;

uniform vec3 MaxPoint;		// the most positive point of the trampoline model
uniform vec3 ContactPoint;	// the contact point on the trampoline (IN MODEL SPACE!)
uniform float Amplitude;		// the max amount that the trampoline can dip (will reach this point at ContactPoint)
uniform float DipPercent;	// the percent (0.0->1.0) of the full dip that should be displayed (modulate this for animation)

void main()
{
	vec3 NewVertex = a_Vertex.xyz;
	vec3 NewNormal = a_Normal;

	if (DipPercent > 0.0)
	{ 
		// compare model vertex with model contact point to get closest corner vertex
		vec3 cornerPoint = MaxPoint;
		vec3 compareVec = NewVertex - ContactPoint;
		if (compareVec.x < 0) { cornerPoint.x = -MaxPoint.x; }
		if (compareVec.z < 0) { cornerPoint.z = -MaxPoint.z; }

		// if this vertex is not on the edge of the trampoline...
		if (a_Vertex.x != cornerPoint.x && a_Vertex.z != cornerPoint.z)
		{
			// get distance to contact point and distance from contact to corner
			float contactDist = length(NewVertex - ContactPoint);
			float cornerDist = length(cornerPoint - ContactPoint);
			if (cornerDist != 0.0)
			{
				// transform the vertex based on ratio of (distance to contact point) / (distance from contact to corner)
				float ratio = 1.0 - (contactDist / cornerDist);
				NewVertex += a_Normal * -ratio * Amplitude * DipPercent;

				// transform normal based on the ratio
				//NewNormal.x = ContactPoint.x;
				//NewNormal.y = ratio * 2.0 * Amplitude * DipPercent;
				//NewNormal.z = ContactPoint.z;
			}
		}
	}

	// calculate eye space position and normal for fragment shader
    ECPosition = modelview_matrix * vec4(NewVertex, 1.0);
    ECNormal = normalize(normal_matrix * NewNormal);

	// pass on vertex position and color
	gl_Position = projection_matrix * ECPosition;	
	color = a_Color;
    texCoord0 = a_TexCoord0;
}
