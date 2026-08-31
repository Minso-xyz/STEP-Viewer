#pragma once
#include "Point3D.h"
#include "Line3D.h"
#include "Vertex.h"

class Renderer
{
public:

	void DrawCoordinateAxis();
	void DrawPoint(const Point3D& point);
	void DrawLine(const Line3D& line);
	void DrawVertex(Vertex vertex);
};