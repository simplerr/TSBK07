// Lab 1-1.
// This is the same as the first simple example in the course book,
// but with a few error checks.
// Remember to copy your file to a new on appropriate places during the lab so you keep old results.
// Note that the files "lab1-1.frag", "lab1-1.vert" are required.

// Should work as is on Linux and Mac. MS Windows needs GLEW or glee.
// See separate Visual Studio version of my demos.
#ifdef __APPLE__
	#include <OpenGL/gl3.h>
	#include "MicroGlut.h"
	// Linking hint for Lightweight IDE
	// uses framework Cocoa
#endif
#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "loadobj.h"

// Reference to shader program
GLuint program;
GLuint texture;

// vertex array object
unsigned int vertexArrayObjID;

vec3 cameraPos = { 30, 0, 0 };
vec3 targetPos = { 0, 0, 0 };
vec3 upVector = { 0, 1, 0 };
float cameraSpeed = 0.3f;
float pitch = 0.0f;
float yaw = -3.14/2.0;

#define near 1
#define far 300.0
#define right 0.5
#define left -0.5
#define top 0.5
#define bottom -0.5
GLfloat projectionMatrix[] = {	2.0f*near / (right - left), 0.0f, (right + left) / (right - left), 0.0f,
								0.0f, 2.0f*near / (top - bottom), (top + bottom) / (top - bottom), 0.0f,
								0.0f, 0.0f, -(far + near) / (far - near), -2 * far*near / (far - near),
								0.0f, 0.0f, -1.0f, 0.0f };

struct Windmill
{	
	Model *balcony, *roof, *walls, *blade;
	vec3 position;
};

struct Windmill windmill;

void render_windmill(struct Windmill* windmill, GLuint program, float time)
{
	mat4 rot, trans, scale, total;

	scale = S(1, 1, 1);
	rot = Rx(-time * 4);
	rot = IdentityMatrix();
	trans = T(windmill->position.x, windmill->position.y, windmill->position.z);
	total = Mult(scale, rot);
	total = Mult(trans, total);
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);	// Upload matrix

	// Draw the windmill
	DrawModel(windmill->balcony, program, "in_Position", "in_Normal", "inTexCoord");
	DrawModel(windmill->walls, program, "in_Position", "in_Normal", "inTexCoord");
	DrawModel(windmill->roof, program, "in_Position", "in_Normal", "inTexCoord");

	// Draw the blades
	trans = T(windmill->position.x+5, windmill->position.y+10, windmill->position.z);

	// 1
	rot = Rx(0 + time);
	total = Mult(trans, rot);
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);	// Upload matrix
	DrawModel(windmill->blade, program, "in_Position", "in_Normal", "inTexCoord");

	// 2
	rot = Rx(3.14f/2.0f + time);
	total = Mult(trans, rot);
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);	// Upload matrix
	DrawModel(windmill->blade, program, "in_Position", "in_Normal", "inTexCoord");

	// 3
	rot = Rx(3.14f + time);
	total = Mult(trans, rot);
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);	// Upload matrix
	DrawModel(windmill->blade, program, "in_Position", "in_Normal", "inTexCoord");

	// 3
	rot = Rx(-3.14f/2.0f + time);
	total = Mult(trans, rot);
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);	// Upload matrix
	DrawModel(windmill->blade, program, "in_Position", "in_Normal", "inTexCoord");

}

void load_windmill(vec3 position, struct Windmill* windmill)
{
	windmill->position = position;

	windmill->blade = LoadModelPlus("windmill/blade.obj");
	windmill->balcony = LoadModelPlus("windmill/windmill-balcony.obj");
	windmill->roof = LoadModelPlus("windmill/windmill-roof.obj");
	windmill->walls = LoadModelPlus("windmill/windmill-walls.obj");
}

void mouseMove(int x, int y)
{
	static int last_x = 1024 / 2;
	static int last_y = 768 / 2;

	int delta_x = last_x - x;
	int delta_y = last_y - y;

	float camera_sensitivity = 0.01f;
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

void init(void)
{
	// vertex buffer object, used for uploading the geometry
	unsigned int vertexBufferObjID;
	unsigned int bunnyIndexBufferObjID;
	unsigned int bunnyNormalBufferObjID;
	unsigned int bunnyTexCoordBufferObjID;
	GLuint colorBufferObjID;

	glutPassiveMotionFunc(&mouseMove);
//	initKeymapManager();
	glutWarpPointer(1024 / 2, 768 / 2);

	// Load windmill
	vec3 position = { 0, 0, 0 };
	load_windmill(position, &windmill);

	dumpInfo();

	// GL inits
	glClearColor(1,0.2,0.5,0);
	//glFrontFace(GL_CW);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	
	printError("GL inits");

	// Load and compile shader
	program = loadShaders("lab3-2.vert", "lab3-2.frag");
	printError("init shader");

	// Load texture
	glUniform1i(glGetUniformLocation(program, "texUnit"), 0); // Texture unit 0
	LoadTGATextureSimple("conc.tga", &texture);

	// Upload projection matrix
	glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix);

	// End of upload of geometry
	printError("init arrays");
}


void display(void)
{
	printError("pre display");
	static float time = 0;

	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Upload camera matrix
	mat4 cameraMatrix = lookAt(cameraPos.x, cameraPos.y, cameraPos.z, targetPos.x, targetPos.y, targetPos.z, upVector.x, upVector.y, upVector.z);
	glUniformMatrix4fv(glGetUniformLocation(program, "cameraMatrix"), 1, GL_TRUE, cameraMatrix.m);

	render_windmill(&windmill, program, time);
	
	glutSwapBuffers();

	time += 0.01;
}

void OnTimer(int value)
{
	// Move the camera using user input
	if (glutKeyIsDown('a'))
	{
		vec3 dir = VectorSub(targetPos, cameraPos);
		vec3 right_vec = CrossProduct(dir, upVector);
		right_vec = Normalize(right_vec);

		cameraPos = VectorSub(cameraPos, right_vec);
		targetPos = VectorSub(targetPos, right_vec);
	}
	else if (glutKeyIsDown('d'))
	{
		vec3 dir = VectorSub(targetPos, cameraPos);
		vec3 right_vec = CrossProduct(dir, upVector);
		right_vec = Normalize(right_vec);

		cameraPos = VectorAdd(cameraPos, right_vec);
		targetPos = VectorAdd(targetPos, right_vec);
	}

	if (glutKeyIsDown('w'))
	{
		vec3 dir = VectorSub(targetPos, cameraPos);
		dir = Normalize(dir);
		cameraPos = VectorAdd(cameraPos, dir);
		targetPos = VectorAdd(targetPos, dir);
	}
	if (glutKeyIsDown('s'))
	{
		vec3 dir = VectorSub(targetPos, cameraPos);
		dir = Normalize(dir);
		cameraPos = VectorSub(cameraPos, dir);
		targetPos = VectorSub(targetPos, dir);
	}

	glutPostRedisplay();
	glutTimerFunc(20, &OnTimer, value);
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitContextVersion(3, 2);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(1024, 768);
	glutCreateWindow ("GL3 white triangle example");
	glutDisplayFunc(display); 

	// Timer
	glutTimerFunc(100, &OnTimer, 0);

	init ();
	glutMainLoop();
}
