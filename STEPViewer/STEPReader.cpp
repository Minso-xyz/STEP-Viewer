#include "STEPReader.h"
#include "Edge.h"
#include <vector>
#include <string>
#include <fstream>
#include "Point3D.h"
#include "Renderer.h"
#include <sstream>
#include <map>
#include "Line3D.h"

std::vector<std::string> STEPReader::ReadAllLines(const std::string& filePath)
{
	std::vector<std::string> lines;
	std::ifstream file(filePath);
		
	if (!file.is_open())
	{
		return lines;
	}

	std::string line;

	while (std::getline(file, line))
	{
		lines.push_back(line);
	}
	return lines;
}

// Add <id, line> to "entityMap" dictionary
void STEPReader::BuildEntityMap(const std::vector<std::string>& lines)
{
	entityMap.clear();

	for (const auto& line : lines)
	{
		int idLength = line.find('=');

		if (idLength == std::string::npos)
		{
			continue;
		}
		std::string id = line.substr(0, idLength);
		entityMap[id] = line;
	}
}

Point3D STEPReader::ParseCartesianPoint(const std::string& line)
{
	// line : #1247=CARTESIAN_POINT('',(0.,0.,0.));
	// coords : 0.,0.,0.

	int start = line.find("('',(");
	int end = line.rfind(")");
	std::string coords = line.substr(start + 5, end - (start + 5));

	std::stringstream ss(coords);
	std::string x_str;
	std::string y_str;
	std::string z_str;

	// Split the extracted coordinates by ','
	std::getline(ss, x_str, ',');
	std::getline(ss, y_str, ',');
	std::getline(ss, z_str, ',');

	double x = std::stof(x_str);
	double y = std::stof(y_str);
	double z = std::stof(z_str);

	return Point3D(x, y, z);
}

Point3D STEPReader::ParseVertexPoint(const std::string& line)
{
	// Line: #20=VERTEX_POINT('',#10);
	int start = line.find(',');
	std::string idStr = line.substr(start + 1, 10);  // #10);

	std::stringstream ss(idStr);
	std::string pointId;
	std::getline(ss, pointId, ')');   // returns pointId : #10

	std::string pointLine = entityMap[pointId];   // Find the line of CARTESIAN_POINT responding to pointId
	return ParseCartesianPoint(pointLine);   // returns Point3D
}

Edge STEPReader::ParseEdgeCurve(const std::string& line)
{
	// #70=EDGE_CURVE('',#20,#21,#50,.T.);
	// #20 : Start / #21 : End
	int start = line.find(',');
	std::string idStr = line.substr(start + 1, 20);   // returns #20,#21,#50,.T.);

	std::stringstream ss(idStr);
	std::string startId;
	std::string endId;

	// returns the start-end point id
	std::getline(ss, startId, ',');
	std::getline(ss, endId, ',');

	// retrieve the vertex line corresponding to startId-endId
	std::string startVertexLine = entityMap[startId];
	std::string endVertexLine = entityMap[endId];

	// parse the point
	Point3D startPoint = ParseVertexPoint(startVertexLine);
	Point3D endPoint = ParseVertexPoint(endVertexLine);

	return Edge(startPoint, endPoint);
}

Vector3D STEPReader::ParseDirection(const std::string& line)
{
	// #30=DIRECTION('',(1.0,0.0,0.0));
	int start = line.find(',');
	int end = line.find(';');
	std::string coords = line.substr(start + 2, end - 2 -(start + 2));   // returns 1.0,0.0,0.0

	std::stringstream ss(coords);
	std::string xText;
	std::string yText;
	std::string zText;

	std::getline(ss, xText, ',');
	std::getline(ss, yText, ',');
	std::getline(ss, zText, ')');

	double x = std::stod(xText);
	double y = std::stod(yText);
	double z = std::stod(zText);

	return Vector3D(x,y,z);
}

Vector3D STEPReader::ParseVector(const std::string& line)
{
	// #40=VECTOR('',#30,1.0);   // #30: Direction, 1.0: Magnitude
	int start = line.find(',');
	int end = line.find(';');
	std::string str = line.substr(start + 1, end - 1 - (start + 1));   // returns #30,1.0

	std::stringstream ss(str);
	std::string vectorId;
	std::string magnitudeText;

	std::getline(ss, vectorId, ',');   // #31
	std::getline(ss, magnitudeText, ',');   // 1.0

	std::string vectorLine = entityMap[vectorId];   // #31=DIRECTION('',(0.0,1.0,0.0));
	Vector3D direction = ParseDirection(vectorLine);
	double magnitude = std::stof(magnitudeText);

	return direction * magnitude;
}

Line3D STEPReader::ParseLine(const std::string& line)
{
	// #50=LINE('',#10,#40);
	int start = line.find(',');
	std::string idStr = line.substr(start + 1, 20);   // returns #10,#40);

	std::stringstream ss(idStr);
	std::string pointId;
	std::string vectorId;

	// returns CARTESIAN_POINT and VECTOR
	std::getline(ss, pointId, ',');
	std::getline(ss, vectorId, ')');

	return Line3D(Point3D(0, 0, 0), Point3D(0, 0, 0));;
}

std::vector<Vertex> STEPReader::ExtractVerticesFromAllLines(std::vector<std::string> lines)
{
	std::vector<Vertex> vertices;
	Vertex vertex;

	for (int i = 0; i < lines.size(); i++)
	{
		if (lines[i].find("=VERTEX_POINT") != std::string::npos)
		{
			Point3D point = ParseVertexPoint(lines[i]);
			vertices.push_back(Vertex(point));
		}
	}
	return vertices;
}

std::vector<Point3D> STEPReader::ExtractPointsFromAllLines(std::vector<std::string> lines)
{
	std::vector<Point3D> points;
	Point3D point;

	for (int i = 0; i < lines.size(); i++)
	{
		if (lines[i].find("=CARTESIAN_POINT") != std::string::npos)
		{
			point =  ParseCartesianPoint(lines[i]);
			points.push_back(point);
		}
	}
	return points;
}

std::vector<Edge> STEPReader::ExtractEdgesFromAllLines(std::vector<std::string> lines)
{
	std::vector<Edge> edges;
	Edge edge;

	for (const auto& line : lines)
	{
		if (line.find("=EDGE_CURVE") != std::string::npos)
		{
			edge = ParseEdgeCurve(line);
			edges.push_back(edge);
		}
	}
	return edges;
}

std::vector<Vector3D> STEPReader::ExtractDirectionsFromAllLines(std::vector<std::string> lines)
{
	std::vector<Vector3D> directions;
	Vector3D direction;

	for (const auto& line : lines)
	{
		if (line.find("=DIRECTION") != std::string::npos)
		{
			direction = ParseDirection(line);
			directions.push_back(direction);
		}
	}
	return directions;
}

std::vector<Vector3D> STEPReader::ExtractVectorsFromAllLines(std::vector<std::string> lines)
{
	std::vector<Vector3D> vectors;
	Vector3D vector;

	for (const auto& line : lines)
	{
		if (line.find("=VECTOR") != std::string::npos)
		{
			vector = ParseVector(line);
			vectors.push_back(vector);
		}
	}
	return vectors;
}

void STEPReader::DrawPoints(Renderer& renderer, std::vector<Point3D> points)
{
	for (int i = 0; i < points.size(); i++)
	{
		renderer.DrawPoint(points[i]);
	}
}

void STEPReader::DrawVertices(Renderer& renderer, std::vector<Vertex>& vertices)
{
	for (int i = 0; i < vertices.size(); i++)
	{
		renderer.DrawVertex(vertices[i]);
	}
}

void STEPReader::DrawEdges(Renderer& renderer, std::vector<Edge> edges)
{
	for (const auto& edge : edges)
	{
		renderer.DrawEdge(edge);
	}
}




