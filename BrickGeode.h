#pragma once
#include "Geode.h"
#include <stdlib.h>
#include <time.h>
#include <vector>

struct material {
	glm::vec3 ambient, diffuse, specular;
	float shininess;

	material(glm::vec3 a, glm::vec3 d, glm::vec3 s, float shine) :
		ambient(a),
		diffuse(d),
		specular(s),
		shininess(shine) {}

	material() :
		ambient(glm::vec3(0.0)),
		diffuse(glm::vec3(0.0)),
		specular(glm::vec3(0.0)),
		shininess(32.0) {}
};

class BrickGeode : public Geode
{
public:
	BrickGeode(OBJObject *obj, int colorindex);
	void draw(GLuint shaderProgram);

	static std::vector<material> materials;
	static int NUM_MATS;

private:
	material mat;
};

