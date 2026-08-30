#include <iostream>
#include <GLFW/glfw3.h>
#include "Point3D.h"

void DrawPoint(const Point3D& point);

int main()
{
	if (!glfwInit())
	{
		return -1;
	}

	GLFWwindow* window = glfwCreateWindow(800, 600, "STEP Viewer", nullptr, nullptr);  // create the window

	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	while (!glfwWindowShouldClose(window))
	{
		// Background with black color
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glBegin(GL_LINES);

		// X-Axis (Red)
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex2f(-0.8f, 0.0f);
		glVertex2f(0.8f, 0.0f);

		// Y-Axis (Green)
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex2f(0.0f, -0.8f);
		glVertex2f(0.0f, 0.8f);

		glEnd();

		// Draw Point3D from GeoKernel3D
		Point3D point(0.5, 0.3, 0);
		DrawPoint(point);

		glfwSwapBuffers(window);

		glfwPollEvents();   // handle the mouse/keyboard inputs
	}

	glfwTerminate();

	return 0;
}

void DrawPoint(const Point3D& point)
{
	glPointSize(10.0f);
	glBegin(GL_POINTS);
	glColor3f(1.0f, 0.0f, 1.0f);
	glVertex2f((float)point.X, (float)point.Y);
	glEnd();
}