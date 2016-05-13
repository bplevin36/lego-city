#pragma once
#include "Group.h"
#include <glm\mat4x4.hpp>

class MatrixTransform :
	public Group
{
public:
	MatrixTransform();
	MatrixTransform(glm::mat4);
	void update(glm::mat4 C);
	void updateTransform(glm::mat4);
	void animate();
	bool animated = false;
private:
	glm::mat4 M;
	int frameCount = 0;
};

