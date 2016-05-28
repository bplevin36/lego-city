#include <stdlib.h>
#include <stdio.h>
#include <math.h>   // include math functions, such as sin, cos, M_PI
#include <iostream> // allow c++ style console printouts
#include "Skybox.h"
#include "Window.h"

using namespace std; // allow console printouts without std::


Skybox::Skybox(vector<const GLchar*> faces) {
	GLfloat sky_vertices[] = {
		// Front vertices
		-500.0, -500.0,  500.0,
		500.0, -500.0,  500.0,
		500.0,  500.0,  500.0,
		-500.0,  500.0,  500.0,
		// Back vertices
		-500.0, -500.0, -500.0,
		500.0, -500.0, -500.0,
		500.0,  500.0, -500.0,
		-500.0,  500.0, -500.0
	};
	GLuint sky_indices[] = {  // Note that we start from 0!
							  // Front face
		0, 1, 2,
		2, 3, 0,
		// Top face
		1, 5, 6,
		6, 2, 1,
		// Back face
		7, 6, 5,
		5, 4, 7,
		// Bottom face
		4, 0, 3,
		3, 7, 4,
		// Left face
		4, 5, 1,
		1, 0, 4,
		// Right face
		3, 2, 6,
		6, 7, 3
	};

	// Create buffers/arrays
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sky_vertices), sky_vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sky_indices), sky_indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
		GL_FLOAT, // What type these components are
		GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
		3 * sizeof(GLfloat), // Offset between consecutive vertex attributes. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
		(GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.

	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

	glBindVertexArray(0); // Unbind VAO

	cubemapTexture = loadCubemap(faces);
}


/** Load a ppm file from disk.
@input filename The location of the PPM file.  If the file is not found, an error message
will be printed and this function will return 0
@input width This will be modified to contain the width of the loaded image, or 0 if file not found
@input height This will be modified to contain the height of the loaded image, or 0 if file not found
@return Returns the RGB pixel data as interleaved unsigned chars (R0 G0 B0 R1 G1 B1 R2 G2 B2 .... etc) or 0 if an error ocured
**/
unsigned char* loadPPM(const char* filename, int& width, int& height)
{
	const int BUFSIZE = 128;
	FILE* fp;
	unsigned int read;
	unsigned char* rawData;
	char buf[3][BUFSIZE];
	char* retval_fgets;
	size_t retval_sscanf;

	if ((fp = fopen(filename, "rb")) == NULL)
	{
		std::cerr << "error reading ppm file, could not locate " << filename << std::endl;
		width = 0;
		height = 0;
		return NULL;
	}

	// Read magic number:
	retval_fgets = fgets(buf[0], BUFSIZE, fp);

	// Read width and height:
	do
	{
		retval_fgets = fgets(buf[0], BUFSIZE, fp);
	} while (buf[0][0] == '#');
	retval_sscanf = sscanf(buf[0], "%s %s", buf[1], buf[2]);
	width = atoi(buf[1]);
	height = atoi(buf[2]);

	// Read maxval:
	do
	{
		retval_fgets = fgets(buf[0], BUFSIZE, fp);
	} while (buf[0][0] == '#');

	// Read image data:
	rawData = new unsigned char[width * height * 3];
	read = fread(rawData, width * height * 3, 1, fp);
	fclose(fp);
	if (read != 1)
	{
		std::cerr << "error parsing ppm file, incomplete data" << std::endl;
		delete[] rawData;
		width = 0;
		height = 0;
		return NULL;
	}

	return rawData;
}

GLuint Skybox::loadCubemap(vector<const GLchar*> faces)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0);

	int width, height;
	unsigned char* image;

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	for (GLuint i = 0; i < faces.size(); i++)
	{
		image = loadPPM(faces[i], width, height);
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
			GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image
		);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return textureID;
}

GLuint Skybox::getCubemap() {
	return cubemapTexture;
}

void Skybox::draw(GLuint shaderProgram) {
	GLuint MatrixID = glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &Window::V[0][0]);
	MatrixID = glGetUniformLocation(shaderProgram, "projection");
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &Window::P[0][0]);

	glBindVertexArray(VAO);
	glUniform1i(glGetUniformLocation(shaderProgram, "skybox"), 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}