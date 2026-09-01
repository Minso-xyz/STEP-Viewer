#include <iostream>
#include <GLFW/glfw3.h>
#include "Point3D.h"
#include "Line3D.h"
#include "Renderer.h"
#include <vector>
#include "Camera.h"

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

	glEnable(GL_DEPTH_TEST);

	Camera camera;
	Renderer renderer;

	Point3D point(0.5, 0.3, 0);
	Line3D line(Point3D(-0.5, -0.5, 0), Point3D(0.5, 0.5, 0));

	while (!glfwWindowShouldClose(window))
	{
		int width;
		int height;

		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		camera.ApplyProjection(width, height);
		camera.ApplyView();
		renderer.DrawCoordinateAxis();

		Point3D point(0.5, 0.3, 0.0);
		renderer.DrawPoint(point);

		Line3D line(
			Point3D(-0.5, -0.5, 0.0),
			Point3D(0.5, 0.5, 0.0));

		renderer.DrawLine(line);

		//// Background with black color
		//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		//glClear(GL_COLOR_BUFFER_BIT);
		//
		//renderer.DrawCoordinateAxis();   // Draw X,Y coordinate axis
		//renderer.DrawPoint(point);   // Draw Point3D from GeoKernel3D
		//renderer.DrawLine(line);   // Draw Line3D

		renderer.DrawCube();   // Draw cube

		glfwSwapBuffers(window);
		glfwPollEvents();   // handle the mouse/keyboard inputs
	}

	glfwTerminate();

	return 0;
}