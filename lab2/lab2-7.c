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

Model *model1, *model2;
GLfloat pos1[] = { 0, 0, 0 };
GLfloat pos2[] = { 5, 7, 0 };
vec3 cameraPos = { 10, 10, 10 };

#define near 1.0
#define far 300.0
#define right 0.5
#define left -0.5
#define top 0.5
#define bottom -0.5
GLfloat projectionMatrix[] = {	2.0f*near / (right - left), 0.0f, (right + left) / (right - left), 0.0f,
								0.0f, 2.0f*near / (top - bottom), (top + bottom) / (top - bottom), 0.0f,
								0.0f, 0.0f, -(far + near) / (far - near), -2 * far*near / (far - near),
								0.0f, 0.0f, -1.0f, 0.0f };

void init(void)
{
	// vertex buffer object, used for uploading the geometry
	unsigned int vertexBufferObjID;
	unsigned int bunnyIndexBufferObjID;
	unsigned int bunnyNormalBufferObjID;
	unsigned int bunnyTexCoordBufferObjID;
	GLuint colorBufferObjID;

	model1 = LoadModelPlus("bunnyplus.obj");
	model2 = LoadModelPlus("cubeplus.obj");

	dumpInfo();

	// GL inits
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glClearColor(1,0.2,0.5,0);
	//glFrontFace(GL_CW);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	
	printError("GL inits");

	// Load and compile shader
	program = loadShaders("lab2-7.vert", "lab2-7.frag");
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
	static float e = 0;

	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Move the camera
	cameraPos = MultVec3(Ry(0.01), cameraPos);
	//cameraPos.x = 10*sinf(e);
	//cameraPos.z = 10*cos(e);

	// Upload camera matrix
	mat4 cameraMatrix = lookAt(cameraPos.x, cameraPos.y, cameraPos.z, 0, 0, 0, 0, 1, 0);
	glUniformMatrix4fv(glGetUniformLocation(program, "cameraMatrix"), 1, GL_TRUE, cameraMatrix.m);

	mat4 rot, trans, scale, total;

	scale = S(6, 6, 6);
	rot = Rx(-e*4);
	trans = T(pos1[0], pos1[1], pos1[2]);
	total = Mult(scale, rot);
	total = Mult(trans, total);
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);	// // Upload matrix


	DrawModel(model1, program, "in_Position", "in_Normal", "inTexCoord");

	scale = S(2, 2, 2);
	trans = T(pos2[0], pos2[1], pos2[2]);
	total = Mult(scale, rot);
	total = Mult(trans, total);
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);	// // Upload matrix

	DrawModel(model2, program, "in_Position", "in_Normal", "inTexCoord");
	
	printError("display");
	
	glutSwapBuffers();

	e += 0.01;
}

void OnTimer(int value)
{
	

	
	
	
	glutPostRedisplay();
	glutTimerFunc(20, &OnTimer, value);
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitContextVersion(3, 2);
	glutCreateWindow ("GL3 white triangle example");
	glutDisplayFunc(display); 

	// Timer
	glutTimerFunc(20, &OnTimer, 0);

	init ();
	glutMainLoop();
}
