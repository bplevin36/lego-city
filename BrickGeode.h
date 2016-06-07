#ifndef _BRICKGEODE_H_
#define _BRICKGEODE_H_

#include "Geode.h"
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <deque>
#include <limits.h>
#include <SFML/Audio.hpp>

struct material {
	glm::vec3 ambient, diffuse, specular;
	float shininess;

	material(glm::vec3 a, glm::vec3 d, glm::vec3 s, float shine) :
		ambient(a),
		diffuse(d),
		specular(s),
		shininess(shine) {}

	material() :
		ambient(glm::vec3(0.0)),
		diffuse(glm::vec3(0.0)),
		specular(glm::vec3(0.0)),
		shininess(32.0) {}
};

class BrickGeode : public Geode
{
public:
	BrickGeode(OBJObject *obj, int colorindex);
	BrickGeode(OBJObject *obj, int colorindex, int framedelay);
	BrickGeode(OBJObject *obj, int colorindex, bool animate);
	void draw(GLuint shaderProgram);

	static void speedAnim();
	static void slowAnim();

	static std::vector<material> materials;
	static int MAT_WHITE, MAT_RED, MAT_BLUE, MAT_GREEN, MAT_YELLOW, MAT_GRAY;
	static int NUM_MATS;

private:
	static float BrickGeode::anim_offset;
	static int BrickGeode::frame_max;

	int framecount = 0;
	int framedelay = 0;
	material mat;

	static const int NUM_SOUNDS = 5;
	static const int MAX_SOUNDS = 50;

	static const char* sound_filepaths[NUM_SOUNDS];

	// For sounds
	static bool soundsLoaded;
	static std::deque<sf::Sound*> soundQueue;
	static sf::SoundBuffer buffers[BrickGeode::NUM_SOUNDS];
};

#endif

