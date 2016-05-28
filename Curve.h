#pragma once
#include "Geode.h"
#include "Point.h"
#include <vector>

#define NUM_POINTS 100
class Curve : Geode
{
public:
	Curve();
	Curve(glm::mat4 pointMat);
	Curve(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4);
	~Curve();

	void reevaluate();
	void draw(GLuint shaderProgram);
	void selectionDraw(GLuint shaderProgram);
	void update(glm::mat4);
	int factorial(int);
	void setPoint(int index, glm::vec3 point);
	glm::vec3 getPoint(int index);
	Point* getControlPoint(int index);
	void setControlPoint(int, Point*);
	void setupBuffers();
	static glm::vec3 maxHeight;
	glm::vec3 eval(float t);

private:
	Point* controls[4];
	glm::mat4 toWorld;
	GLuint VBO, VAO, PAO, PBO, EBO;
	std::vector<glm::vec3> points;
	void assignPointsFromMat();
	float C(int i, float t);

};

