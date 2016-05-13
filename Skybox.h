#ifndef _SKYBOX_H_
#define _SKYBOX_H_

#include <GL/glew.h>
#include <vector>

class Skybox
{
public:
	Skybox(std::vector<const GLchar*>);
	//Skybox(char* filename);
	void draw(GLuint);
	~Skybox();
private:
	GLuint loadCubemap(std::vector<const GLchar*>);
	GLuint cubemapTexture, VAO, VBO, EBO;
};
#endif