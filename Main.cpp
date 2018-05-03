#include <stdio.h>
#include <stdlib.h>
#include<iostream>
#include <string>
#if _WIN32
	#include <Windows.h>
#endif
#if __APPLE__
	#include <OpenGL/gl.h>
	#include <OpenGL/glu.h>
	#include <GLUT/glut.h>
#else
	#include <GL/gl.h>
	#include <GL/glu.h>
	#include <GL/glut.h>
#endif
#define MAX_WIDTH 10
#define MAX_HEIGHT 10
#define PI 3.14159265
struct Wave {
	float displacement;
}Wave;
struct CameraControl {
	int xRotation, yRotation;

	float camX, camY, camZ;

	int lastMouseX, lastMouseY;

	bool leftMouseDown, rightMouseDown;


}CameraControl;

struct GraphicsOptions {
	bool lightingOn;
	bool wireFrame;
	bool normals;
	bool motion;
}Options;

typedef struct Color {
	unsigned char R, G, B, A;
}Color;

typedef struct Vector3f {
	float x, y, z;
}Vector3f;

void initGlobals()
{
	CameraControl.xRotation = 0;
	CameraControl.yRotation = 0;

	CameraControl.camX = 0;
	CameraControl.camY = 5;
	CameraControl.camZ = 0;

	CameraControl.lastMouseX = 0;
	CameraControl.lastMouseY = 0;

	CameraControl.leftMouseDown = false;
	CameraControl.rightMouseDown = false;

	Options.lightingOn = true;
	Options.wireFrame = false;
	Options.normals = true;
	Options.motion = true;

	Wave.displacement = 0;
}

void setLighting(bool l)
{
	if (l && Options.lightingOn)
	{
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
	}
	if (!l || !Options.lightingOn)
	{
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
	}
}



void drawAxes(float length)
{
	//X
	glColor3f(255, 0, 0);
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(length, 0, 0);
	glEnd();
	//Y
	glBegin(GL_LINES);
	glColor3f(0, 255, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, length, 0);
	glEnd();
	//Z
	glBegin(GL_LINES);
	glColor3f(0, 0, 255);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, length);
	glEnd();
}

Vector3f addVectors(Vector3f v1, Vector3f v2)
{
	Vector3f sum = { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
	return sum;
}

Vector3f crossProductVectors(Vector3f v1, Vector3f v2)
{
	float x, y, z;
	x = v1.y * v2.z - v2.y * v1.z;
	y = -(v1.z *v2.x - v2.z * v1.x);
	z = v1.x * v2.y - v2.x * v1.y;
	return Vector3f{ x, y, z };
}

Vector3f leSinWave(float x, float z, bool isNorm)
{
	float A = 1;
	float B = PI / 5;
	float C = -1;
	float t = Wave.displacement;

	if (isNorm)
	{
		Vector3f xTan = {1, -A*B*cosf(B*x-t), 0};
		Vector3f zTan = {0, -A * B*cosf(B*z), 1};
		return crossProductVectors(xTan, zTan);
	}
	else
	{
		float y = A * sinf(B * x - t) + A * sinf(B * z) + C;
		return Vector3f{x, y, z};
	}
}





void drawVector(Vector3f pos, Vector3f target, Color col, bool normalize, float normLen)
{
	float normFactor, vecLen;
	Vector3f newPos;
	setLighting(false);
	glColor3ub(col.R, col.G, col.B);

	if (normalize)
	{
		vecLen = sqrtf(target.x * target.x + target.y * target.y + target.z * target.z);
		normFactor = vecLen / normLen;

		target.x /= normFactor;
		target.y /= normFactor;
		target.z /= normFactor;
	}
	newPos = addVectors(pos, target);

	glBegin(GL_LINES);
		glVertex3fv(&pos.x);
		glVertex3fv(&newPos.x);
	glEnd();

	glColor3f(1, 1, 1);
	setLighting(true);
}

void drawLeSquareAsTriangles(Vector3f p1, Vector3f p2, Vector3f p3, Vector3f p4, 
	Vector3f n1, Vector3f n2, Vector3f n3, Vector3f n4)
{
	glBegin(GL_TRIANGLES);
	//Triangle 1
	glNormal3fv(&n1.x);
	glVertex3fv(&p1.x);
	glNormal3fv(&n2.x);
	glVertex3fv(&p2.x);
	glNormal3fv(&n3.x);
	glVertex3fv(&p3.x);

	//Triangle 2
	glNormal3fv(&n3.x);
	glVertex3fv(&p3.x);
	glNormal3fv(&n2.x);
	glVertex3fv(&p2.x);
	glNormal3fv(&n4.x);
	glVertex3fv(&p4.x);

	glEnd();
}

void drawWave()
{
	if (Options.lightingOn)
	{
		float r, g, b;
		r = 0;
		g = 187 / 255;
		b = 1;

		float lightPosition[] = { 1, 1, 1, 1 };
		float materialAmbient[] = { r, g, b, 1, 1 };
		float materialDiffuse[] = { r, g, b, 1, 1 };
		float materialSpecular[] = { 1, 1, 1, 1 };
		float materialShiny = 10;

		glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, materialAmbient);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDiffuse);
		//glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialSpecular);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &materialShiny);

	}
	glColor3f(1, 1, 1);
	for (float i = -10, step = 0.5; i < 10; i += step)
	{
		for (float j = -10; j < 10; j += step)
		{
			Vector3f p1, p2, p3, p4;
			Vector3f n1, n2, n3, n4;

			p1 = leSinWave(i, j + step, false);
			p2 = leSinWave(i + step, j + step, false);
			p3 = leSinWave(i, j, false);
			p4 = leSinWave(i + step, j, false);

			n1 = leSinWave(i, j + step, true);
			n2 = leSinWave(i + step, j + step, true);
			n3 = leSinWave(i, j, true);
			n4 = leSinWave(i + step, j, true);

			drawLeSquareAsTriangles(p1, p2, p3, p4, n1, n2, n3, n4);



			if (Options.normals)
			{
				Color normalColor = {255, 255, 0, 255};
				drawVector(p1, n1, normalColor, true, 0.5);
				drawVector(p2, n2, normalColor, true, 0.5);
				drawVector(p3, n3, normalColor, true, 0.5);
				drawVector(p4, n4, normalColor, true, 0.5);
			}

		}
	}
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glLoadIdentity();
	glPushMatrix();

	//Makes Camera do things ~ Dom
	glRotatef(-CameraControl.yRotation, 0, 1, 0);
	glRotatef(-CameraControl.xRotation, 1, 0, 0);
	glTranslatef(-CameraControl.camX, -CameraControl.camY, -CameraControl.camZ);


	/* Put drawing code here */
	drawAxes(1);
	drawWave();
	//Stuff
	glutSolidTeapot(1);
	if(glGetError())
	{
		std::cout << "Sod off\n" ;
		std::cout << gluErrorString(glGetError()) << std::endl;
	}

	glPopMatrix();

	glutSwapBuffers();
}

/* You can ignore this for now, it just lets you exit when you press 'q' or ESC */
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
		case 27:
		case 'q':
			exit(EXIT_SUCCESS);
			break;
		default:
			break;
	}
}

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		CameraControl.leftMouseDown = true;
	}
	else
	{
		CameraControl.leftMouseDown = false;
	}

	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		CameraControl.rightMouseDown = true;
	}
	else
	{
		CameraControl.rightMouseDown = false;
	}

	if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
	{
		initGlobals();
	}

	if (button == 3 && state == GLUT_DOWN)
	{
		CameraControl.camY += 0.1;
	}

	if (button == 4 && state == GLUT_DOWN)
	{
		CameraControl.camY -= 0.1;
	}

	if (CameraControl.leftMouseDown || CameraControl.rightMouseDown)
	{
		CameraControl.lastMouseX = x;
		CameraControl.lastMouseY = y;
	}

}

void mouseMotion(int x, int y)
{
	
	if (CameraControl.leftMouseDown)
	{
		int dX, dY;
		dX = x - CameraControl.lastMouseX;
		dY = y - CameraControl.lastMouseY;
		CameraControl.xRotation -= dY;
		CameraControl.yRotation -= dX;
		if (CameraControl.xRotation > 90)
		{
			CameraControl.xRotation = 90;
		}
		if (CameraControl.xRotation < -90)
		{
			CameraControl.xRotation = -90;
		}
	}

	if (CameraControl.rightMouseDown)
	{
		float dX, dY;
		dX = ((float)x - CameraControl.lastMouseX) / -500;
		dY = ((float)y - CameraControl.lastMouseY) / 500;

		CameraControl.camX -= dX;
		CameraControl.camZ += dY;

	}

	if (CameraControl.leftMouseDown || CameraControl.rightMouseDown)
	{
		CameraControl.lastMouseX = x;
		CameraControl.lastMouseY = y;
	}

}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(110, (float)w/(float)h, 0.01, 100);
	glMatrixMode(GL_MODELVIEW);
}

void update()
{
	Wave.displacement = glutGet(GLUT_ELAPSED_TIME) / (float) 1000;

	glutPostRedisplay();
}

void init()
{
	/* In this program these OpenGL calls only need to be done once,
	but normally they would go elsewhere, e.g. display */

	glMatrixMode(GL_PROJECTION);
	glOrtho(-1, 1, -1, 1, -100, 100);
	glMatrixMode(GL_MODELVIEW);
	initGlobals();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_NORMALIZE);
	glClearColor(0, 0, 0, 255);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	//Window
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(400, 100);
	glutCreateWindow("Tutorial 1");

	init();

	glutDisplayFunc(display);
	glutIdleFunc(update);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotion);
	glutReshapeFunc(reshape);
	glutMainLoop();

	return EXIT_SUCCESS;
}