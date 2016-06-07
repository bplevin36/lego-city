#include "MatrixTransform.h"
#include <glm/gtx/string_cast.hpp>


MatrixTransform::MatrixTransform()
{
	M = glm::mat4(1.0f);
}

MatrixTransform::MatrixTransform(glm::mat4 initial) {
	M = initial;
}

void MatrixTransform::update(glm::mat4 C) {
	//update all children as well
	for (std::list<Node*>::iterator it = children.begin(); it != children.end(); ++it) {
		(*it)->update(C * M);
	}
}

void MatrixTransform::updateTransform(glm::mat4 update) {
	M = update * M;
}