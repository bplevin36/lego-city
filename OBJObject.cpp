#include "OBJObject.h"
#include "Window.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>

GLuint OBJObject::shaderProgram = 0;

OBJObject::OBJObject() {

}

OBJObject::OBJObject(const char *filepath, GLuint environment)
{
	box = environment;
	parse(filepath);
	this->setupBuffers();
}

void OBJObject::setupBuffers(){
	if (vertices.size() <= 0) {
		return;//skip setup if data not initialized already
	}
	toWorld = glm::mat4(1.0f);
	this->angleX = 0.0f;
	this->angleY = 0.0f;

	// Create buffers/arrays
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &VBON);
	glGenBuffers(1, &EBO);

	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
		GL_FLOAT, // What type these components are
		GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
		3 * sizeof(GLfloat), // Offset between consecutive vertex attributes. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
		(GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.

	glBindBuffer(GL_ARRAY_BUFFER, VBON);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
		GL_FLOAT, // What type these components are
		GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
		sizeof(glm::vec3), // Offset between consecutive vertex attributes. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
		(GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
}

glm::vec3 OBJObject::getNormal(int index) {
	return normals[index];
}

glm::mat4 OBJObject::getToWorld() {
	return toWorld;
}

void OBJObject::parse(const char *filepath) 
{
	//TODO parse the OBJ file
	// Populate the face indices, vertices, and normals vectors with the OBJ Object data

	float x, y, z;  // vertex coordinates
	float r, g, b;  // vertex color
	int v1, v2, v3; // face vertex indices
	int c1, c2;    // characters read from file
	char line[300]; //whole line
	int count = 0;

	std::ifstream fp (filepath, std::ifstream::in);  // make the file name configurable so you can load other files
	
	if (!fp.is_open()) { std::cerr << "error loading file" << std::endl; exit(-1); }  // just in case the file can't be found or is corrupt

	std::cout << "Parsing: " << filepath << std::endl;
	c1 = fp.get();
	c2 = fp.get();

	while (c1 != EOF) {

		if ((c1 == 'v') && (c2 == ' '))
		{
			fp >> x >> y >> z;
			if (fp.peek() != '\n') {
				fp >> r >> g >> b;
			}
			else {
				//fp.ignore(10, '\n');
			}
			if (x > max.x) {
				max.x = x;
			} else if(x < min.x){
				min.x = x;
			}
			if (y > max.y) {
				max.y = y;
			}else if (y < min.y){
				min.y = y;
			}
			if (z > max.z) {
				max.z = z;
			}else if(z < min.z){
				min.z = z;
			}
			this->vertices.push_back(glm::vec3(x, y, z));

		}
		else if ((c1 == 'v') && (c2 == 'n')) { //parse a normal vector

			fp >> x >> y >> z;
			this->normals.push_back(glm::vec3(x, y, z));
		}
		else if ((c1 == 'f') && (c2 == ' ')) { //parse a face
			int junk;
			fp >> v1;
			fp.ignore(2);
			fp >> junk;

			fp >> v2;
			fp.ignore(2);
			fp >> junk;

			fp >> v3;
			fp.ignore(2);
			fp >> junk;

			this->indices.push_back(v1-1);
			this->indices.push_back(v2-1);
			this->indices.push_back(v3-1);
		}
	    //consume rest of line
		fp.ignore(300, '\n');

		c1 = fp.get();
		c2 = fp.get();
	}

	//calculate center and recenter model
	glm::vec3 center = (max + min) / 2.0f;
	int i;
	for (i = 0; i < vertices.size(); i++) {
		vertices[i] = vertices[i] - center;
		vertices[i].y += 0.5;
	}
	//calculate scaling factor and rescale
	min = glm::vec3(INFINITY);
	max = glm::vec3(-INFINITY);
	for (i = 0; i < vertices.size(); i++) {
		if (vertices[i].x < min.x) {
			min.x = vertices[i].x;
		}
		else if (vertices[i].x > max.x) {
			max.x = vertices[i].x;
		}
		if (vertices[i].y < min.y) {
			min.y = vertices[i].y;
		}
		else if (vertices[i].y > max.y) {
			max.y = vertices[i].y;
		}
		if (vertices[i].z < min.z) {
			min.z = vertices[i].z;
		}
		else if (vertices[i].z > max.z) {
			max.z = vertices[i].z;
		}
	}
	float maxX = std::max(max.x, std::abs(min.x));
	float maxY = std::max(max.y, std::abs(min.y));
	float maxZ = std::max(max.z, std::abs(min.z));
	float maxDim = std::max(maxX, std::max(maxY, maxZ));
	if (maxDim > 1) {
		for (i = 0; i < vertices.size(); i++) {
			vertices[i] = vertices[i] / maxDim;
		}
	}


	std::cout << "Total vertices: " << vertices.size() << std::endl;
	std::cout<<"Number of normals: "<<normals.size() << std::endl;
	//std::cout << "First face: " << this->indices[0] << ", " << this->indices[1] << ", " << this->indices[2] << std::endl;
	std::cout << "First normal: (" << normals[0].x << ", " << normals[0].y << ", " << normals[0].z << ")" << std::endl;
	std::cout << "Number of faces: " << (this->indices.size()+1)/3 << std::endl;
	fp.close();   // make sure you don't forget to close the file when done
}

void OBJObject::draw(GLuint shaderProgram) 
{
	// Calculate combination of the model (toWorld), view (camera inverse), and perspective matrices
	glm::mat4 MVP = Window::P * Window::V * toWorld;
	// We need to calculate this because as of GLSL version 1.40 (OpenGL 3.1, released March 2009), gl_ModelViewProjectionMatrix has been
	// removed from the language. The user is expected to supply this matrix to the shader when using modern OpenGL.
	GLuint MatrixID = glGetUniformLocation(shaderProgram, "MVP");
	GLuint modelID = glGetUniformLocation(shaderProgram, "model");
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(modelID, 1, GL_FALSE, &toWorld[0][0]);
	
	glUniform1i(glGetUniformLocation(shaderProgram, "skybox"), 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, box);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

}

void OBJObject::update(glm::mat4 C) {
	this->toWorld = C;
}

void OBJObject::update()
{
	//spin(1.0f);
	this->toWorld = this->translation;
	this->toWorld = this->toWorld * this->rotateX;
	this->toWorld = this->toWorld * this->rotateY;
	this->toWorld = this->toWorld * this->scaling;
	this->toWorld = this->orbiting * this->toWorld;
}

void OBJObject::setToWorld(glm::mat4 newWorld) {
	toWorld = newWorld;
}

glm::vec3 OBJObject::getPos() {
	return position;
}

void OBJObject::reset() {
	this->translation = glm::mat4(1.0f);
	this->rotateX = glm::mat4(1.0f);
	this->rotateY = glm::mat4(1.0f);
	this->orbiting = glm::mat4(1.0f);
	this->scaling = glm::mat4(1.0f);
	this->scaleFactor = 1.0f;
	this->orbitAngle = 0.0f;
	this->position = glm::vec3(0.0f);
}

void OBJObject::setAngleX(float deg)
{
	this->angleX = deg;
	// This creates the matrix to rotate the cube
	this->rotateX = glm::rotate(glm::mat4(1.0f), this->angleX / 180.0f * glm::pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f));
}

void OBJObject::setAngleY(float deg)
{
	this->angleY = deg;
	// This creates the matrix to rotate the cube
	this->rotateY = glm::rotate(glm::mat4(1.0f), this->angleY / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
}


void OBJObject::orbit(float deg, glm::vec3 axis) {
	this->orbiting = glm::rotate(glm::mat4(1.0f), deg / 180.0f * glm::pi<float>(), axis) * this->orbiting;
}

void OBJObject::translate(glm::vec3 vector) {
	position = position + vector;
	this->translation = glm::translate(glm::mat4(1.0f), position);
}

void OBJObject::scale(float factor) {
	this->scaleFactor += factor;
	this->scaling = glm::scale(glm::mat4(1.0f), glm::vec3(this->scaleFactor));
}

void OBJObject::print_matrix(glm::mat4 matrix) {

	int row;
	for (row = 0; row < 4; row++) {//each row
		std::cout << "| ";
		int col;
		for (col = 0; col < 4; col++) {//each column
			std::cout << ((matrix[col])[row]) << " ";
		}
		std::cout << "|" << std::endl;
	}

}