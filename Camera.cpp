#include "Camera.h"



Camera::Camera()
{
	toWorld = glm::mat4(1.0f);
}


Camera::~Camera()
{
}

glm::mat4 Camera::getToWorld()
{
	return toWorld;
}

void Camera::update(glm::mat4 changed)
{
	toWorld = changed;
}

void Camera::draw(GLuint)
{
}
