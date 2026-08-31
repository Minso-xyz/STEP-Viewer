#include "Renderer.h"
#include <GLFW/glfw3.h>

void Renderer::DrawPoint(const Point3D& point)
{
	glPointSize(10.0f);
	glBegin(GL_POINTS);
	glColor3f(1.0f, 0.0f, 1.0f);
	glVertex3f((float)point.X, (float)point.Y, (float)point.Z);
	glEnd();
}

void Renderer::DrawLine(const Line3D& line)
{
	glLineWidth(2.0f);
	glBegin(GL_LINES);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f((float)line.Start.X, (float)line.Start.Y, (float)line.Start.Z);
	glVertex3f((float)line.End.X, (float)line.End.Y, (float)line.End.Z);
	glEnd();
}

void Renderer::DrawVertex(Vertex vertex)
{
	Renderer::DrawPoint(vertex.Position);
}

void Renderer::DrawCoordinateAxis()
{
	glLineWidth(1.0f);
	glBegin(GL_LINES);

	// X-Axis (Red)
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(-0.8f, 0.0f, 0.0f);
	glVertex3f(0.8f, 0.0f, 0.0f);

	// Y-Axis (Green)
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, -0.8f, 0.0f);
	glVertex3f(0.0f, 0.8f, 0.0f);

	// Z-Axis (Blue)
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, -0.8f);
	glVertex3f(0.0f, 0.0f, 0.8f);

	glEnd();
}
