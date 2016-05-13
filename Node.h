#ifndef _NODE_H_
#define _NODE_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>

class Node
{
public:
	Node();
	~Node();
	virtual void draw(GLuint) = 0;
	virtual void update(glm::mat4 C) = 0;
};

#endif