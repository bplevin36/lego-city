#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Skybox.h"
#include "shader.h"
#include "OBJObject.h"
#include "Group.h"


class Window
{
public:
	static int width;
	static int height;
	static glm::mat4 P;
	static glm::mat4 V;
	static void initialize_objects();
	static void clean_up();
	static GLFWwindow* create_window(int width, int height);
	static void resize_callback(GLFWwindow* window, int width, int height);
	static void idle_callback();
	static void display_callback(GLFWwindow*);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void layout_roads();
	static void addStud(glm::ivec3, Group*);
	static void addStud(glm::ivec3, Group*, int);
	static void addStud(glm::ivec3, Group*, int, int);
	static void addBrick(glm::ivec3, glm::ivec2, Group*);
	static void addBrick(glm::ivec3, glm::ivec2, Group*, int);
};

#endif
