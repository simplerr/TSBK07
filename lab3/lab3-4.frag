#version 150

in vec3 OutPosW;
in vec3 OutNormalW;
in vec2 OutTex;

out vec4 OutColor;

// Lighting data
uniform vec3 lightSourcesDirPosArr[4];
uniform vec3 lightSourcesColorArr[4];
uniform float specularExponent[4];
uniform bool isDirectional[4];

uniform sampler2D TexUnit;
uniform bool skybox_active;

uniform mat4 gView;
uniform vec3 gEyePosW;

void main(void)
{	
	vec3 color = vec3(0.1f);	// Ambient
	
	if(skybox_active)
	{
		OutColor = texture(TexUnit, OutTex);
	}
	else
	{
		vec3 normal = normalize(OutNormalW);

		// Add the effect of all lights
		for(int i = 0; i < 4; i++)
		{
			vec3 lightDir;
			if(isDirectional[i]) {
				lightDir = normalize(lightSourcesDirPosArr[i]);
			}
			else {
				lightDir = normalize(lightSourcesDirPosArr[i] - OutPosW);
			} 
	
			// Diffuse
			float shade = clamp(dot(normal, lightDir), 0, 1);

			if(shade > 0)
			{	
				vec3 diffuse = shade * lightSourcesColorArr[i];
				color += diffuse;

				// Specular
				vec3 eyeVector = normalize(gEyePosW - OutPosW);

				vec3 v = reflect(-lightDir, normal);
				shade = pow(max(dot(v, eyeVector), 0.0f), specularExponent[i]); 
				vec3 spec = shade * lightSourcesColorArr[i];
				color += spec;
			}
		}	

		OutColor = texture(TexUnit, OutTex) * vec4(color, 1.0);
	}	
}
