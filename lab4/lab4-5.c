// Lab 4, terrain generation

#ifdef __APPLE__
	#include <OpenGL/gl3.h>
	// Linking hint for Lightweight IDE
	// uses framework Cocoa
#endif
//#include "MicroGlut.h"
#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "loadobj.h"
#include "LoadTGA.h"

float GetHeight(float x_world, float z_world);
Model* GenerateTerrain(TextureData *tex);
void mouse(int x, int y);

mat4 projectionMatrix;

// Camera stuff
vec3 cameraPos = { 30, 0, 0 };
vec3 targetPos = { 0, 0, 0 };
vec3 upVector = { 0, 1, 0 };
float pitch = 0.0f;
float yaw = -3.14 / 2.0;

GLfloat* vertexArray;
TextureData* terrainTexture;
Model* sphere;

vec3 sphere_pos = { 20, 20, 20 };

GLuint grass_texture, conc_texture;

/*
	Light data
*/
Point3D lightSourcesColorsArr[] = {
	{ 1.0f, 1.0f, 1.0f }, // White light (the only one used)
	{ 0.0f, 1.0f, 0.0f }, // Green light
	{ 0.0f, 0.0f, 1.0f }, // Blue light
	{ 1.0f, 1.0f, 1.0f }  // White light
};

GLfloat specularExponent[] = { 150.0, 20.0, 60.0, 5.0 };
GLint isDirectional[] = { 0,0,1,1 };

Point3D lightSourcesDirectionsPositions[] = {
	{ 1.0f, 100.0f, 100.0f }, // White, directional
	{ 0.0f, 5.0f, 10.0f }, // Green light, positional
	{ -1.0f, 0.0f, 0.0f }, // Blue light along X
	{ 0.0f, 0.0f, -1.0f }  // White light along Z
};



// vertex array object
Model *m, *m2, *tm;
// Reference to shader program
GLuint program;
GLuint tex1, tex2;
TextureData ttex; // terrain

void init(void)
{
	// GL inits
	glClearColor(0.2,0.2,0.5,0);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	printError("GL inits");

	initKeymapManager();

	projectionMatrix = frustum(-0.1, 0.1, -0.1, 0.1, 0.2, 500.0);

	// Load and compile shader
	program = loadShaders("terrain.vert", "terrain.frag");
	glUseProgram(program);
	printError("init shader");
	
	// Upload light data to the shader
	glUniform3fv(glGetUniformLocation(program, "lightSourcesDirPosArr"), 4, &lightSourcesDirectionsPositions[0].x);
	glUniform3fv(glGetUniformLocation(program, "lightSourcesColorArr"), 4, &lightSourcesColorsArr[0].x);
	glUniform1fv(glGetUniformLocation(program, "specularExponent"), 4, specularExponent);
	glUniform1iv(glGetUniformLocation(program, "isDirectional"), 4, isDirectional);

	glUniformMatrix4fv(glGetUniformLocation(program, "gProjection"), 1, GL_TRUE, projectionMatrix.m);
	glUniform1i(glGetUniformLocation(program, "TexUnit"), 0); // Texture unit 0
	LoadTGATextureSimple("grass.tga", &tex1);
	
	sphere = LoadModelPlus("groundsphere.obj");

	LoadTGATextureSimple("grass.tga", &grass_texture);
	LoadTGATextureSimple("conc.tga", &conc_texture);

	// Load terrain data	
	LoadTGATextureData("fft-terrain.tga", &ttex);
	tm = GenerateTerrain(&ttex);

	glUniform1i(glGetUniformLocation(program, "TexUnit"), 0);
	glUniform1i(glGetUniformLocation(program, "TexUnit2"), 1);

	printError("init terrain");
}

void display(void)
{
	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	mat4 total, modelView;
	
	printError("pre display");
	
	glUseProgram(program);

	
	// Set camera height to terrain height
	cameraPos.y = GetHeight(cameraPos.x, cameraPos.z) + 5; // small offset

	// This is needed to not mess up the camera target (move to Camera class :P)
	vec3 dir = { cosf(pitch) * sinf(yaw),
		sinf(pitch),
		cosf(pitch) * cosf(yaw)
	};

	targetPos = VectorAdd(cameraPos, dir);

	// Upload camera matrix
	mat4 cameraMatrix = lookAt(cameraPos.x, cameraPos.y, cameraPos.z, targetPos.x, targetPos.y, targetPos.z, upVector.x, upVector.y, upVector.z);

	modelView = IdentityMatrix();
	total = Mult(cameraMatrix, modelView);
	glUniformMatrix4fv(glGetUniformLocation(program, "gWorld"), 1, GL_TRUE, modelView.m);
	glUniformMatrix4fv(glGetUniformLocation(program, "gView"), 1, GL_TRUE, cameraMatrix.m);
	
	glUniform1i(glGetUniformLocation(program, "multitexturing"), 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, grass_texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, conc_texture);
	DrawModel(tm, program, "InPosL", "InNormalL", "InTex");
	glUniform1i(glGetUniformLocation(program, "multitexturing"), 0);

	sphere_pos.y = GetHeight(sphere_pos.x, sphere_pos.z);
	sphere_pos.z += 0.1;

	glUniformMatrix4fv(glGetUniformLocation(program, "gWorld"), 1, GL_TRUE, T(sphere_pos.x, sphere_pos.y, sphere_pos.z).m);
	glBindTexture(GL_TEXTURE_2D, grass_texture);		// Bind Our Texture tex1
	DrawModel(sphere, program, "InPosL", "InNormalL", "InTex");

	printError("display 2");
	
	glutSwapBuffers();
}

void timer(int i)
{
	// Move the camera using user input
	if (keyIsDown('a'))
	{
		vec3 dir = VectorSub(targetPos, cameraPos);
		vec3 right_vec = CrossProduct(dir, upVector);
		right_vec = Normalize(right_vec);

		cameraPos = VectorSub(cameraPos, right_vec);
		targetPos = VectorSub(targetPos, right_vec);
	}
	else if (keyIsDown('d'))
	{
		vec3 dir = VectorSub(targetPos, cameraPos);
		vec3 right_vec = CrossProduct(dir, upVector);
		right_vec = Normalize(right_vec);

		cameraPos = VectorAdd(cameraPos, right_vec);
		targetPos = VectorAdd(targetPos, right_vec);
	}

	if (keyIsDown('w'))
	{
		vec3 dir = VectorSub(targetPos, cameraPos);
		dir = Normalize(dir);
		cameraPos = VectorAdd(cameraPos, dir);
		targetPos = VectorAdd(targetPos, dir);
	}
	if (keyIsDown('s'))
	{
		vec3 dir = VectorSub(targetPos, cameraPos);
		dir = Normalize(dir);
		cameraPos = VectorSub(cameraPos, dir);
		targetPos = VectorSub(targetPos, dir);
	}

	glutTimerFunc(20, &timer, i);
	glutPostRedisplay();
}

void mouse(int x, int y)
{
	static int last_x = 1024 / 2;
	static int last_y = 768 / 2;

	int delta_x = last_x - x;
	int delta_y = last_y - y;

	float camera_sensitivity = 0.005f;
	yaw += (float)delta_x * camera_sensitivity;
	pitch += (float)delta_y * camera_sensitivity;

	vec3 dir = { cosf(pitch) * sinf(yaw),
		sinf(pitch),
		cosf(pitch) * cosf(yaw)
	};

	targetPos = VectorAdd(cameraPos, dir);

	last_x = x;
	last_y = y;
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
	glutInitContextVersion(3, 2);
	glutInitWindowSize(1024, 768);
	glutCreateWindow ("TSBK07 Lab 4");
	glutDisplayFunc(display);

	if (GLEW_OK != glewInit())
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		printf("glewInit failed, aborting.\n");
		exit(1);
	}
	printf("Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	init ();
	glutTimerFunc(20, &timer, 0);

	glutPassiveMotionFunc(mouse);

	glutMainLoop();
	exit(0);
}

Model* GenerateTerrain(TextureData *tex)
{
	terrainTexture = tex;

	int vertexCount = tex->width * tex->height;
	int triangleCount = (tex->width - 1) * (tex->height - 1) * 2;
	int x, z;

	vertexArray = malloc(sizeof(GLfloat) * 3 * vertexCount);
	GLfloat *normalArray = malloc(sizeof(GLfloat) * 3 * vertexCount);
	GLfloat *texCoordArray = malloc(sizeof(GLfloat) * 2 * vertexCount);
	GLuint *indexArray = malloc(sizeof(GLuint) * triangleCount * 3);

	printf("bpp %d\n", tex->bpp);
	for (x = 0; x < tex->width; x++)
		for (z = 0; z < tex->height; z++)
		{
			// Vertex array. You need to scale this properly
			float height = tex->imageData[(x + z * tex->width) * (tex->bpp / 8)] / 5.0f;
			vertexArray[(x + z * tex->width) * 3 + 0] = x / 1.0;
			vertexArray[(x + z * tex->width) * 3 + 1] = height;
			vertexArray[(x + z * tex->width) * 3 + 2] = z / 1.0;

			// Texture coordinates. You may want to scale them.
			texCoordArray[(x + z * tex->width) * 2 + 0] = x; // (float)x / tex->width;
			texCoordArray[(x + z * tex->width) * 2 + 1] = z; // (float)z / tex->height;
		}

	// Normal vectors. You need to calculate these.
	for (x = 0; x < tex->width; x++)
	{
		for (z = 0; z < tex->height; z++)
		{
			vec3 p1, p2, p3;
			vec3 edge = { 0.0f, 0.0f, 0.0f };
			int i;

			// p1 [x-1][z-1]
			if (x < 1 && z < 1)
				i = (x + 1 + (z + 1) * tex->width) * 3;
			else
				i = (x - 1 + (z - 1) * tex->width) * 3;

			p1 = (vec3) { vertexArray[i + 0], vertexArray[i + 1], vertexArray[i + 2] };

			// p1 [x-1][z] (if on the edge use [x+1] instead of [x-1])
			if (x < 1)
				i = (x + 1 + (z)* tex->width) * 3;
			else
				i = (x - 1 + (z)* tex->width) * 3;

			p2 = (vec3) { vertexArray[i + 0], vertexArray[i + 1], vertexArray[i + 2] };

			// p1 [x][z-1]
			if (z < 1)
				i = (x + (z + 1) * tex->width) * 3;
			else
				i = (x + (z - 1) * tex->width) * 3;

			p3 = (vec3) { vertexArray[i + 0], vertexArray[i + 1], vertexArray[i + 2] };

			vec3 e1 = VectorSub(p2, p1);
			vec3 e2 = VectorSub(p3, p1);
			vec3 normal = CrossProduct(e1, e2);
			normal = Normalize(normal);

			i = (x + z * tex->width) * 3;
			normalArray[i + 0] = normal.x;
			normalArray[i + 1] = normal.y;
			normalArray[i + 2] = normal.z;
		}
	}
	for (x = 0; x < tex->width - 1; x++)
		for (z = 0; z < tex->height - 1; z++)
		{
			// Triangle 1
			indexArray[(x + z * (tex->width - 1)) * 6 + 0] = x + z * tex->width;
			indexArray[(x + z * (tex->width - 1)) * 6 + 1] = x + (z + 1) * tex->width;
			indexArray[(x + z * (tex->width - 1)) * 6 + 2] = x + 1 + z * tex->width;
			// Triangle 2
			indexArray[(x + z * (tex->width - 1)) * 6 + 3] = x + 1 + z * tex->width;
			indexArray[(x + z * (tex->width - 1)) * 6 + 4] = x + (z + 1) * tex->width;
			indexArray[(x + z * (tex->width - 1)) * 6 + 5] = x + 1 + (z + 1) * tex->width;
		}
	// End of terrain generation

	// Create Model and upload to GPU:

	Model* model = LoadDataToModel(
		vertexArray,
		normalArray,
		texCoordArray,
		NULL,
		indexArray,
		vertexCount,
		triangleCount * 3);

	return model;
}

float GetHeight(float x_world, float z_world)
{
	int x = (int)x_world;
	int z = (int)z_world;

	float h1 = vertexArray[(x + z * terrainTexture->width) * 3 + 1];	// + 1 gives Y
	float h2 = vertexArray[(x + 1 + z * terrainTexture->width) * 3 + 1];
	float h3 = vertexArray[(x + (z + 1) * terrainTexture->width) * 3 + 1];
	float h4 = vertexArray[(x + 1 + (z + 1) * terrainTexture->width) * 3 + 1];

	float dx = x_world - x;
	float dz = z_world - z;

	float height = 0.0f;

	// Test which triangle to use
	if (dx + dz < 1)
	{
		// Interpolate height
		height = h1 + (h2 - h1) * dx + (h3 - h1) * dz;
	}
	else
	{
		height = h4 + (h2 - h4) * (1.0f - dx) + (h3 - h4) * (1.0f - dz);
	}


	return height; // test
}