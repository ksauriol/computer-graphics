#include <GL/glew.h>
#include <GL/freeglut.h>

//glm library
#include <glm/glm.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Model.h"
#include "Shader.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <string>

Shader shader; // loads our vertex and fragment shaders

Model *plane; //a plane

Model *body;
Model *head;
Model *leftHand;
Model *rightHand;
Model *leftLeg;
Model *rightLeg;
Model *leftHorn;
Model *rightHorn;
Model *leftEye;
Model *rightEye;

Model *tree;
Model *bench;

glm::mat4 projection; // projection matrix
glm::mat4 view; // where the camera is looking
glm::mat4 viewFree; // where the camera is looking

//////////// CAMERA + PLAYER MOVEMENT
glm::vec3 pMove(0.0f, 0.0f, 0.3f);
glm::vec3 cMove(0.3f, 0.0f, 0.0f);

glm::mat4 bodyMat;
float angle;
float limbAngle;
bool limbDir;
float jump;
bool timetojump;
float maxheight;
float height = 0.01f;
bool isjumping;
bool up;

glm::vec3 camPos;
glm::vec3 camCenter;
bool positioned = false;
int state;
float movement = 0.3f;
/* report GL errors, if any, to stderr */
void checkError(const char *functionName)
{
	GLenum error;
	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cerr << "GL error " << error << " detected in " << functionName << std::endl;
	}
}

void initShader(void)
{
	shader.InitializeFromFile("shaders/phong.vert", "shaders/phong.frag");
	shader.AddAttribute("vertexPosition");
	shader.AddAttribute("vertexNormal");

	checkError("initShader");
}

void initRendering(void)
{
	glClearColor(0.117f, 0.565f, 1.0f, 0.0f); // Dodger Blue
	checkError("initRendering");
}

void init(void)
{
	// Perspective projection matrix.
	projection = glm::perspective(45.0f, 800.0f / 600.0f, 1.0f, 1000.0f);


	// Load identity matrix into model matrix (no initial translation or rotation)


	initShader();
	initRendering();
}

/* This prints in the console when you start the program*/
void dumpInfo(void)
{
	printf("Vendor: %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version: %s\n", glGetString(GL_VERSION));
	printf("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	checkError("dumpInfo");
}

/*This gets called when the OpenGL is asked to display. This is where all the main rendering calls go*/
void display(void)
{

	//glm::rot
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// camera positioned at 20 on the z axis, looking into the screen down the -Z axis.
	/*
	if (firstPerson) {
		//view = glm::translate(bodyMat,0.0f, 1.8f, 0.0f);
		glm::vec4 eyePos = bodyMat*glm::vec4(0, 0, 0, 1);
		glm::vec4 center = bodyMat*glm::vec4(0, 0, -20, 1);

		view = glm::lookAt(glm::vec3(eyePos.x, eyePos.y, eyePos.z), glm::vec3(center.x, center.y, center.z), glm::vec3(0.0f, 1.0f, 0.0f));
		body->render(glm::scale(1.0f, 1.3f, 1.0f), projection); // Render current active model.

		rightHand->render(glm::translate(0.0f, 1.1f, 0.0f)*glm::rotate(limbAngle, 1.0f, 0.0f, 0.0f)*glm::translate(-1.2f, -1.8f, 0.2f)*glm::scale(0.25f, 1.86f, 0.25f), projection);
		leftHand->render(glm::translate(0.0f, 1.1f, 0.0f)*glm::rotate(-limbAngle, 1.0f, 0.0f, 0.0f)*glm::translate(1.2f, -1.8f, 0.2f)*glm::scale(0.25f, 1.86f, 0.25f), projection);
		rightLeg->render(glm::translate(0.0f, 1.1f, 0.0f)*glm::rotate(-limbAngle*0.5f, 1.0f, 0.0f, 0.0f)*glm::translate(-.35f, -4.3f, 0.0f)*glm::scale(0.25f, 1.86f, 0.25f), projection);
		leftLeg->render(glm::translate(0.0f, 1.1f, 0.0f)*glm::rotate(limbAngle*0.5f, 1.0f, 0.0f, 0.0f)*glm::translate(0.35f, -4.3f, 0.0f)*glm::scale(0.25f, 1.86f, 0.25f), projection);
		plane->render(view * glm::translate(0.0f, -5.0f, 0.0f)*glm::scale(20.0f, 1.0f, 20.0f), projection);
		tree->render(view * glm::translate(-10.0f, -5.0f, -15.0f)*glm::scale(0.2f, 0.1f, 0.2f), projection);
		tree->render(view * glm::translate(-10.0f, -5.0f, 15.0f)*glm::scale(0.2f, 0.1f, 0.2f), projection);
		bench->render(view* glm::translate(10.0f, -5.0f, 0.0f)*glm::scale(0.2f, 0.2f, 0.2f), projection);
		bench->render(view* glm::translate(-10.0f, -5.0f, 0.0f)*glm::scale(0.2f, 0.2f, 0.2f)*glm::rotate(180.0f, 0.0f, 1.0f, 0.0f), projection);
	}
	else {
	*/
		viewFree = glm::lookAt(camPos, camCenter, glm::vec3(0.0f, 1.0f, 0.0f));
		//free cam
		
		if (height < 1 && up){
			state = 1;
		}
		if (height >= 1){
			state = 2;
			up = false;
		}
		if (height <= 0){
			state = 3;
			up = false;
		}
		printf("State: %d\n", state);

			switch (state){
			case 1:
				jump = .1f;
				height += .1f;
				break;
			case 2:
				jump = -.1f;
				height -= .1f;
				break;
			
			case 3:
				jump = 0.0f;
				height = 0.01f;
				//isjumping = false;
				break;
			}//end switch
		
		bodyMat = glm::translate(bodyMat, 0.0f, jump, 0.0f);
		
		
	/*		if (jump){

			jump = false;
			timetojump = false;
			if (maxheight >= 3 && maxheight > 0){
				bodyMat = glm::translate(bodyMat, 0.0f, -0.03f, 0.0f);
				maxheight -= .03f;
			}
			else if (maxheight <= 0){
				jump = true;
				timetojump = true;
			}
			bodyMat = glm::translate(bodyMat, 0.0f, 0.03f, 0.0f);
			maxheight += .03f;
		}//end if jump */
		

		body->render(viewFree*bodyMat*glm::scale(1.0f, 1.6f, 1.0f), projection); // Render current active model.
		head->render(viewFree*bodyMat*glm::translate(0.0f, 1.4f, 0.0f)*glm::scale(1.0f, 1.0f, 1.0f), projection);

		rightHand->render(viewFree*bodyMat*glm::translate(0.0f, 1.1f, 0.0f)*glm::rotate(limbAngle, 1.0f, 0.0f, 0.0f)*glm::translate(-1.2f, -1.8f, 0.2f)*glm::scale(0.25f, 1.86f, 0.25f), projection);
		leftHand->render(viewFree*bodyMat*glm::translate(0.0f, 1.1f, 0.0f)*glm::rotate(-limbAngle, 1.0f, 0.0f, 0.0f)*glm::translate(1.2f, -1.8f, 0.2f)*glm::scale(0.25f, 1.86f, 0.25f), projection);
		rightLeg->render(viewFree*bodyMat*glm::translate(0.0f, 1.1f, 0.0f)*glm::rotate(-limbAngle*0.5f, 1.0f, 0.0f, 0.0f)*glm::translate(-.35f, -4.2f, 0.0f)*glm::scale(0.25f, 1.85f, 0.25f), projection);
		leftLeg->render(viewFree*bodyMat*glm::translate(0.0f, 1.1f, 0.0f)*glm::rotate(limbAngle*0.5f, 1.0f, 0.0f, 0.0f)*glm::translate(0.35f, -4.2f, 0.0f)*glm::scale(0.25f, 1.85f, 0.25f), projection);

		rightHorn->render(viewFree*bodyMat*glm::translate(0.5f, 2.5f, 0.0f)*glm::rotate(40.0f, 0.0f, 0.0f, -1.0f)*glm::scale(0.15f, 0.5f, 0.15f), projection);
		leftHorn->render(viewFree*bodyMat*glm::translate(-0.5f, 2.5f, 0.0f)*glm::rotate(40.0f, 0.0f, 0.0f, 1.0f)*glm::scale(0.15f, 0.5f, 0.15f), projection);

		rightEye->render(viewFree*bodyMat*glm::translate(0.4f, 1.5f, -0.6f)*glm::rotate(90.0f, 1.0f, 0.0f, 0.0f)*glm::scale(0.15f, 0.4f, 0.12f), projection);
		leftEye->render(viewFree*bodyMat*glm::translate(-0.4f, 1.5f, -0.6f)*glm::rotate(90.0f, 1.0f, 0.0f, 0.0f)*glm::scale(0.15f, 0.4f, 0.12f), projection);

		plane->render(viewFree * glm::translate(0.0f, -5.0f, 0.0f)*glm::scale(20.0f, 1.0f, 20.0f), projection);
		tree->render(viewFree * glm::translate(-10.0f, -5.0f, -15.0f)*glm::scale(0.2f, 0.1f, 0.2f), projection);
		tree->render(viewFree * glm::translate(-10.0f, -5.0f, 15.0f)*glm::scale(0.2f, 0.1f, 0.2f), projection);
		bench->render(viewFree * glm::translate(10.0f, -5.0f, 0.0f)*glm::scale(0.2f, 0.2f, 0.2f), projection);
		bench->render(viewFree* glm::translate(-10.0f, -5.0f, 0.0f)*glm::scale(0.2f, 0.2f, 0.2f)*glm::rotate(180.0f, 0.0f, 1.0f, 0.0f), projection);

	//}
		if (!positioned) {
			bodyMat = glm::rotate(bodyMat, -90.0f, 0.0f, 1.0f, 0.0f);
			positioned = !positioned;
		}

	glutSwapBuffers(); // Swap the buffers.
	checkError("display");
}

/*This gets called when nothing is happening (OFTEN)*/
void idle()
{
	glutPostRedisplay(); // create a display event. Display calls as fast as CPU will allow when put in the idle function
}

/*Called when the window is resized*/
void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	checkError("reshape");
}

/*Called when a normal key is pressed*/
void keyboard(unsigned char key, int x, int y)
{
	glm::vec3 lookatdir = glm::normalize(camCenter - camPos);
	timetojump = true;
	switch (key) {
	case 27: // this is an ascii value
		exit(0);
		break;
	case 'a':
		bodyMat = glm::translate(bodyMat, 0.0f, 0.0f, 0.3f);
		camPos -= cMove;
		camCenter -= cMove;
		break;
	case 'd':
		bodyMat = glm::translate(bodyMat, 0.0f, 0.0f, -0.3f);
		camPos += cMove;
		camCenter += cMove;
		break;
	case 'g':
		bodyMat = glm::rotate(bodyMat, -5.0f, 0.0f, 1.0f, 0.0f);

		//if (firstPerson)
		//view = glm::rotate(view, -5.0f, 0.0f, 1.0f, 0.0f);
		break;
	case 'r':
		bodyMat = glm::rotate(bodyMat, 5.0f, 0.0f, 1.0f, 0.0f);

		//if (firstPerson)
		//view = glm::rotate(view, 5.0f, 0.0f, 1.0f, 0.0f);
		break;
	case 'w':
		if (limbAngle >= 20.0f) limbDir = false;
		if (limbAngle <= -20.0f) limbDir = true;
		limbAngle = (limbDir) ? limbAngle + 3.0f : limbAngle - 3.0f;
		bodyMat = glm::translate(bodyMat, 0.0f, 0.0f, -0.3f);
		//if (firstPerson)
		//view = glm::translate(view, 0.0f, 0.0f, -0.3f);
		break;
	case 's':
		if (limbAngle >= 20.0f) limbDir = false;
		if (limbAngle <= -20.0f) limbDir = true;
		limbAngle = (limbDir) ? limbAngle + 3.0f : limbAngle - 3.0f;
		bodyMat = glm::translate(bodyMat, 0.0f, 0.0f, 0.3f);
		//if (firstPerson)
		//view = glm::translate(view, 0.0f, 0.0f, 0.3f);
		break;
	case 'f':
		camPos += lookatdir;
		camCenter += lookatdir;
		break;
	case 'v':
		camPos -= lookatdir;
		camCenter -= lookatdir;
		break;
	case 'q':
		limbAngle = (limbAngle != 20.0f) ? 20.0f : -20.0f;
		break;
/*	case 'c':
		firstPerson = firstPerson ? false : true;
		break;
		*/
	case 32 :
		//isjumping = true;
		up = true;
			//bodyMat = glm::translate(bodyMat, 0.0f, 0.3f, 0.0f);
			break;
		
		/*
		case 'y':
		view = glm::lookAt(glm::vec3(30.0f, 00.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		break;
		case 'z':
		view = glm::lookAt(glm::vec3(0.0f, 30.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f));
		break;*/
	}
}

void keyboardArrow(int key, int x, int y) {
	glm::vec3 left = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), camCenter - camPos));
	switch (key) {
	case GLUT_KEY_UP:
		if (camCenter.y < 30.0f) {
			camCenter.y += 0.8f;
		}
		break;
	case GLUT_KEY_DOWN:
		if (camCenter.y > -30.0f) {
			camCenter.y -= 0.8f;
		}
		break;
	case GLUT_KEY_RIGHT:
		camCenter -= left*0.8f;
		break;
	case GLUT_KEY_LEFT:
		camCenter += left*0.8f;
		break;
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);
	glewInit();
	dumpInfo();
	init();
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(keyboardArrow);
	glEnable(GL_DEPTH_TEST);

	camPos = glm::vec3(0.0f, 0.0f, 40.0f);
	camCenter = glm::vec3(0.0f, 0.0f, 0.0f);


	body = new Model(&shader, "models/cylinder.obj");
	head = new Model(&shader, "models/sphere.obj");
	rightHand = new Model(&shader, "models/cylinder.obj");
	leftHand = new Model(&shader, "models/cylinder.obj");
	rightLeg = new Model(&shader, "models/cylinder.obj");
	leftLeg = new Model(&shader, "models/cylinder.obj");
	rightHorn = new Model(&shader, "models/cylinder.obj");
	leftHorn = new Model(&shader, "models/cylinder.obj");
	rightEye = new Model(&shader, "models/cylinder.obj");
	leftEye = new Model(&shader, "models/cylinder.obj");

	plane = new Model(&shader, "models/plane.obj");
	tree = new Model(&shader, "models/palm_tree.obj");//working
	bench = new Model(&shader, "models/klupa117.obj", "models/");
	glutMainLoop();

	return 0;
}