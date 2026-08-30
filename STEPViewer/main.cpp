#include <iostream>
#include <GLFW/glfw3.h>

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

		// Create a point in the center
		glPointSize(10.f);
		glBegin(GL_POINTS);

		// In red color
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex2f(0.0f, 0.0f);
		glEnd();

		glfwSwapBuffers(window);

		glfwPollEvents();   // handle the mouse/keyboard inputs
	}

	glfwTerminate();

	return 0;
}