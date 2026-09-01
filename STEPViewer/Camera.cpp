#include "Camera.h"
#include <GLFW/glfw3.h>

Camera::Camera()
{
	Distance = 5.0f;
	Yaw = 45.0f;
	Pitch = 35.0f;
	Zoom = 1.5f;
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