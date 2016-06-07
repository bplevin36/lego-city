#include "window.h"
#include "Cylinder.h"
#include "Pod.h"
#include "MatrixTransform.h"
#include "Camera.h"
#include "Curve.h"
#include "Building.h"
#include "BrickGeode.h"
#include <cmath>
#include <random>
#include <time.h>
#include <chrono>
#include <stdlib.h>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/string_cast.hpp>

int MAP_SIZE = 100;
#define NO_STUD 0
#define ROAD_STUD 1
#define INTER_STUD 2
#define BUILDING_CORNER 3
#define MAJOR_ROAD_WIDTH 3
#define MED_ROAD_WIDTH 2
#define SM_ROAD_WIDTH 1
#define MAX_PARCEL 15

const glm::vec3 STUD_DIMS = glm::vec3(0.70, 0.85, 0.70);

const char* window_title = "GLFW Starter Project";

const char* lego_filepath = "lego1x1.obj";
OBJObject *brickObj;
Building *building1, *building2, *building3, *building4;
MatrixTransform *baseTransform, *b1Trans, *b2Trans, *b3Trans, *b4Trans;

Skybox* skybox;

std::vector<std::vector<int>>* roadMap = new std::vector<std::vector<int>>(MAP_SIZE, std::vector<int>(MAP_SIZE, NO_STUD));
std::vector<glm::ivec2> buildingCorners;
std::vector<Building*> buildings;
Group* roads;

float spotExp = 1.0f;
float spotWidth = 12.5f;

int mouseButton = -1; //current mouse state
glm::vec2 lastMouse;
bool lastInitialized = false;
bool demoing = false;

GLint skyShader;
GLuint shaderProgram;
GLuint curveShader;

// Default camera parameters
glm::vec3 cam_pos(STUD_DIMS.x* MAP_SIZE / 2, 45.0f, 100.0f);		// e  | Position of camera
glm::vec3 old_pos = cam_pos;
glm::vec3 cam_look_at(STUD_DIMS.x* MAP_SIZE/2, 0.0f, STUD_DIMS.y* MAP_SIZE/2);	// d  | This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);			// up | What orientation "up" is
glm::vec3 look_dir(0.0, 0.0, 1.0);

//Lighting parameters
glm::vec4 dirLightDir(-0.5f, -0.8f, -0.5f, 0.0f);
glm::vec4 ptLightPos(0.0, 10.0, 1.0, 0.0);
glm::vec4 ptLightNorm = glm::normalize(ptLightPos);
glm::vec4 spotPos(0.0f, 2.0f, 10.0f, 0.0f);
glm::vec4 spotNorm = glm::normalize(spotPos);
int Window::width;
int Window::height;
glm::mat4 Window::P;
glm::mat4 Window::V;

glm::ivec2 highwayStart, highwayEnd;
bool vertical = false;

std::default_random_engine generator;
std::uniform_real_distribution<float> distribution(0.0, 1.0);
std::chrono::high_resolution_clock::duration d = std::chrono::high_resolution_clock::now().time_since_epoch();

bool onMap(glm::ivec2 pt, std::vector<std::vector<int>>* map) {
	return pt.x < map->size() && pt.y < (*map)[0].size() && pt.x >= 0 && pt.y >= 0;
}

void road_between(glm::ivec2 p1, glm::ivec2 p2, int half_width, std::vector<std::vector<int>>* map) {
	if (p1.x == p2.x) {//vertical road
		for (int y = std::min(p1.y, p2.y); y < std::max(p1.y, p2.y); y++) {
			for (int x = p1.x - half_width; x < p1.x + half_width; x++) {
				(*map)[x][y] = ROAD_STUD;
			}
		}
	} else if (p1.y == p2.y){//horizontal road
		for (int x = std::min(p1.x, p2.x); x < std::max(p1.x, p2.x); x++) {
			for (int y = p1.y - half_width; y < p1.y + half_width; y++) {
				(*map)[x][y] = ROAD_STUD;
			}
		}
	}
	//no road laid if points are not aligned
}

void shoot_road(glm::ivec2 pt, glm::ivec2 dir, int half_width, std::vector<std::vector<int>>* map) {
	//std::cout << "Road from: " << glm::to_string(pt) << " in direction: " << glm::to_string(dir) << std::endl;
	bool intersected = true;
	bool toQuit = false;
	int since_inter = 0;
	glm::ivec2 perp = glm::ivec2(-dir.y, dir.x);
	glm::ivec2 left, right;
	for (glm::ivec2 curr = pt; std::max(curr.x, curr.y) < MAP_SIZE && std::min(curr.x, curr.y) >= 0 && !toQuit; curr += dir) {
		left = curr + (half_width + 1)*perp;
		right = curr - (half_width + 1)*perp;
		if ((*map)[left.x][left.y] == ROAD_STUD && (*map)[right.x][right.y] == ROAD_STUD) {
			if (!intersected) {//this means we just entered an intersection so back up and add markers
				glm::ivec2 lftmark = left - dir;
				glm::ivec2 rgtmark = right - dir;
				(*map)[lftmark.x][lftmark.y] = INTER_STUD;
				(*map)[rgtmark.x][rgtmark.y] = INTER_STUD;
				if (distribution(generator) < 0.2f && half_width == SM_ROAD_WIDTH) {
					toQuit = true; // some intersections cause small roads to stop
				}
			}
			intersected = true;//intersection detected
			since_inter = 0;
		}
		//place intersection markers
		if (intersected) {
			if ((*map)[left.x][left.y] != ROAD_STUD) {
				(*map)[left.x][left.y] = INTER_STUD;
				intersected = false;
			}
			if((*map)[right.x][right.y] != ROAD_STUD) {
				(*map)[right.x][right.y] = INTER_STUD;
				intersected = false;
			}
		}
		if (!intersected) {// if clear, update count since intersection
			since_inter++;
		}
		// place row of road studs
		for (int i = -half_width; i <= half_width; i++) {
			glm::ivec2 stud = curr + (perp*i);
			if (onMap(stud, roadMap)) {
				(*map)[stud.x][stud.y] = ROAD_STUD;
			}
		}
		if (since_inter >= half_width * 3) {
			if (half_width == MED_ROAD_WIDTH) {
				if (distribution(generator) < 0.015f) {
					glm::ivec2 interloc = curr - (half_width + 1)*dir;
					if (distribution(generator) < 0.5f) {
						shoot_road(interloc, perp, SM_ROAD_WIDTH, roadMap);
					} else {
						shoot_road(interloc, -perp, SM_ROAD_WIDTH, roadMap);
					}
					since_inter = 0;
				}
			} else if(half_width == MAJOR_ROAD_WIDTH){
				if (distribution(generator) < 0.04f) {
					glm::ivec2 interloc = curr - (half_width + 1)*dir;//backtrack to completed part of road
					float sample = distribution(generator);
					if (sample < 0.5f) {//place symmetrical intersection
						shoot_road(interloc, perp, MED_ROAD_WIDTH, roadMap);
						shoot_road(interloc, -perp, MED_ROAD_WIDTH, roadMap);
					}
					else if (sample < 0.75f){
						shoot_road(interloc, perp, MED_ROAD_WIDTH, roadMap);
					} else {
						shoot_road(interloc, -perp, MED_ROAD_WIDTH, roadMap);
					}
					since_inter = 0;
				}
			}
		}
	}//on loop exit drop edge markers
	(*map)[left.x][left.y] = INTER_STUD;
	(*map)[right.x][right.y] = INTER_STUD;
}

void Window::layout_roads() {
	//reseed random generator
	std::chrono::high_resolution_clock::duration d = std::chrono::high_resolution_clock::now().time_since_epoch();
	generator.seed(d.count());
	//draw corners for debug
	(*roadMap)[0][0] = INTER_STUD;
	(*roadMap)[0][MAP_SIZE - 1] = ROAD_STUD;
	(*roadMap)[MAP_SIZE - 1][0] = ROAD_STUD;
	(*roadMap)[MAP_SIZE - 1][MAP_SIZE - 1] = ROAD_STUD;
	
	if (distribution(generator) < 0.5f) {
		vertical = true;
	}
	glm::ivec2 highwayStart, highwayEnd;
	//lay major highway east west
	if (!vertical) {
		highwayStart = glm::ivec2(0, MAP_SIZE / 2);
		highwayEnd = glm::ivec2(MAP_SIZE - 1, MAP_SIZE / 2);
	} else {
		highwayStart = glm::ivec2(MAP_SIZE / 2, 0);
		highwayEnd = glm::ivec2(MAP_SIZE/2, MAP_SIZE - 1);
	}
	//road_between(highwayStart, highwayEnd, MAJOR_ROAD_WIDTH, roadMap);
	shoot_road(highwayEnd, vertical ? glm::ivec2(0, -1) : glm::ivec2(-1, 0), MAJOR_ROAD_WIDTH, roadMap);
	
	// create road map in road group
	for (int x = 0; x < MAP_SIZE; x++) {
		for (int y = 0; y < MAP_SIZE; y++) {
			if ((*roadMap)[x][y] == ROAD_STUD){
				Window::addStud(glm::ivec3(x, -1, y), roads, BrickGeode::MAT_GRAY, false);
			} else if ((*roadMap)[x][y] == INTER_STUD) {
				if ((x == 0 || (*roadMap)[x - 1][y] == ROAD_STUD) && (y == 0 || (*roadMap)[x][y - 1] == ROAD_STUD)){
					(*roadMap)[x][y] = BUILDING_CORNER;
					glm::ivec2 corner(x, y);
					buildingCorners.push_back(corner);
				} 
				Window::addStud(glm::ivec3(x, -1, y), roads, BrickGeode::MAT_GREEN, false);
			}else{
				Window::addStud(glm::ivec3(x, -1, y), roads, BrickGeode::MAT_GREEN, false);
			}
		}
	}
}

void place_building(int x, int y, int length, int width, int height) {
	MatrixTransform* base = new MatrixTransform(glm::translate(glm::mat4(), glm::vec3((float)x, 0.0, (float)y)*STUD_DIMS));
	//std::cout << "Building base: " << glm::to_string(*base) << std::endl;
	baseTransform->addChild(base);
	Building* building = new Building(length, width, height, base);
	buildings.push_back(building);
	baseTransform->update(glm::mat4());
}

void place_buildings() {
	
	for (int i = 0; i < buildingCorners.size(); i++) {
		int xDim = 0, yDim = 0;
		glm::ivec2 curr = buildingCorners[i];
		do {//find x dimension
			xDim++;
		} while (curr.x + xDim < MAP_SIZE && (*roadMap)[curr.x + xDim][curr.y] == NO_STUD);
		do {//find y dimension
			yDim++;
		} while (curr.y + yDim < MAP_SIZE && (*roadMap)[curr.x][curr.y + yDim] == NO_STUD);
		// Area term of height
		float area = xDim * yDim;
		float interp = area / 100;
		glm::ivec2 center = glm::ivec2(MAP_SIZE / 2);

		for (int xPos = curr.x; xPos < curr.x + xDim; ) {
			int xRoom = std::min(MAX_PARCEL-2, curr.x + xDim - xPos);
			float sample = distribution(generator);
			int xWidth = sample*(xRoom-2) + 3;
			for (int yPos = curr.y; yPos < curr.y + yDim;) {
				int yRoom = std::min(MAX_PARCEL-2, curr.y + yDim - yPos);
				sample = distribution(generator);
				int yLength = sample*(yRoom - 2) + 3;
				float dist = glm::length(glm::vec2(center - glm::ivec2(xPos,yPos)));
				// Add between 2 and 4 for area, up to 8 for position
				int height = std::min(std::max((int)(3 / interp), 2), 4)
					+ std::min((int)((glm::length(glm::vec2(center)) - dist) / (glm::length(glm::vec2(center)) / 10)), 8);
				place_building(xPos+1, yPos+1, xWidth, yLength, height);
				yPos += yLength + 1;
			}
			xPos += xWidth + 1;
		}
	}
}

void Window::initialize_objects()
{
	// Initialize rng
	srand(time(NULL));

	std::vector<const GLchar*> faces = { "right.ppm","left.ppm","top.ppm","base.ppm","front.ppm","back.ppm" };
	skybox = new Skybox(faces);
	brickObj = new OBJObject(lego_filepath);

	roads = new Group();
	layout_roads();
	baseTransform = new MatrixTransform();
	place_buildings();
	roads->update(glm::translate(glm::mat4(), glm::vec3(0.0)));//glm::vec3((-MAP_SIZE/2)*STUD_DIMS.x, 0, (-MAP_SIZE/2))*STUD_DIMS.z));

	old_pos = glm::vec3(highwayStart.x*STUD_DIMS.x, 2, highwayStart.y*STUD_DIMS.y);
	
	/*
	b1Trans = new MatrixTransform(glm::translate(glm::mat4(), glm::vec3(0.0)));
	b2Trans = new MatrixTransform(glm::translate(glm::mat4(), glm::vec3(10.0, 0.0, 0.0)));
	b3Trans = new MatrixTransform(glm::translate(glm::mat4(), glm::vec3(0.0, 0.0, -20.0)));
	b4Trans = new MatrixTransform(glm::translate(glm::mat4(), glm::vec3(10.0, 0.0, -20.0)));

	baseTransform->addChild(b1Trans);
	baseTransform->addChild(b2Trans);
	baseTransform->addChild(b3Trans);
	baseTransform->addChild(b4Trans);

	building1 = new Building(12, 12, 6, b1Trans);
	building2 = new Building(12, 12, 6, b2Trans);
	building3 = new Building(12, 12, 6, b3Trans);
	building4 = new Building(12, 12, 6, b4Trans);
	*/
	// Always make sure to update root after adding bricks!
	baseTransform->update(glm::mat4());

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
		P = glm::perspective(45.0f, (float)width / (float)height, 0.5f, 5000.0f);
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

	// Use the shader of programID
	glUseProgram(shaderProgram);
	//Initialize lighting
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 pointLightColors[] = {
		glm::vec3(1.0f, 0.6f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 1.0, 0.0),
		glm::vec3(0.2f, 0.2f, 1.0f)
	};
		
	// Directional light
	glUniform3f(glGetUniformLocation(shaderProgram, "dirLight.direction"), dirLightDir.x, dirLightDir.y, dirLightDir.z);
	glUniform3f(glGetUniformLocation(shaderProgram, "dirLight.ambient"), 0.5f, 0.5f, 0.5f);
	glUniform3f(glGetUniformLocation(shaderProgram, "dirLight.diffuse"), 1.0f, 1.0f, 1.0f);
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

	//Draw roads
	roads->draw(shaderProgram);

	// Draw all bricks
	baseTransform->draw(shaderProgram);

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

// Studpos should be given as number of studs offset in each direction
void Window::addStud(glm::ivec3 studpos, Group* group) {
	MatrixTransform *studTransform = new MatrixTransform(glm::translate(glm::mat4(), glm::vec3(studpos) * STUD_DIMS));
	studTransform->addChild(new BrickGeode(brickObj, 0));
	group->addChild(studTransform);
}

// Overloaded version to allow brick color specification
void Window::addStud(glm::ivec3 studpos, Group* group, int colorindex) {
	MatrixTransform *studTransform = new MatrixTransform(glm::translate(glm::mat4(), glm::vec3(studpos) * STUD_DIMS));
	studTransform->addChild(new BrickGeode(brickObj, colorindex));
	group->addChild(studTransform);
}

// Overloaded version to allow brick color specification and animation delay specification
void Window::addStud(glm::ivec3 studpos, Group* group, int colorindex, int framedelay) {
	MatrixTransform *studTransform = new MatrixTransform(glm::translate(glm::mat4(), glm::vec3(studpos) * STUD_DIMS));
	studTransform->addChild(new BrickGeode(brickObj, colorindex, framedelay));
	group->addChild(studTransform);
}

// Overloaded version to allow brick color specification and animation toggle boolean
void Window::addStud(glm::ivec3 studpos, Group* group, int colorindex, bool animate) {
	MatrixTransform *studTransform = new MatrixTransform(glm::translate(glm::mat4(), glm::vec3(studpos) * STUD_DIMS));
	studTransform->addChild(new BrickGeode(brickObj, colorindex, animate));
	group->addChild(studTransform);
}

// Brickpos should be given as number of studs offset in each direction
// Brickdims should be given as dimensions in studs
void Window::addBrick(glm::ivec3 brickpos, glm::ivec2 brickdims, Group* group) {
	MatrixTransform *brickTransform = new MatrixTransform(glm::translate(glm::mat4(), glm::vec3(brickpos) * STUD_DIMS));

	// Choose a random brick color
	int color = rand() % BrickGeode::NUM_MATS;

	for (int x = 0; x < brickdims.x; x++) {
		for (int z = 0; z < brickdims.y; z++) {
			addStud(glm::ivec3(x, 0, z), brickTransform, color);
		}
	}
	group->addChild(brickTransform);
}

// Overloaded version to allow animation delay specification
void Window::addBrick(glm::ivec3 brickpos, glm::ivec2 brickdims, Group* group, int framedelay) {
	MatrixTransform *brickTransform = new MatrixTransform(glm::translate(glm::mat4(), glm::vec3(brickpos) * STUD_DIMS));

	// Choose a random brick color
	int color = rand() % BrickGeode::NUM_MATS;

	for (int x = 0; x < brickdims.x; x++) {
		for (int z = 0; z < brickdims.y; z++) {
			addStud(glm::ivec3(x, 0, z), brickTransform, color, framedelay);
		}
	}
	group->addChild(brickTransform);
}

void Window::cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {
	double deltaX = 0.0;
	double deltaY = 0.0;

	if (lastInitialized) {
		deltaX = xpos - lastMouse.x;
		deltaY = ypos - lastMouse.y;
	}

	if (mouseButton == GLFW_MOUSE_BUTTON_LEFT) {
		glm::vec3 oldPoint = trackBallMap(lastMouse.x, lastMouse.y);
		glm::vec3 newPoint = trackBallMap(xpos, ypos);
		glm::vec3 cross = glm::cross(oldPoint, newPoint);

		float angle = 40.0 * std::acos(glm::dot(oldPoint, newPoint) / (glm::length(newPoint) * glm::length(oldPoint)));
		glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), angle / 180.0f * glm::pi<float>(), cross);

		cam_pos = glm::vec3(rotate * glm::vec4(cam_pos, 0.0));
		cam_up = glm::vec3(rotate * glm::vec4(cam_up, 0.0));
		V = glm::lookAt(cam_pos, cam_look_at, cam_up);
	}
	
	lastMouse.x = xpos;
	lastMouse.y = ypos;
	lastInitialized = true;
}

void Window::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
		//handle mouse info tracking
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
}

void Window::resetMap() {
	baseTransform->clearChildren();
	buildingCorners.clear();
	buildings.clear();

	delete roadMap;
	roadMap = new std::vector<std::vector<int>>(MAP_SIZE, std::vector<int>(MAP_SIZE, NO_STUD));
	roads->clearChildren();
	layout_roads();
	roads->update(glm::translate(glm::mat4(), glm::vec3(0.0)));
	place_buildings();
	baseTransform->update(glm::mat4());
}

void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Check for a key press
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		if (key == GLFW_KEY_R) {
			resetMap();
		}
		else if (key == GLFW_KEY_S && (mods & GLFW_MOD_SHIFT)) {
			BrickGeode::slowAnim();
		}
		else if (key == GLFW_KEY_S && !(mods & GLFW_MOD_SHIFT)) {
			BrickGeode::speedAnim();
		}
		else if (key == GLFW_KEY_D) {
			demoing = !demoing;
			if (demoing) {
				glm::vec3 temp = cam_pos;
				cam_pos = old_pos;
				old_pos = temp;
				cam_look_at = cam_pos + (vertical ? glm::vec3(0, 0, 1) : glm::vec3(1, 0, 0));
				cam_up = glm::vec3(0, 1, 0);
				V = glm::lookAt(cam_pos, cam_look_at, cam_up);
			}
			else {
				glm::vec3 temp = cam_pos;
				cam_pos = old_pos;
				old_pos = temp;
				V = glm::lookAt(cam_pos, cam_look_at, cam_up);
			}
		}
		else if (key == GLFW_KEY_EQUAL) {
			MAP_SIZE += (float)MAP_SIZE / 4.f;
			resetMap();
		}
		else if (key == GLFW_KEY_MINUS) {
			MAP_SIZE -= (float)MAP_SIZE / 4.f;
			resetMap();
		}
		else if (key == GLFW_KEY_RIGHT && demoing) {
			look_dir = glm::vec3(glm::rotate(glm::mat4(1.0f), -0.5f * glm::pi<float>(), glm::vec3(0,1,0))*glm::vec4(look_dir,0.0));
			cam_look_at = cam_pos + look_dir;
			V = glm::lookAt(cam_pos, cam_look_at, cam_up);
		}
		else if (key == GLFW_KEY_LEFT && demoing) {
			look_dir = glm::vec3(glm::rotate(glm::mat4(1.0f), 0.5f * glm::pi<float>(), glm::vec3(0, 1, 0))*glm::vec4(look_dir, 0.0));
			cam_look_at = cam_pos + look_dir;
			V = glm::lookAt(cam_pos, cam_look_at, cam_up);
		}
		else if (key == GLFW_KEY_UP) {
			glm::vec3 increment = glm::normalize(cam_look_at - cam_pos) * STUD_DIMS;
			cam_pos += increment;
			cam_look_at += increment;
			V = glm::lookAt(cam_pos, cam_look_at, cam_up);
		}

		// Check if escape was pressed
		else if (key == GLFW_KEY_ESCAPE)
		{
			// Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
	}
}