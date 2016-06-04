#pragma once
#include "Node.h"
#include "OBJObject.h"

class Geode : public Node
{
public:
	Geode();
	Geode(OBJObject *obj);
	~Geode();
	void draw(GLuint shaderProgram);
	void update(glm::mat4 C);

protected:
	OBJObject *obj;
	glm::mat4 toWorld = glm::mat4();
};

