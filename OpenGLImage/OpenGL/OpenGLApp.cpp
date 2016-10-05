#include "OpenGLApp.h"
#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <cmath>
#include "OpenGLApp.h"

OpenGLApp::OpenGLApp()
{
}


OpenGLApp::~OpenGLApp()
{
}


bool OpenGLApp::Initialize() // initialize your opengl scene
{
	//clear and change background to black
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	return true;
} 

bool OpenGLApp::Shutdown() // shut down your scene
{
	return true;
}  

// used to setup your opengl camera and drawing area
void OpenGLApp::SetupProjection(int width, int height)
{
	if (height == 0)
	{
		height = 1;
	}

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// you need 3 things to create a frustum
	// 1. Field of view (52.0f here)
	// 2. aspect ratio of screen (width/height of window)
	// 3. near and far plane (1, and 1000)
	gluPerspective(60.0f, (GLfloat)width / (GLfloat)height, 1.0f, 1000.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	m_windowWidth = width;
	m_windowHeight = height;
}

void OpenGLApp::Prepare(float dt)
{

}

void OpenGLApp::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -10.0f);

	
	glColor3f(0.0f, 0.5f, 1.0f); //blue
	

	//ShowGrid();

}

void OpenGLApp::ShowGrid()
{
	glPointSize(3.0f); //change the size of points drawn 

	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 1.0f); //blue

	//draw vertical lines
	for (int x = -6; x <= 6; x++)
	{
		glVertex3f(x, 4.0f, 0.0f); //start of line (0, 4, 0)
		glVertex3f(x, -4.0f, 0.0f); //end of line  (0, -4, 0)
	}

	//draw horizontal lines
	for (int y = -4; y <= 4; y++)
	{
		glVertex3f(-6.0f, y, 0.0f); //start of line (, 4, 0)
		glVertex3f(6.0f, y, 0.0f); //end of line  (0, -4, 0)
	}
	glEnd();



	//draw points on grid
	glBegin(GL_POINTS);
	//X AXIS
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-6.0f, 0.0f, 0.0f);
	glVertex3f(-5.0f, 0.0f, 0.0f);
	glVertex3f(-4.0f, 0.0f, 0.0f);
	glVertex3f(-3.0f, 0.0f, 0.0f);
	glVertex3f(-2.0f, 0.0f, 0.0f);
	glVertex3f(-1.0f, 0.0f, 0.0f);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f); //origin
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, 0.0f, 0.0f);
	glVertex3f(2.0f, 0.0f, 0.0f);
	glVertex3f(3.0f, 0.0f, 0.0f);
	glVertex3f(4.0f, 0.0f, 0.0f);
	glVertex3f(5.0f, 0.0f, 0.0f);
	glVertex3f(6.0f, 0.0f, 0.0f);

	//Y Axis
	glVertex3f(0.0f, -4.0f, 0.0f);
	glVertex3f(0.0f, -3.0f, 0.0f);
	glVertex3f(0.0f, -2.0f, 0.0f);
	glVertex3f(0.0f, -1.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 2.0f, 0.0f);
	glVertex3f(0.0f, 3.0f, 0.0f);
	glVertex3f(0.0f, 4.0f, 0.0f);

	glEnd();


}