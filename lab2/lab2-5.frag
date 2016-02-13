#version 150

in vec3 ex_Color;
in vec2 outTexCoord;
in vec3 ex_Normal;
out vec4 out_Color;

uniform sampler2D texUnit;

void main(void)
{	
	vec3 light = vec3(0.58, 0.58, 0.58);

	light = normalize(light);
	float shade = clamp(dot(ex_Normal, light), 0, 1);


	out_Color = vec4(vec3(shade), 1.0) * texture(texUnit, outTexCoord);	
}
