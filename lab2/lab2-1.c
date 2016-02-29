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

// vertex array object
unsigned int vertexArrayObjID;

Model *model;

void init(void)
{
	// vertex buffer object, used for uploading the geometry
	unsigned int vertexBufferObjID;
	unsigned int bunnyIndexBufferObjID;
	unsigned int bunnyNormalBufferObjID;
	unsigned int bunnyTexCoordBufferObjID;
	GLuint colorBufferObjID;

	model = LoadModel("bunnyplus.obj");

	dumpInfo();

	// GL inits
	glClearColor(1,0.2,0.5,0);
	//glFrontFace(GL_CW);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	
	printError("GL inits");

	// Load and compile shader
	program = loadShaders("lab2-1.vert", "lab2-1.frag");
	printError("init shader");
	

	// Upload geometry to the GPU:	
	// Allocate and activate Vertex Array Object
	glGenVertexArrays(1, &vertexArrayObjID);
	glBindVertexArray(vertexArrayObjID);

	// Allocate Vertex Buffer Objects
	glGenBuffers(1, &vertexBufferObjID);			// position
	glGenBuffers(1, &colorBufferObjID);				// color
	glGenBuffers(1, &bunnyIndexBufferObjID);		// index
	glGenBuffers(1, &bunnyNormalBufferObjID);		// normal
	glGenBuffers(1, &bunnyTexCoordBufferObjID);		// tex coords
	
	// VBO for vertex data
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjID);
	glBufferData(GL_ARRAY_BUFFER, model->numVertices * 3 * sizeof(GLfloat), model->vertexArray, GL_STATIC_DRAW);
	glVertexAttribPointer(glGetAttribLocation(program, "in_Position"), 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(glGetAttribLocation(program, "in_Position"));

	// VBO for normal data
	glBindBuffer(GL_ARRAY_BUFFER, bunnyNormalBufferObjID);
	glBufferData(GL_ARRAY_BUFFER, model->numVertices * 3 * sizeof(GLfloat), model->normalArray, GL_STATIC_DRAW);
	glVertexAttribPointer(glGetAttribLocation(program, "in_Normal"), 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(glGetAttribLocation(program, "in_Normal"));

	// Index data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bunnyIndexBufferObjID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, model->numIndices*sizeof(GLuint), model->indexArray, GL_STATIC_DRAW);

	// Tex coord data
	if (model->texCoordArray != NULL)
	{
		glBindBuffer(GL_ARRAY_BUFFER, bunnyTexCoordBufferObjID);
		glBufferData(GL_ARRAY_BUFFER, model->numVertices * 2 * sizeof(GLfloat), model->texCoordArray, GL_STATIC_DRAW);
		glVertexAttribPointer(glGetAttribLocation(program, "inTexCoord"), 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(glGetAttribLocation(program, "inTexCoord"));
	}

	// End of upload of geometry
	printError("init arrays");
}


void display(void)
{
	printError("pre display");

	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(vertexArrayObjID);	// Select VAO
	glDrawElements(GL_TRIANGLES, model->numIndices, GL_UNSIGNED_INT, 0L);
	
	printError("display");
	
	glutSwapBuffers();
}

void OnTimer(int value)
{
	// Rotate around all axes
	static float e = 0;
	mat4 rot = Mult(Mult(Rz(e * 5), Rx(e *2)), Ry(e * 2));
	glUniformMatrix4fv(glGetUniformLocation(program, "myMatrix"), 1, GL_TRUE, rot.m);	// // Upload matrix
	e += 0.01;
	
	glutPostRedisplay();
	glutTimerFunc(20, &OnTimer, value);
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitContextVersion(3, 2);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow ("GL3 white triangle example");
	glutDisplayFunc(display); 

	// Timer
	glutTimerFunc(20, &OnTimer, 0);

	init ();
	glutMainLoop();
}
