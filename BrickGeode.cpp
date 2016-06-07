#include "BrickGeode.h"

bool BrickGeode::soundsLoaded = false;
std::deque<sf::Sound*> BrickGeode::soundQueue;

sf::SoundBuffer BrickGeode::buffers[BrickGeode::NUM_SOUNDS];
const char* BrickGeode::sound_filepaths[BrickGeode::NUM_SOUNDS] =
{ "click_1.wav", "click_2.wav", "click_3.wav", "click_4.wav", "click_5.wav" };

float BrickGeode::anim_offset = 0.5f;
int BrickGeode::frame_max = 50;

int BrickGeode::MAT_RED = 0;
int BrickGeode::MAT_YELLOW = 1;
int BrickGeode::MAT_GREEN = 2;
int BrickGeode::MAT_BLUE = 3;
int BrickGeode::MAT_WHITE = 4;
int BrickGeode::MAT_GRAY = 5;

std::vector<material> BrickGeode::materials = std::vector<material>({
	// Red plastic
	material(	glm::vec3(0.4, 0.0, 0.0),
				glm::vec3(0.8, 0.0, 0.0),
				glm::vec3(0.7, 0.6, 0.6),
				32.0),

	// Yellow Plastic
	material(	glm::vec3(0.4, 0.4, 0.0),
				glm::vec3(0.8, 0.8, 0.0),
				glm::vec3(0.6, 0.6, 0.5),
				32.0),

	// Green Plastic
	material(	glm::vec3(0.1, 0.35, 0.1),
				glm::vec3(0.2, 0.7, 0.2),
				glm::vec3(0.45, 0.55, 0.45),
				32.0),

	// Blue Plastic
	material(	glm::vec3(0.0, 0.0, 0.425),
				glm::vec3(0.0, 0.0, 0.85),
				glm::vec3(0.6, 0.6, 0.7),
				32.0),

	// White Plastic
	material(glm::vec3(0.45, 0.45, 0.45),
		glm::vec3(0.9, 0.9, 0.9),
		glm::vec3(0.7, 0.7, 0.7),
		32.0),

	// Gray Plastic
	material(glm::vec3(0.45, 0.45, 0.45),
		glm::vec3(0.6, 0.6, 0.6),
		glm::vec3(0.6, 0.6, 0.6),
		8.0)
});

int BrickGeode::NUM_MATS = (int) materials.size();

BrickGeode::BrickGeode(OBJObject *obj, int colorindex)
{
	this->obj = obj;
	this->mat = BrickGeode::materials[colorindex];

	// Load sounds
	if (!BrickGeode::soundsLoaded) {
		for (int i = 0; i < BrickGeode::NUM_SOUNDS; i++) {
			BrickGeode::buffers[i].loadFromFile(BrickGeode::sound_filepaths[i]);
		}
		BrickGeode::soundsLoaded = true;
	}
}

BrickGeode::BrickGeode(OBJObject *obj, int colorindex, int framedelay)
{
	this->obj = obj;
	this->mat = BrickGeode::materials[colorindex];
	this->framedelay = framedelay;

	// Load sounds
	if (!BrickGeode::soundsLoaded) {
		for (int i = 0; i < BrickGeode::NUM_SOUNDS; i++) {
			BrickGeode::buffers[i].loadFromFile(BrickGeode::sound_filepaths[i]);
		}
		BrickGeode::soundsLoaded = true;
	}
}

BrickGeode::BrickGeode(OBJObject *obj, int colorindex, bool animate)
{
	this->obj = obj;
	this->mat = BrickGeode::materials[colorindex];
	
	if (animate == false) {
		this->framecount = INT_MAX;
	}

	// Load sounds
	if (!BrickGeode::soundsLoaded) {
		for (int i = 0; i < BrickGeode::NUM_SOUNDS; i++) {
			BrickGeode::buffers[i].loadFromFile(BrickGeode::sound_filepaths[i]);
		}
		BrickGeode::soundsLoaded = true;
	}
}

void BrickGeode::draw(GLuint shaderProgram)
{
	// Set material of brick
	glUniform3f(glGetUniformLocation(shaderProgram, "material.ambient"), mat.ambient.x, mat.ambient.y, mat.ambient.z);
	glUniform3f(glGetUniformLocation(shaderProgram, "material.diffuse"), mat.diffuse.x, mat.diffuse.y, mat.diffuse.z);
	glUniform3f(glGetUniformLocation(shaderProgram, "material.specular"), mat.specular.x, mat.specular.y, mat.specular.z);
	glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), mat.shininess);

	if (framedelay <= 0) {
		glm::mat4 animMat = this->toWorld;

		// If on last frame, play sound effect	
		if (framecount == (BrickGeode::frame_max - 1)) {

			// Check is sounds have finished playing
			if (!(BrickGeode::soundQueue.empty())) {
				while (BrickGeode::soundQueue.front()->getStatus() == 0) {
					delete(BrickGeode::soundQueue.front());
					BrickGeode::soundQueue.pop_front();
					if (BrickGeode::soundQueue.empty()) { break; }
				}
			}

			// Only play if enough sf::Sound objects are available
			if (BrickGeode::soundQueue.size() < BrickGeode::MAX_SOUNDS) {
				// Choose random sound
				int index = rand() % BrickGeode::NUM_SOUNDS;
				sf::Sound* sound = new sf::Sound(buffers[index]);
				sound->play();
				BrickGeode::soundQueue.push_back(sound);
			}
		}

		if (framecount < BrickGeode::frame_max) {
			animMat = glm::translate(animMat, glm::vec3(0.0, (BrickGeode::frame_max - framecount) * BrickGeode::anim_offset, 0.0));
			framecount++;
		}

		if (framecount == BrickGeode::frame_max) {
			framecount = INT_MAX; // make sure we never animate thie brick again
		}

		obj->setToWorld(animMat);
		obj->draw(shaderProgram);
	}
	else { framedelay--; }
}

void BrickGeode::speedAnim() {
	BrickGeode::anim_offset *= 2;
	BrickGeode::frame_max /= 2;
}

void BrickGeode::slowAnim() {
	BrickGeode::anim_offset /= 2;
	BrickGeode::frame_max *= 2;
}