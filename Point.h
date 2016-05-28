#pragma once
#include "Node.h"
#include <vector>
class Point
{
public:
	Point();
	Point(glm::vec3);
	~Point();
	int getId();
	static std::vector<Point*> allPoints;
	glm::vec3 p;

private:
	static int currentID;
	int ID;
};

