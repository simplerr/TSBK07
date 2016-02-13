#version 150

in vec3 ex_Color;
in vec2 outTexCoord;
out vec4 out_Color;

void main(void)
{
	// Test this some more (TODO)
	float a = sin(outTexCoord.s*30)/2+0.5;
	float b = sin(outTexCoord.t*30)/2+0.5;
	out_Color = vec4(a, b, 1.0, 0.0);
	//out_Color = vec4(outTexCoord.x, outTexCoord.y, 1.0, 1.0);
}
