#version 150

uniform mat4 myMatrix;
in  vec3 in_Position;

void main(void)
{
	gl_Position = myMatrix * vec4(in_Position, 1.0);
}
