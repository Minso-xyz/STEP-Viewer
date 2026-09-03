#include "STEPReader.h"
#include "Edge.h"
#include <vector>
#include <string>
#include <fstream>
#include "Point3D.h"
#include "Renderer.h"
#include <sstream>
#include <map>

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




