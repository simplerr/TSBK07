#version 150

in vec3 InPosL;			// Local position
in vec3 InNormalL;		// Local normal
in vec2 InTex;			// Texture coordinate

out vec3 OutPosW;		// World position
out vec3 OutNormalW;	// World normal
out vec2 OutTex;		// Pass through

uniform mat4 gWorld;
uniform mat4 gWorldInvTranspose;
uniform mat4 gView;
uniform mat4 gProjection;

void main(void)
{
	// Transform to world space
	OutNormalW = mat3(transpose(inverse(gWorld)))  * InNormalL;
	OutPosW = (gWorld * vec4(InPosL, 1.0f)).xyz;

	// Homogeneous clip space transformation	
	gl_Position =  gProjection * gView * gWorld * vec4(InPosL, 1.0);
	
	// Texture coordinate
	OutTex = InTex;
}
