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

		// Draw Point
		Point3D point(0.5, 0.3, 0.0);
		renderer.DrawPoint(point);

		// Draw Line
		Line3D line(
			Point3D(-0.5, -0.5, 0.0),
			Point3D(0.5, 0.5, 0.0));
		renderer.DrawLine(line);

		// Draw cube
		renderer.DrawCube();   

		glfwSwapBuffers(window);
		glfwPollEvents();   // handle the mouse/keyboard inputs
		camera.HandleInput(window);
	}

	glfwTerminate();

	return 0;
}