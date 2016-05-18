#pragma once
#include "Geode.h"
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
	void update(glm::mat4);
	int factorial(int);
	void setPoint(int index, glm::vec3 point);
	glm::vec3 getPoint(int index);
	void setupBuffers();

private:
	glm::mat4 p;
	glm::mat4 toWorld;
	GLuint VBO, VAO, PAO, PBO, EBO;

	std::vector<glm::vec3> points;

	glm::vec3 eval(float t);
	float C(int i, float t);

};

