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
uniform sampler2D TexUnit2;
uniform bool skybox_active;
uniform bool multitexturing;

uniform mat4 gView;
uniform vec3 gEyePosW;

uniform float gTime;

void main(void)
{	
	vec3 color = vec3(0.2f);	// Ambient
	
	if(skybox_active)
	{
		OutColor = texture(TexUnit, OutTex);
	}
	else
	{
		vec3 normal = normalize(OutNormalW);

		// Only use the effect off one light
		for(int i = 0; i < 1; i++)
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

		if(multitexturing)
		{
			float height = (30 - OutPosW.y) / 20;
			height = clamp(height, 0, 1);

			float water = (5 - OutPosW.y) / 5;
			water = clamp(water, 0, 1);

			OutColor = (water * vec4(0, 0, 0.8, 1) + height * texture(TexUnit, OutTex) + (1-height) * texture(TexUnit2, OutTex)) * vec4(color, 1.0);		
		}
		else
		{
			OutColor = texture(TexUnit, OutTex) * vec4(color, 1.0);
			//OutColor = vec4(OutNormalW, 1.0f);
		}	
	}	
}
