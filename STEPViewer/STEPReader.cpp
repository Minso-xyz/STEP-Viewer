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
#include "Circle3D.h"
#include "Axis2Placement3D.h"
#include "BoundingBox.h"
#include "BSplineCurve.h"

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

std::string STEPReader::GetCompleteEntity(int startLineIndex, std::vector<std::string> lines)
{
	std::string entityComplete;

	for (int i = startLineIndex; i < lines.size(); i++)
	{
		entityComplete += lines[i];

		if (lines[i].find(';') != std::string::npos)
		{
			break;
		}
	}
	return entityComplete;
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
	std::string idStr = line.substr(start + 1, 50);  // #10);

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

	std::string pointLine = entityMap[pointId];
	std::string vectorLine = entityMap[vectorId];

	Point3D startPoint = ParseCartesianPoint(pointLine);
	Vector3D vector = ParseVector(vectorLine);
	Point3D endPoint = Point3D(
		startPoint.X + vector.X,
		startPoint.Y + vector.Y,
		startPoint.Z + vector.Z);

	return Line3D(startPoint, endPoint);
}

Circle3D STEPReader::ParseCircle(const std::string& line)
{
	// #2124 = CIRCLE('', #2337, 55.);
	int start = line.find(',');
	std::string idStr = line.substr(start + 1, 50);   // returns #2337, 55.);

	std::stringstream ss(idStr);
	std::string positionId;
	std::string radiusText;

	// returns the ID of AXIS2_PLACEMENT_3D and radius
	std::getline(ss, positionId, ',');
	std::getline(ss, radiusText, ')');

	std::string positionLine = entityMap[positionId];   // #2336=AXIS2_PLACEMENT_3D('',#3398,#2666,#2667);
	double radius = std::stod(radiusText);

	Axis2Placement3D position = ParseAxis2Placement3D(positionLine);

	return Circle3D(position, radius);
}

Axis2Placement3D STEPReader::ParseAxis2Placement3D(const std::string& line)
{
	// #2336=AXIS2_PLACEMENT_3D('',#3398,#2666,#2667);
	int start = line.find(',');
	std::string idStr = line.substr(start + 1, 20);   // returns #3398,#2666,#2667);

	std::stringstream ss(idStr);
	std::string originId;
	std::string normalId;
	std::string xDirectionId;

	// returns the ID of Origin (CARTESIAN_POINT), Normal/Z-direction (VECTOR), X-direction/Ref-direction (VECTOR)
	std::getline(ss, originId, ',');
	std::getline(ss, normalId, ',');
	std::getline(ss, xDirectionId, ')');

	std::string originLine = entityMap[originId];
	std::string normalLine = entityMap[normalId];
	std::string xDirectionLine = entityMap[xDirectionId];

	Point3D origin = STEPReader::ParseCartesianPoint(originLine);
	Vector3D normal = STEPReader::ParseDirection(normalLine);
	Vector3D xDirection = STEPReader::ParseDirection(xDirectionLine);

	return Axis2Placement3D(origin, normal, xDirection);
}

// #115=B_SPLINE_CURVE_WITH_KNOTS
	// ('',
	// 3,    <------------------------------------ Degree (Cubic B-Spline)
	// (
	// #3597,#3598,#3599,#3600,      <------------ ControlPoints (total 16)
	// #3601,#3602, #3603, #3604, 
	// #3605, #3606, #3607, #3608, 
	// #3609, #3610, #3611, #3612
	// ),
	// 
	// .UNSPECIFIED.,        <-------------------- .UNSPECIFIED./ .POLYLINE_FORM./ .CIRCULAR_ARC./ .ELLIPTIC_ARC./ .PARABOLIC_ARC.
	// .F.,                  <-------------------- CloasedCurve = false; Open Curve
	// .F.,                  <-------------------- SelfIntersect = false
	// 
	// (4, 2, 2, 2, 2, 2, 2, 4),    <------------ KnotMultiplicities
	// 
	// (                                      *** DistinctKnots
	// 0.,                   <------------------- repeats 4 x 0.0 
	// 0.125,                <------------------- repeats 2 x 0.125 
	// 0.249999999999999,    <------------------- repeats 2 x 0.24999...
	// 0.499999999999999,    <------------------- repeats 2 x 0.499999...
	// 0.624999999999999,    <------------------- repeats 2 x 0.624999...
	// 0.749999999999999,    <------------------- repeats 2 x 0.749999...
	// 0.875,                <------------------- repeats 2 x 0.875
	// 1.),                  <------------------- repeats 4 x 1.0
	// 
	// .UNSPECIFIED.         <------------------- KNOT_TYPE (Enum); .UNIFORM_KNOTS./ .QUASI_UNIFORM_KNOTS./ .PIECEWISE_BEZIER_KNOTS./ .UNSPECIFIED.
	// );

	// #115=B_SPLINE_CURVE_WITH_KNOTS('',3,(#3597,#3598,#3599,#3600,#3601,#3602,
	// #3603, #3604, #3605, #3606, #3607, #3608, #3609, #3610, #3611, #3612),
	// .UNSPECIFIED., .F., .F., (4, 2, 2, 2, 2, 2, 2, 4), (0., 0.125, 0.249999999999999, 0.499999999999999,
	// 0.624999999999999, 0.749999999999999, 0.875, 1.), .UNSPECIFIED.);

BSplineCurve STEPReader::ParseBSplineCurveWithKnots(const std::string line)
{
	// Remove the entity name from the line
	size_t start = line.find('(');
	size_t end = line.rfind(')');
	std::string parameters = line.substr(start + 1,end - start - 1);

	// Split the parameter into degree, controlPoints, multiplicityCounts, and knots
	auto tokens = SplitTopLevelParameters(parameters);
	int degree = std::stoi(tokens[1]);
	std::string controlPointBlock = tokens[2];
	std::string multiplicityBlock = tokens[6];
	std::string knotBlock = tokens[7];

	// Parse Control points
	std::vector<Point3D> controlPoints = ParseControlPoints(controlPointBlock);

	// Parse multiplicity numbers
	std::vector<int> multiplicityNums = ParseMultiplicityNumbers(multiplicityBlock);

	// Parse Knots
	std::vector<double> knots = ParseKnots(knotBlock);

	// Multiple the number of the knots
	std::vector<double> finalKnots = CalculateFinalKnots(multiplicityNums, knots);

	return BSplineCurve(
		controlPoints,
		finalKnots,
		degree
	);
}

// Split the long line of entity and sort the types of parameters

// Example of the raw entity data
// '', 3,
// (#1, #2, #3),
// .F.,
// (4, 2, 2, 4),
// (0.0, 0.5, 1.0)

// Result
// tokens[0] = ''
// tokens[1] = 3
// tokens[2] = (#1, #2, #3)
// tokens[3] = .F.
// tokens[4] = (4, 2, 2, 4)
// tokens[5] = (0.0, 0.5, 1.0)
std::vector<std::string> STEPReader::SplitTopLevelParameters(const std::string& text)
{
	std::vector<std::string> tokens;
	std::string current;
	int depth = 0;

	for (char c : text)
	{
		if (c == '(')
		{
			depth++;
			current += c;
		}
		else if (c == ')')
		{
			depth--;
			current += c;
		}
		else if (c == ',' && depth == 0)
		{
			tokens.push_back(current);
			current.clear();
		}
		else
		{
			current += c;
		}
	}

	if (!current.empty())
	{
		tokens.push_back(current);
	}
	return tokens;
}

std::vector<Point3D> STEPReader::ParseControlPoints(std::string controlPointsBlock)
{
	// controlPointsBlock : (#1, #2, #3, ...)
	int start = controlPointsBlock.find('(');
	int end = controlPointsBlock.find(')');
	std::string strPoints = controlPointsBlock.substr(start + 1, end - (start + 1));

	// Extract the id of the cartesian points
	std::stringstream ss(strPoints);
	std::vector<std::string> cartesianPointIds;
	std::string id;
	while (std::getline(ss, id, ','))
	{
		cartesianPointIds.push_back(id);
	}

	// Extract the entity line of the cartesian points
	std::vector<std::string> entityLines;
	std::string entityLine;
	for (auto id : cartesianPointIds)
	{
		entityLine = entityMap[id];
		entityLines.push_back(entityLine);
	}

	// Parse the cartesian points
	std::vector<Point3D> controlPoints;
	Point3D controlPoint;
	for (auto entityLine : entityLines)
	{
		controlPoint = ParseCartesianPoint(entityLine);
		controlPoints.push_back(controlPoint);
	}
	return controlPoints;
}

std::vector<int> STEPReader::ParseMultiplicityNumbers(std::string multiplicityBlock)
{
	// Multiplicity block : (4, 2, 2, 4)
	int start = multiplicityBlock.find('(');
	int end = multiplicityBlock.find(')');
	std::string strMultiNums = multiplicityBlock.substr(start + 1, end - (start + 1));

	// Extract Multiplicity numbers
	std::stringstream ss(strMultiNums);
	std::vector<int> multiplicityNums;
	std::string multiplicityNum;
	while (std::getline(ss, multiplicityNum, ','))
	{
		multiplicityNums.push_back(std::stoi(multiplicityNum));
	}
	return multiplicityNums;
}

std::vector<double> STEPReader::ParseKnots(std::string knotBlock)
{
	// Knot block : (0.0, 0.5, 1.0)
	int start = knotBlock.find('(');
	int end = knotBlock.find(')');
	std::string strKnots = knotBlock.substr(start + 1, end - (start + 1));

	// Extract Knots
	std::stringstream ss(strKnots);
	std::vector<double> knots;
	std::string knot;
	while (std::getline(ss, knot, ','))
	{
		knots.push_back(std::stoi(knot));
	}
	return knots;
}

std::vector<double> STEPReader::CalculateFinalKnots(std::vector<int> multiplicityNums, std::vector<double> knots)
{
	std::vector<double> knotsFinal;

	if (knots.size() != multiplicityNums.size())
	{
		throw std::runtime_error("Knot and multiplicity counts do not match.");
	}

	for (int i = 0; i < knots.size(); i++)
	{
		for (int j = 0; j < multiplicityNums[i]; j++)
		{
			knotsFinal.push_back(knots[i]);
		}
	}
	return knotsFinal;
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

std::vector<Line3D> STEPReader::ExtractLinesFromAllLines(std::vector<std::string> lines)
{
	std::vector<Line3D> lines3D;
	Line3D line3D;

	for (const auto& line : lines)
	{
		if (line.find("=LINE") != std::string::npos)
		{
			line3D = ParseLine(line);
			lines3D.push_back(line3D);
		}
	}
	return lines3D;
}

std::vector<Circle3D> STEPReader::ExtractCirclesFromAllLines(std::vector<std::string> lines)
{
	std::vector<Circle3D> circles;
	Circle3D circle;

	for (const auto& line : lines)
	{
		if (line.find("=CIRCLE") != std::string::npos)
		{
			circle = ParseCircle(line);
			circles.push_back(circle);
		}
	}
	return circles;
}

std::vector<BSplineCurve> STEPReader::ExtractBSplineCurvesFromAllLines(std::vector<std::string> lines)
{
	std::vector<BSplineCurve> curves;

	for (int i = 0; i<lines.size();i++)
	{
		if (lines[i].find("=B_SPLINE_CURVE_WITH_KNOTS") != std::string::npos)
		{
			std::string entity = GetCompleteEntity(i, lines);
			BSplineCurve curve = ParseBSplineCurveWithKnots(entity);
			curves.push_back(curve);
		}
	}
	return curves;
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

void STEPReader::DrawLines(Renderer& renderer, std::vector<Line3D> lines)
{
	for (const auto& line : lines)
	{
		renderer.DrawLine(line);
	}
}

void STEPReader::DrawCircles(Renderer& renderer, std::vector<Circle3D> circles)
{
	for (const auto& circle : circles)
	{
		renderer.DrawCircle(circle);
	}
}

void STEPReader::DrawBSplineCurves(Renderer& renderer, std::vector<BSplineCurve> curves)
{
	for (const auto& curve : curves)
	{
		renderer.DrawBSplineCurve(curve);
	}
}





