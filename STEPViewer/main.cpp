#include <iostream>
#include <GLFW/glfw3.h>
#include "Point3D.h"
#include "Line3D.h"
#include "Renderer.h"
#include <vector>
#include "Camera.h"
#include "STEPReader.h"

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
	STEPReader reader;

	// Read the WireframeCube.stp file
	std::vector<std::string> lines = reader.ReadAllLines("..\\WireframeCube.stp");
	reader.BuildEntityMap(lines);
	std::vector<Point3D> points = reader.ExtractPointsFromAllLines(lines);
	std::vector<Vertex> vertices = reader.ExtractVerticesFromAllLines(lines);
	std::vector<Edge> edges = reader.ExtractEdgesFromAllLines(lines);
	std::vector<Vector3D> directions = reader.ExtractDirectionsFromAllLines(lines);
	std::vector<Vector3D> vectors = reader.ExtractVectorsFromAllLines(lines);

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

		//// Draw Point
		//Point3D point(0.5, 0.3, 0.0);
		//renderer.DrawPoint(point);

		//// Draw Line
		//Line3D line(
		//	Point3D(-0.5, -0.5, 0.0),
		//	Point3D(0.5, 0.5, 0.0));
		//renderer.DrawLine(line);

		//// Draw cube
		//renderer.DrawCube();

		// Draw Wireframe cube from the step file
		//reader.DrawPoints(renderer, points);
		reader.DrawVertices(renderer, vertices);
		reader.DrawEdges(renderer, edges);

		glfwSwapBuffers(window);
		glfwPollEvents();   // handle the mouse/keyboard inputs
		camera.HandleInput(window);
	}

	glfwTerminate();

	return 0;
}