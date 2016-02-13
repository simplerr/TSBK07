#version 150

in  vec3 in_Position;
in vec3 in_Normal;
in vec3 in_Color;
in vec2 inTexCoord;
out vec3 ex_Color;
out vec2 outTexCoord;

uniform mat4 mdlMatrix;
uniform mat4 projMatrix;

void main(void)
{
	ex_Color = mat3(mdlMatrix) * in_Normal;
	gl_Position =   projMatrix * mdlMatrix * vec4(in_Position, 1.0);	
	outTexCoord = inTexCoord;
}
