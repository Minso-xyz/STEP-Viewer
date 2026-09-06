#include "Camera.h"
#include <GLFW/glfw3.h>

Camera::Camera()
{
	Distance = 5.0f;
	Yaw = 45.0f;
	Pitch = -135.0f;
	Zoom = 300.0f;
}

void Camera::ApplyProjection(int width, int height)
{
	if (height == 0)
	{
		height = 1;
	}

	float aspectRatio = static_cast<float>(width) / static_cast<float>(height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(
		-Zoom * aspectRatio,
		Zoom * aspectRatio,
		-Zoom,
		Zoom,
		-100.0,
		100.0);

	glMatrixMode(GL_MODELVIEW);
}

void Camera::ApplyView()
{
	glLoadIdentity();

	glTranslatef(
		0.0f,
		0.0f,
		-Distance);

	glRotatef(
		Pitch,
		1.0f,
		0.0f,
		0.0f
	);

	glRotatef(
		Yaw,
		0.0f,
		1.0f,
		0.0f);
}

void Camera::HandleInput(GLFWwindow* window)
{
	// Zoom-in
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		 Zoom -= 0.01f;
	}

	// Zoom-out
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		Zoom += 0.01f;
	}

	// Orbit Left
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		Yaw -= 0.05f;
	}

	// Orbit Right
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		Yaw += 0.05f;
	}

	// Pitch Up
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		Pitch -= 0.05f;
	}

	// Pitch Down
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		Pitch += 0.05f;
	}
}