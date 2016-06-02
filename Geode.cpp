#include "Geode.h"

Geode::Geode()
{

}

Geode::Geode(OBJObject *obj)
{
	this->obj = obj;
}

Geode::~Geode()
{
}

void Geode::draw(GLuint shaderProgram)
{
	obj->setToWorld(this->toWorld);
	obj->draw(shaderProgram);
}

void Geode::update(glm::mat4 C)
{
	this->toWorld = C;
}
