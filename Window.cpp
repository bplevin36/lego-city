#include "window.h"
#include "Cylinder.h"
#include "Pod.h"
#include "MatrixTransform.h"
#include "Camera.h"
#include "Curve.h"

const char* window_title = "GLFW Starter Project";
const char* cylinder_filepath = "cylinder.obj";
const char* pod_filepath = "pod.obj";
const char* bear_filepath = "bear.obj";
OBJObject* bunny;
OBJObject* dragon;
OBJObject* bear;
OBJObject* active = NULL;
Group* root;
Pod* chair;
Skybox* skybox;
float pointSize = 1.0f;
float spotExp = 1.0f;
float spotWidth = 12.5f;
int currLight = 0;

int mouseButton = -1; //current mouse state
glm::vec2 lastMouse;
bool lastInitialized = false;

GLint skyShader;
GLuint shaderProgram;
GLuint curveShader;

// Default camera parameters
glm::vec3 cam_pos(0.0f, 0.0f, 20.0f);		// e  | Position of camera
glm::vec3 cam_look_at(0.0f, 0.0f, 0.0f);	// d  | This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);			// up | What orientation "up" is
//Lighting parameters
glm::vec4 dirLightDir(-0.2f, -1.0f, -0.3f, 0.0f);
glm::vec4 ptLightPos(0.0, 10.0, 1.0, 0.0);
glm::vec4 ptLightNorm = glm::normalize(ptLightPos);
glm::vec4 spotPos(0.0f, 2.0f, 10.0f, 0.0f);
glm::vec4 spotNorm = glm::normalize(spotPos);
int Window::width;
int Window::height;
glm::mat4 Window::P;
glm::mat4 Window::V;

Curve c1;



void Window::initialize_objects()
{
	std::vector<const GLchar*> faces = { "right.ppm","left.ppm","top.ppm","base.ppm","front.ppm","back.ppm" };
	skybox = new Skybox(faces);
	// Load the shader program. Similar to the .obj objects, different platforms expect a different directory for files
#ifdef _WIN32 // Windows (both 32 and 64 bit versions)
	shaderProgram = LoadShaders("../shader.vert", "../shader.frag");
#else // Not windows
	shaderProgram = LoadShaders("shader.vert", "shader.frag");
#endif

#ifdef _WIN32 // Windows (both 32 and 64 bit versions)
	skyShader = LoadShaders("../skybox.vert", "../skybox.frag");
#else // Not windows
	skyShader = LoadShaders("skybox.vert", "skybox.frag");
#endif

#ifdef _WIN32 // Windows (both 32 and 64 bit versions)
	curveShader = LoadShaders("../curve.vert", "../curve.frag");
#else // Not windows
	curveShader = LoadShaders("curve.vert", "curve.frag");
#endif
	glm::mat4 m1 = glm::mat4(
		glm::vec4(1.0, 1.0, 1.0, 1.0),
		glm::vec4(-1.0, 1.0, 1.0, 1.0),
		glm::vec4(-1.0, -1.0, 1.0, 1.0),
		glm::vec4(1.0, -1.0, 1.0, 1.0)
	);
	c1 = Curve(m1);

}

void Window::clean_up()
{
	glDeleteProgram(shaderProgram);
	glDeleteProgram(curveShader);
	glDeleteProgram(skyShader);
}

GLFWwindow* Window::create_window(int width, int height)
{
	// Initialize GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return NULL;
	}

	// 4x antialiasing
	glfwWindowHint(GLFW_SAMPLES, 4);

	// Create the GLFW window
	GLFWwindow* window = glfwCreateWindow(width, height, window_title, NULL, NULL);

	// Check if the window could not be created
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window
	glfwMakeContextCurrent(window);

	// Set swap interval to 1
	glfwSwapInterval(1);

	// Get the width and height of the framebuffer to properly resize the window
	glfwGetFramebufferSize(window, &width, &height);
	// Call the resize callback to make sure things get drawn immediately
	Window::resize_callback(window, width, height);

	return window;
}

void Window::resize_callback(GLFWwindow* window, int width, int height)
{
	Window::width = width;
	Window::height = height;
	// Set the viewport size
	glViewport(0, 0, width, height);

	if (height > 0)
	{
		P = glm::perspective(45.0f, (float)width / (float)height, 1.0f, 1000.0f);
		V = glm::lookAt(cam_pos, cam_look_at, cam_up);
	}
}

void Window::idle_callback()
{
	
}

void Window::display_callback(GLFWwindow* window)
{
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Draw skybox
	glUseProgram(skyShader);
	skybox->draw(skyShader);

	//TODO draw curves
	glUseProgram(curveShader);
	c1.draw(curveShader);

	// Use the shader of programID
	glUseProgram(shaderProgram);	
	//Initialize lighting
	glClearColor(0.75f, 0.52f, 0.3f, 1.0f);
		glm::vec3 pointLightColors[] = {
		glm::vec3(1.0f, 0.6f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 1.0, 0.0),
		glm::vec3(0.2f, 0.2f, 1.0f)
	};

	// Set material
	glUniform3f(glGetUniformLocation(shaderProgram, "material.ambient"), 0.0f, 0.0f, 0.0f);
	glUniform3f(glGetUniformLocation(shaderProgram, "material.diffuse"), 0.5f, 0.25f, 0.5f);
	glUniform3f(glGetUniformLocation(shaderProgram, "material.specular"), 0.45f, 0.55f, 0.45f);
	glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), 32.0);
	// Directional light
	glUniform3f(glGetUniformLocation(shaderProgram, "dirLight.direction"), dirLightDir.x, dirLightDir.y, dirLightDir.z);
	glUniform3f(glGetUniformLocation(shaderProgram, "dirLight.ambient"), 0.3f, 0.24f, 0.14f);
	glUniform3f(glGetUniformLocation(shaderProgram, "dirLight.diffuse"), 0.7f, 0.42f, 0.26f);
	glUniform3f(glGetUniformLocation(shaderProgram, "dirLight.specular"), 0.5f, 0.5f, 0.5f);
	
	// Point light 1
	glUniform3f(glGetUniformLocation(shaderProgram, "pointLights[0].position"), ptLightPos.x, ptLightPos.y, ptLightPos.z);
	glUniform3f(glGetUniformLocation(shaderProgram, "pointLights[0].ambient"), pointLightColors[0].x * 0.1, pointLightColors[0].y * 0.1, pointLightColors[0].z * 0.1);
	glUniform3f(glGetUniformLocation(shaderProgram, "pointLights[0].diffuse"), pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);
	glUniform3f(glGetUniformLocation(shaderProgram, "pointLights[0].specular"), pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);
	glUniform1f(glGetUniformLocation(shaderProgram, "pointLights[0].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(shaderProgram, "pointLights[0].linear"), 0.09);
	glUniform1f(glGetUniformLocation(shaderProgram, "pointLights[0].quadratic"), 0.032);
	
	// SpotLight
	glUniform3f(glGetUniformLocation(shaderProgram, "spotlight.position"), spotPos.x, spotPos.y, spotPos.z);
	glUniform3f(glGetUniformLocation(shaderProgram, "spotlight.direction"), -spotNorm.x, -spotNorm.y, -spotNorm.z);
	glUniform3f(glGetUniformLocation(shaderProgram, "spotlight.ambient"), 0.5f, 0.3f, 0.3f);
	glUniform3f(glGetUniformLocation(shaderProgram, "spotlight.diffuse"), 0.8f, 0.8f, 0.0f);
	glUniform3f(glGetUniformLocation(shaderProgram, "spotlight.specular"), 0.8f, 0.8f, 0.0f);
	glUniform1f(glGetUniformLocation(shaderProgram, "spotlight.constant"), 1.0f);
	glUniform1f(glGetUniformLocation(shaderProgram, "spotlight.linear"), 0.09);
	glUniform1f(glGetUniformLocation(shaderProgram, "spotlight.quadratic"), 0.032);
	glUniform1f(glGetUniformLocation(shaderProgram, "spotlight.exponent"), spotExp);
	glUniform1f(glGetUniformLocation(shaderProgram, "spotlight.cutOff"), glm::cos(glm::radians(spotWidth)));
	

	//Assign uniforms
	GLuint view_pos = glGetUniformLocation(shaderProgram, "viewPos");
	glUniform3f(view_pos, cam_pos.x, cam_pos.y, cam_pos.z);

	// Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();
	// Swap buffers
	glfwSwapBuffers(window);
}


glm::vec3 trackBallMap(double xpos, double ypos) {
	glm::vec3 v;    // Vector v is the synthesized 3D position of the mouse location on the trackball
	float d;     // this is the depth of the mouse location: the delta between the plane through the center of the trackball and the z position of the mouse
	v.x = (2.0*xpos - Window::width) / Window::width;   // this calculates the mouse X position in trackball coordinates, which range from -1 to +1
	v.y = (Window::height - 2.0*ypos) / Window::height;   // this does the equivalent to the above for the mouse Y position
	v.z = 0.0;   // initially the mouse z position is set to zero, but this will change below
	d = glm::length(v);    // this is the distance from the trackball's origin to the mouse location, without considering depth (=in the plane of the trackball's origin)
	d = (d<1.0) ? d : 1.0;   // this limits d to values of 1.0 or less to avoid square roots of negative values in the following line
	v.z = sqrtf(1.001 - d*d);  // this calculates the Z coordinate of the mouse position on the trackball, based on Pythagoras: v.z*v.z + d*d = 1*1
	v = glm::normalize(v); // Still need to normalize, since we only capped d, not v.
	return v;  // return the mouse location on the surface of the trackball
}

void Window::cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {
	double deltaX = 0.0;
	double deltaY = 0.0;
	
	if (lastInitialized) {
		deltaX = xpos - lastMouse.x;
		deltaY = ypos - lastMouse.y;
	}

		if (mouseButton == GLFW_MOUSE_BUTTON_RIGHT && active != NULL){
			if (currLight == 0) {
				active->translate(glm::vec3(deltaX*0.035, (0 - deltaY)*0.035, 0.0));
			}
			else if (currLight == 3) {//spotlight width adjustment
				spotWidth += (deltaY * 0.2);
			}
		}
		else if (mouseButton == GLFW_MOUSE_BUTTON_LEFT) {
			glm::vec3 oldPoint = trackBallMap(lastMouse.x, lastMouse.y);
			glm::vec3 newPoint = trackBallMap(xpos, ypos);
			glm::vec3 cross = glm::cross(oldPoint, newPoint);

			float angle = 40.0 * std::acos(glm::dot(oldPoint, newPoint) / (glm::length(newPoint) * glm::length(oldPoint)));
			//std::cout << "Trying to turn " << angle << " degrees" << std::endl;
			glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), angle / 180.0f * glm::pi<float>(), cross);

			cam_pos = glm::vec3(rotate * glm::vec4(cam_pos, 0.0));
			V = glm::lookAt(cam_pos, cam_look_at, cam_up);
			/*
			if (currLight == 0 && active!=NULL) {
				active->orbit(angle, cross);
			}else if (currLight == 1){
				glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), angle / 180.0f * glm::pi<float>(), cross);
				dirLightDir = rotate * dirLightDir;
				glUniform3f(glGetUniformLocation(shaderProgram, "dirLight.direction"), dirLightDir.x, dirLightDir.y, dirLightDir.z);
			} else if (currLight == 2){
				glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), angle / 180.0f * glm::pi<float>(), cross);
				ptLightPos = rotate * ptLightPos;
				ptLightNorm = glm::normalize(ptLightPos);
				glUniform3f(glGetUniformLocation(shaderProgram, "pointLights[0].position"), ptLightPos.x, ptLightPos.y, ptLightPos.z);
			}
			else if (currLight == 3) {
				glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), angle / 180.0f * glm::pi<float>(), cross);
				spotPos = rotate * spotPos;
				//printf("Spotlight direction (%f, %f, %f)\n", -spotNorm.x, -spotNorm.y, -spotNorm.z);
				spotNorm = glm::normalize(spotPos);
				glUniform3f(glGetUniformLocation(shaderProgram, "spotlight.position"), spotPos.x, spotPos.y, spotPos.z);
				glUniform3f(glGetUniformLocation(shaderProgram, "spotlight.direction"), -spotNorm.x, -spotNorm.y, -spotNorm.z);
			}*/
		}
	
	lastMouse.x = xpos;
	lastMouse.y = ypos;
	lastInitialized = true;
}

void Window::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
			mouseButton = GLFW_MOUSE_BUTTON_LEFT;
		}
		else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
			mouseButton = GLFW_MOUSE_BUTTON_RIGHT;
		}
		else {
			mouseButton = -1;
		}
}

void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	glm::vec3 d = glm::vec3(0.0);
	d.z = yoffset;
	glm::vec3 toOrigin = cam_pos - cam_look_at;
	toOrigin = (toOrigin * (GLfloat)yoffset) * 0.1f;
	cam_pos = glm::vec3(glm::translate(glm::mat4(1.0f), toOrigin) * glm::vec4(cam_pos, 1.0));
	V = glm::lookAt(cam_pos, cam_look_at, cam_up);
	/*
	if (active != NULL && currLight == 0) {
		active->translate(d);
	}
	else if (currLight == 2) {
		ptLightPos = ptLightPos - (ptLightNorm * float(yoffset));
	}
	else if (currLight == 3) {
		spotPos = spotPos - (spotNorm * float(yoffset));
	}*/
}

void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Check for a key press
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_F1) {
			//material for silver bunny
			glUniform3f(glGetUniformLocation(shaderProgram, "material.ambient"), 0.19225, 0.019225, 0.019225);
			glUniform3f(glGetUniformLocation(shaderProgram, "material.diffuse"), 0.50754, 0.050754, 0.050754);
			glUniform3f(glGetUniformLocation(shaderProgram, "material.specular"), 0.208273, 0.0508273, 0.308273);
			glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), 5.0);
			active = bunny;
		}
		else if (key == GLFW_KEY_F2) {
			//material properties for golden dragon
			glUniform3f(glGetUniformLocation(shaderProgram, "material.ambient"), 0.24725, 0.1995, 0.0745);
			glUniform3f(glGetUniformLocation(shaderProgram, "material.diffuse"), 0.75164, 0.60648, 0.22648);
			glUniform3f(glGetUniformLocation(shaderProgram, "material.specular"), 0.628281, 0.555802, 0.366065);
			glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), 51.2);
			active = dragon;
		}
		else  if (key == GLFW_KEY_F3) {
			//material for rubber bear
			glUniform3f(glGetUniformLocation(shaderProgram, "material.ambient"), 0.05, 0.05, 0.05);
			glUniform3f(glGetUniformLocation(shaderProgram, "material.diffuse"), 0.05, 0.05, 0.05);
			glUniform3f(glGetUniformLocation(shaderProgram, "material.specular"), 0.7, 0.7, 0.7);
			glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), 10.0);
			active = bear;
		}
		else if (key == GLFW_KEY_0) {
			currLight = 0;
		}
		else if (key == GLFW_KEY_1) {
			currLight = 1;
		}
		else if (key == GLFW_KEY_2) {
			currLight = 2;
		}
		else if (key == GLFW_KEY_3) {
			currLight = 3;
		}
		else if (key == GLFW_KEY_E) {
			if (mods & GLFW_MOD_SHIFT) {//capital E
				spotExp *= 2.0;
			}
			else {
				spotExp /= 2.0;
			}
		}
		else if (key == GLFW_KEY_P) {
			if (mods & GLFW_MOD_SHIFT) {//capital P
				++pointSize;
			}
			else {//lowercase
				if (pointSize > 1.0f) {
					--pointSize;
				}
			}
			glPointSize(pointSize);
		}
		else if (key == GLFW_KEY_X) {
			if (mods & GLFW_MOD_SHIFT) {
				active->translate(glm::vec3(1.0f, 0.0f, 0.0f));
			}
			else {
				active->translate(glm::vec3(-1.0f, 0.0f, 0.0f));
			}
		}
		else if (key == GLFW_KEY_Y) {
			if (mods & GLFW_MOD_SHIFT) {
				active->translate(glm::vec3(0.0f, 1.0f, 0.0f));
			}
			else {
				active->translate(glm::vec3(0.0f, -1.0f, 0.0f));
			}
		}
		else if (key == GLFW_KEY_Z) {
			if (mods & GLFW_MOD_SHIFT) {
				active->translate(glm::vec3(0.0f, 0.0f, 1.0f));
			}
			else {
				active->translate(glm::vec3(0.0f, 0.0f, -1.0f));
			}
		}
		else if (key == GLFW_KEY_S && active != NULL) {
			if (mods & GLFW_MOD_SHIFT) {
				active->scale(1.0f);
			}
			else {
				active->scale(-1.0f);
			}
		}
		else if (key == GLFW_KEY_R) {
			pointSize = 1.0;
			spotExp = 1.0f;
			spotWidth = 12.5f;
			active->reset();
		}
		// Check if escape was pressed
		if (key == GLFW_KEY_ESCAPE)
		{
			// Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
	}
}