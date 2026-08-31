#include <iostream>
#include <GLFW/glfw3.h>
#include "Point3D.h"
#include "Line3D.h"
#include "Renderer.h"

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

	Renderer renderer;

	Point3D point(0.5, 0.3, 0);
	Line3D line(Point3D(-0.5, -0.5, 0), Point3D(0.5, 0.5, 0));

	while (!glfwWindowShouldClose(window))
	{
		// Background with black color
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		renderer.DrawCoordinateAxis();   // Draw X,Y coordinate axis
		renderer.DrawPoint(point);   // Draw Point3D from GeoKernel3D
		renderer.DrawLine(line);   // Draw Line3D

		glfwSwapBuffers(window);
		glfwPollEvents();   // handle the mouse/keyboard inputs
	}

	glfwTerminate();

	return 0;
}