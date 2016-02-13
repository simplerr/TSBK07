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

// Globals
// Data would normally be read from files
GLfloat vertices[] = {
		-0.5f,-0.5f,-0.5f, // triangle 1 : begin
		-0.5f,-0.5f, 0.5f,
		-0.5f, 0.5f, 0.5f, // triangle 1 : end
		 0.5f, 0.5f,-0.5f, // triangle 2 : begin
		-0.5f,-0.5f,-0.5f,
		-0.5f, 0.5f,-0.5f, // triangle 2 : end
	     0.5f,-0.5f, 0.5f,
		-0.5f,-0.5f,-0.5f,
	     0.5f,-0.5f,-0.5f,
	     0.5f, 0.5f,-0.5f,
	     0.5f,-0.5f,-0.5f,
		- 0.5f,-0.5f,-0.5f,
		- 0.5f,-0.5f,-0.5f,
		- 0.5f, 0.5f, 0.5f,
		- 0.5f, 0.5f,-0.5f,
	     0.5f,-0.5f, 0.5f,
		- 0.5f,-0.5f, 0.5f,
		- 0.5f,-0.5f,-0.5f,
		- 0.5f, 0.5f, 0.5f,
		- 0.5f,-0.5f, 0.5f,
	     0.5f,-0.5f, 0.5f,
	     0.5f, 0.5f, 0.5f,
	     0.5f,-0.5f,-0.5f,
	     0.5f, 0.5f,-0.5f,
	     0.5f,-0.5f,-0.5f,
	     0.5f, 0.5f, 0.5f,
	     0.5f,-0.5f, 0.5f,
	     0.5f, 0.5f, 0.5f,
	     0.5f, 0.5f,-0.5f,
		- 0.5f, 0.5f,-0.5f,
	     0.5f, 0.5f, 0.5f,
		- 0.5f, 0.5f,-0.5f,
		- 0.5f, 0.5f, 0.5f,
	     0.5f, 0.5f, 0.5f,
		- 0.5f, 0.5f, 0.5f,
	     0.5f,-0.5f, 0.5f
};

GLfloat colors[36 * 3] = {
		 0.583f,  0.771f,  0.014f,
	     0.609f,  0.115f,  0.436f,
	     0.327f,  0.483f,  0.844f,
	     0.822f,  0.569f,  0.201f,
	     0.435f,  0.602f,  0.223f,
	     0.310f,  0.747f,  0.185f,
	     0.597f,  0.770f,  0.761f,
	     0.559f,  0.436f,  0.730f,
	     0.359f,  0.583f,  0.152f,
	     0.483f,  0.596f,  0.789f,
	     0.559f,  0.861f,  0.639f,
	     0.195f,  0.548f,  0.859f,
	     0.014f,  0.184f,  0.576f,
	     0.771f,  0.328f,  0.970f,
	     0.406f,  0.615f,  0.116f,
	     0.676f,  0.977f,  0.133f,
	     0.971f,  0.572f,  0.833f,
	     0.140f,  0.616f,  0.489f,
	     0.997f,  0.513f,  0.064f,
	     0.945f,  0.719f,  0.592f,
	     0.543f,  0.021f,  0.978f,
	     0.279f,  0.317f,  0.505f,
	     0.167f,  0.620f,  0.077f,
	     0.347f,  0.857f,  0.137f,
	     0.055f,  0.953f,  0.042f,
	     0.714f,  0.505f,  0.345f,
	     0.783f,  0.290f,  0.734f,
	     0.722f,  0.645f,  0.174f,
	     0.302f,  0.455f,  0.848f,
	     0.225f,  0.587f,  0.040f,
	     0.517f,  0.713f,  0.338f,
	     0.053f,  0.959f,  0.120f,
	     0.393f,  0.621f,  0.362f,
	     0.673f,  0.211f,  0.457f,
	     0.820f,  0.883f,  0.371f,
	     0.982f,  0.099f,  0.879f
};

GLfloat color[] = { 1.0f, 1, 0, 1.0f };

float speedx = 0.02f, speedy = 0.02f;

// vertex array object
unsigned int vertexArrayObjID;
GLuint colorArrayObjID;

Model *model;

void init(void)
{
	// vertex buffer object, used for uploading the geometry
	unsigned int vertexBufferObjID;
	unsigned int bunnyIndexBufferObjID;
	unsigned int bunnyNormalBufferObjID;
	GLuint colorBufferObjID;

	model = LoadModel("bunny.obj");

	dumpInfo();

	// GL inits
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glClearColor(1,0.2,0.5,0);
	//glFrontFace(GL_CW);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	
	printError("GL inits");

	// Load and compile shader
	program = loadShaders("lab1-6.vert", "lab1-6.frag");
	printError("init shader");
	
	// **Own code**
	glUniform4fv(glGetUniformLocation(program, "color_"), 1, color);
	

	// Upload geometry to the GPU:	
	// Allocate and activate Vertex Array Object
	glGenVertexArrays(1, &vertexArrayObjID);
	glBindVertexArray(vertexArrayObjID);

	// Allocate Vertex Buffer Objects
	glGenBuffers(1, &vertexBufferObjID);	// position
	glGenBuffers(1, &colorBufferObjID);		// color
	glGenBuffers(1, &bunnyIndexBufferObjID);		// color
	glGenBuffers(1, &bunnyNormalBufferObjID);		// color
	
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

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bunnyIndexBufferObjID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, model->numIndices*sizeof(GLuint), model->indexArray, GL_STATIC_DRAW);

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
	glutCreateWindow ("GL3 white triangle example");
	glutDisplayFunc(display); 

	// Timer
	glutTimerFunc(20, &OnTimer, 0);

	if (GLEW_OK != glewInit())
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		printf("glewInit failed, aborting.\n");
		exit(1);
	}
	printf("Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	init ();
	glutMainLoop();
}
