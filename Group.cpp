#include "Group.h"



Group::Group()
{
}


Group::~Group()
{
}

void Group::addChild(Node* added)
{
	children.push_back(added);
}

void Group::removeChild(Node* removed)
{
	children.remove(removed);
}

void Group::clearChildren()
{
	children.clear();
}

void Group::draw(GLuint shaderProgram)
{
	for (std::list<Node*>::iterator it = children.begin(); it != children.end(); ++it) {
		(*it)->draw(shaderProgram);
	}
}

void Group::update(glm::mat4 C) {
	for (std::list<Node*>::iterator it = children.begin(); it != children.end(); ++it) {
		(*it)->update(C);
	}
}
