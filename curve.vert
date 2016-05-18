#version 330 core

// This is an example vertex shader. GLSL is very similar to C.
// You can define extra functions if needed, and the main() function is
// called when the vertex shader gets run.
// The vertex shader gets called once per vertex.

layout (location = 0) in vec3 position;

out vec3 FragPos;

uniform mat4 MVP;
uniform mat4 model;

void main()
{
    gl_Position = MVP * vec4(position.x, position.y, position.z, 1.0);
	FragPos = vec3(model * vec4(position, 1.0f));
}
