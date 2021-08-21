#version 450
/*
 * 1 VertexShader
 */
in layout (location = 0) vec4 position;
in layout (location = 1) vec4 normal;
in layout (location = 2)  vec2 tex;

uniform vec3 cameraPos;
uniform vec3 lightPos;
uniform mat4 viewMatrix;

out vec3 passPosition_c;
out vec3 passNormal_c;
out vec2 passTex_c;
out float visibility;

const float density = 0.1;
const float gradient = 1;

void main()
{
    passNormal_c = normalize( normal.xyz  );
    passPosition_c = position.xyz;
    passTex_c = tex;

	// fog effect
//	vec4 posRelativeToCam = viewMatrix * vec4(passPosition_c, 1.0);
//	float dist = length(posRelativeToCam.xyz);
//	visibility = clamp(exp(-pow((dist * density), gradient)), 0, 1);

}