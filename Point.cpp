#include "Point.h"

int Point::currentID = 0;
std::vector<Point*> Point::allPoints =  std::vector<Point*>();

Point::Point()
{
	p = glm::vec3(0.0, 0.0, 0.0);
	ID = Point::currentID++;
	Point::allPoints.push_back(this);
}

Point::Point(glm::vec3 v) {
	p = v;
	ID = Point::currentID++;
	Point::allPoints.push_back(this);
}

int Point::getId() {
	return ID;
}

Point::~Point()
{
}
