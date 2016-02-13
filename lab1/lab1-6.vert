#version 150

uniform mat4 myMatrix;
in  vec3 in_Position;
in vec3 in_Normal;
in vec3 in_Color;
out vec3 ex_Color;

void main(void)
{
	ex_Color = mat3(myMatrix) * in_Normal;
	gl_Position = myMatrix * vec4(in_Position, 1.0);	
}
