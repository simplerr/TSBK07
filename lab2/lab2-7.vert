#version 150

in  vec3 in_Position;
in vec3 in_Normal;
in vec3 in_Color;
in vec2 inTexCoord;
out vec3 ex_Color;
out vec3 ex_Normal;
out vec2 outTexCoord;

uniform mat4 mdlMatrix;
uniform mat4 projMatrix;
uniform mat4 cameraMatrix;

void main(void)
{
	vec3 normal = mat3(mdlMatrix)  * in_Normal;
	normal = normalize(normal);
	ex_Normal = normal;
	
	gl_Position =   projMatrix * cameraMatrix * mdlMatrix * vec4(in_Position, 1.0);	
	outTexCoord = inTexCoord;
}
