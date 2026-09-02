#pragma once
#include <GLFW/glfw3.h>

class Camera
{
public:
	float Distance;   // distance between the camera and the model
	float Yaw;   // Right/Left rotation angle
	float Pitch;   // Up/Down rotation angle
	float Zoom;

public:
	Camera();

	void ApplyProjection(int width, int height);

	void ApplyView();

	void HandleInput(GLFWwindow* window);
};