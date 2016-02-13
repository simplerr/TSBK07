#version 150

in vec3 ex_Color;
in vec2 outTexCoord;
in vec3 ex_Normal;
out vec4 out_Color;

uniform sampler2D texUnit;
uniform int skybox_active;

void main(void)
{	
	if(skybox_active)
	{
		out_Color = texture(texUnit, outTexCoord);
	}
	else
	{
		vec3 light = vec3(0.58, 0.58, 0.58);
		float shade = clamp(dot(ex_Normal, normalize(light)), 0, 1);

		out_Color = shade * texture(texUnit, outTexCoord);	
	}	
}
