#pragma once
#include "Point3D.h"
#include "Line3D.h"
#include "Vertex.h"
#include "Edge.h"
#include "Circle3D.h"

class Renderer
{
public:

	void DrawCoordinateAxis();
	void DrawPoint(const Point3D& point);
	void DrawLine(const Line3D& line);
	void DrawCircle(const Circle3D& circle);
	void DrawVertex(const Vertex& vertex);
	void DrawEdge(const Edge& edge);
	void DrawCube();
};