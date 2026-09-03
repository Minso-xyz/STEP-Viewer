#pragma once
#include "Edge.h"
#include <vector>
#include "Renderer.h"
#include <map>
#include <string>

class STEPReader
{
private:
	std::map<std::string, std::string> entityMap;
public:
	std::vector<std::string> ReadAllLines(const std::string& filePath);
	void BuildEntityMap(const std::vector<std::string>& lines);
	Point3D ParseCartesianPoint(const std::string& line);
	std::vector<Point3D> ExtractPointsFromAllLines(std::vector<std::string> lines);
	void DrawPoints(Renderer& renderer, std::vector<Point3D> points);
	Point3D ParseVertexPoint(const std::string& line);
	std::vector<Vertex> ExtractVerticesFromAllLines(std::vector<std::string> lines);
	void DrawVertices(Renderer& renderer, std::vector<Vertex>& vertices);
};