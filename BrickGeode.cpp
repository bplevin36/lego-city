#include "BrickGeode.h"

std::vector<material> BrickGeode::materials = std::vector<material>({
	// Red plastic
	material(	glm::vec3(0.0, 0.0, 0.0),
				glm::vec3(0.5, 0.0, 0.0),
				glm::vec3(0.7, 0.6, 0.6),
				32.0),

	// Yellow Plastic
	material(	glm::vec3(0.0, 0.0, 0.0),
				glm::vec3(0.5, 0.5, 0.0),
				glm::vec3(0.6, 0.6, 0.5),
				32.0),

	// Green Plastic
	material(	glm::vec3(0.0, 0.0, 0.0),
				glm::vec3(0.1, 0.35, 0.1),
				glm::vec3(0.45, 0.55, 0.45),
				32.0),

	// Blue Plastic
	material(	glm::vec3(0.0, 0.0, 0.0),
				glm::vec3(0.0, 0.0, 0.5),
				glm::vec3(0.6, 0.6, 0.7),
				32.0),

	// White Plastic
	material(glm::vec3(0.0, 0.0, 0.0),
		glm::vec3(0.55, 0.55, 0.55),
		glm::vec3(0.7, 0.7, 0.7),
		32.0)
});

int BrickGeode::NUM_MATS = materials.size();

BrickGeode::BrickGeode(OBJObject *obj, int colorindex)
{
	this->obj = obj;
	this->mat = BrickGeode::materials[colorindex];
}

void BrickGeode::draw(GLuint shaderProgram)
{
	// Set material of brick
	glUniform3f(glGetUniformLocation(shaderProgram, "material.ambient"), mat.ambient.x, mat.ambient.y, mat.ambient.z);
	glUniform3f(glGetUniformLocation(shaderProgram, "material.diffuse"), mat.diffuse.x, mat.diffuse.y, mat.diffuse.z);
	glUniform3f(glGetUniformLocation(shaderProgram, "material.specular"), mat.specular.x, mat.specular.y, mat.specular.z);
	glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), mat.shininess);

	obj->setToWorld(this->toWorld);
	obj->draw(shaderProgram);
}