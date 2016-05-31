#include "OBJObject.h"
#include "Window.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <glm/gtx/string_cast.hpp>

#define DEBUG true

GLuint OBJObject::shaderProgram = 0;

OBJObject::OBJObject() {

}

OBJObject::OBJObject(const char *filepath)
{
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
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;
	bool ret = tinyobj::LoadObj(shapes, materials, err, filepath, (const char*)0, 3U);
	if (!err.empty()) { // `err` may contain warning message.
		std::cerr << err << std::endl;
	}
	// add vecs for all shapes
	for (int s = 0; s < shapes.size(); s++) {
		// add face indices
		indices = shapes[s].mesh.indices;
		// add vertices
		for (int i = 0; i < shapes[s].mesh.positions.size(); i += 3) {
			float x = shapes[s].mesh.positions[i];
			float y = shapes[s].mesh.positions[i + 1];
			float z = shapes[s].mesh.positions[i + 2];
			if (x > max.x) {
				max.x = x;
			}
			else if (x < min.x) {
				min.x = x;
			}
			if (y > max.y) {
				max.y = y;
			}
			else if (y < min.y) {
				min.y = y;
			}
			if (z > max.z) {
				max.z = z;
			}
			else if (z < min.z) {
				min.z = z;
			}
			vertices.push_back(glm::vec3(x, y, z));
		}
		// add normals
		for (int i = 0; i < shapes[s].mesh.normals.size(); i += 3) {
			glm::vec3 normal = glm::vec3(shapes[s].mesh.normals[i], shapes[s].mesh.normals[i + 1], shapes[s].mesh.normals[i + 2]);
			normals.push_back(normal);
		}
	}
	if (DEBUG) {
		std::cout << "# of shapes    : " << shapes.size() << std::endl;
		std::cout << "# of materials : " << materials.size() << std::endl;

		for (size_t i = 0; i < shapes.size(); i++) {
			printf("shape[%ld].name = %s\n", i, shapes[i].name.c_str());
			printf("Size of shape[%ld].indices: %ld\n", i, shapes[i].mesh.indices.size());
			printf("Size of shape[%ld].material_ids: %ld\n", i, shapes[i].mesh.material_ids.size());
			assert((shapes[i].mesh.indices.size() % 3) == 0);
			/* Probably don't want to print ALL indices
			for (size_t f = 0; f < shapes[i].mesh.indices.size() / 3; f++) {
				printf("  idx[%ld] = %d, %d, %d. mat_id = %d\n", f, shapes[i].mesh.indices[3 * f + 0], shapes[i].mesh.indices[3 * f + 1], shapes[i].mesh.indices[3 * f + 2], shapes[i].mesh.material_ids[f]);
			} */

			printf("shape[%ld].vertices: %ld\n", i, shapes[i].mesh.positions.size());
			assert((shapes[i].mesh.positions.size() % 3) == 0);
			/* Probably don't want to print ALL normals
			for (size_t v = 0; v < shapes[i].mesh.normals.size() / 3; v++) {
				printf("  norm[%ld] = (%f, %f, %f)\n", v,
					shapes[i].mesh.normals[3 * v + 0],
					shapes[i].mesh.normals[3 * v + 1],
					shapes[i].mesh.normals[3 * v + 2]);
			}
			*/
		}

		for (size_t i = 0; i < materials.size(); i++) {
			printf("material[%ld].name = %s\n", i, materials[i].name.c_str());
			printf("  material.Ka = (%f, %f ,%f)\n", materials[i].ambient[0], materials[i].ambient[1], materials[i].ambient[2]);
			printf("  material.Kd = (%f, %f ,%f)\n", materials[i].diffuse[0], materials[i].diffuse[1], materials[i].diffuse[2]);
			printf("  material.Ks = (%f, %f ,%f)\n", materials[i].specular[0], materials[i].specular[1], materials[i].specular[2]);
			printf("  material.Tr = (%f, %f ,%f)\n", materials[i].transmittance[0], materials[i].transmittance[1], materials[i].transmittance[2]);
			printf("  material.Ke = (%f, %f ,%f)\n", materials[i].emission[0], materials[i].emission[1], materials[i].emission[2]);
			printf("  material.Ns = %f\n", materials[i].shininess);
			printf("  material.Ni = %f\n", materials[i].ior);
			printf("  material.dissolve = %f\n", materials[i].dissolve);
			printf("  material.illum = %d\n", materials[i].illum);
			printf("  material.map_Ka = %s\n", materials[i].ambient_texname.c_str());
			printf("  material.map_Kd = %s\n", materials[i].diffuse_texname.c_str());
			printf("  material.map_Ks = %s\n", materials[i].specular_texname.c_str());
			printf("  material.map_Ns = %s\n", materials[i].specular_highlight_texname.c_str());
			std::map<std::string, std::string>::const_iterator it(materials[i].unknown_parameter.begin());
			std::map<std::string, std::string>::const_iterator itEnd(materials[i].unknown_parameter.end());
			for (; it != itEnd; it++) {
				printf("  material.%s = %s\n", it->first.c_str(), it->second.c_str());
			}
			printf("\n");
		}
		std::cout << "Max: " << glm::to_string(max) << std::endl;
		std::cout << "Min: " << glm::to_string(min) << std::endl;
	}
	/* Old parsing code
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
	*/
	//calculate center and recenter model
	glm::vec3 center = min; //for bricks, center them at the minimum corner
	if (DEBUG)
		std::cout << "Old center: " << glm::to_string(center) << std::endl;
	int i;
	for (i = 0; i < vertices.size(); i++) {
		vertices[i] = vertices[i] - center;
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
	maxZ = -INFINITY;
	float sumZ = 0;
	for (i = 0; i < vertices.size(); i++) {
		if (vertices[i].z > maxZ) {
			maxZ = vertices[i].z;
		}
		sumZ += vertices[i].z;
	}
	if (DEBUG) {
		std::cout << "Total vertices: " << vertices.size() << std::endl;
		std::cout << "Number of normals: " << normals.size() << std::endl;
		//std::cout << "First face: " << this->indices[0] << ", " << this->indices[1] << ", " << this->indices[2] << std::endl;
		//std::cout << "First normal: (" << normals[0].x << ", " << normals[0].y << ", " << normals[0].z << ")" << std::endl;
		std::cout << "Number of faces: " << (this->indices.size() + 1) / 3 << std::endl;
		std::cout << "MaxZ: " << maxZ << std::endl;
		std::cout << "AvgZ: " << sumZ/vertices.size() << std::endl;
	}
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