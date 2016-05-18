#include "Curve.h"
#include "Window.h"
#include <math.h>



Curve::Curve()
{
	p = glm::mat4(1.0f);
}

Curve::Curve(glm::mat4 pointMat)
{
	this->p = pointMat;
	reevaluate();
	this->setupBuffers();
}

Curve::Curve(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4)
{
	p = glm::mat4(glm::vec4(p1, 0.0), glm::vec4(p2, 0.0), glm::vec4(p3, 0.0), glm::vec4(p4,0.0));
	reevaluate();
	this->setupBuffers();
}


Curve::~Curve()
{
}

void Curve::reevaluate()
{
	points.resize(NUM_POINTS);
	int i;
	for (i = 0; i < NUM_POINTS; i++) {
		points[i] = (eval((float)(i) / (float)(NUM_POINTS)));
	}
}

void Curve::setupBuffers() {
	if (points.size() <= 0) {
		return;//skip setup if data not initialized already
	}
	toWorld = glm::mat4(1.0f);

	// Create buffers/arrays
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &PAO);
	glGenBuffers(1, &PBO);
	glGenBuffers(1, &EBO);

	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), points.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
		GL_FLOAT, // What type these components are
		GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
		3 * sizeof(GLfloat), // Offset between consecutive vertex attributes. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
		(GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.

	//Bind point array object and set buffers
	glBindVertexArray(PAO);
	//Buffer for control points
	glm::vec3 controls[] = { glm::vec3(p[0]), glm::vec3(p[1]), glm::vec3(p[2]), glm::vec3(p[3]) };
	glBindBuffer(GL_ARRAY_BUFFER, PBO);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec3), controls, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
		GL_FLOAT, // What type these components are
		GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
		3 * sizeof(GLfloat), // Offset between consecutive vertex attributes. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
		(GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.

	unsigned int indices[] = { 0,1,2,3 };
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
}

void Curve::draw(GLuint shaderProgram)
{
	glm::mat4 MVP = Window::P * Window::V * toWorld;
	// We need to calculate this because as of GLSL version 1.40 (OpenGL 3.1, released March 2009), gl_ModelViewProjectionMatrix has been
	// removed from the language. The user is expected to supply this matrix to the shader when using modern OpenGL.
	GLuint MatrixID = glGetUniformLocation(shaderProgram, "MVP");
	GLuint modelID = glGetUniformLocation(shaderProgram, "model");
	
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(modelID, 1, GL_FALSE, &toWorld[0][0]);
	glUniform4f(glGetUniformLocation(shaderProgram, "outputColor"), 0.0, 0.0, 0.0, 0.0);

	glBindVertexArray(VAO);
	glDrawArrays(GL_LINE_STRIP, 0, points.size());

	glUniform4f(glGetUniformLocation(shaderProgram, "outputColor"), 1.0, 0.0, 0.0, 0.0);
	glPointSize(8.0f);
	glBindVertexArray(PAO);
	glDrawArrays(GL_POINTS, 0, 4);
	glUniform4f(glGetUniformLocation(shaderProgram, "outputColor"), 1.0, 1.0, 0.1, 0.0);
	glDrawElements(GL_LINES, 4, GL_UNSIGNED_INT, 0);
}

void Curve::update(glm::mat4)
{

}

int Curve::factorial(int num) {
	if (num < 0)
		return -1;
	if (num == 0) {
		return 1;
	}
	return num * factorial(num - 1);
}

void Curve::setPoint(int index, glm::vec3 point)
{
	p = glm::column(p, index, glm::vec4(point, 0.0f));
}

glm::vec3 Curve::getPoint(int index)
{
	return glm::vec3(glm::column(p, index));
}


glm::vec3 Curve::eval(float t)
{
	glm::vec4 Cs = glm::vec4(C(0, t), C(1, t), C(2, t), C(3, t));
	return glm::vec3(p * Cs);
}

float Curve::C(int i, float t)
{

	return (6 / (factorial(3 - i)*factorial(i))) * pow(t, i) * pow((1 - t), (3 - i));
}
