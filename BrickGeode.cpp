#include "BrickGeode.h"

bool BrickGeode::soundsLoaded = false;
std::deque<sf::Sound*> BrickGeode::soundQueue;

sf::SoundBuffer BrickGeode::buffers[BrickGeode::NUM_SOUNDS];
const char* BrickGeode::sound_filepaths[BrickGeode::NUM_SOUNDS] =
{ "click_1.wav", "click_2.wav", "click_3.wav", "click_4.wav", "click_5.wav" };

const float BrickGeode::ANIM_OFFSET = 0.1f;


std::vector<material> BrickGeode::materials = std::vector<material>({
	// Red plastic
	material(	glm::vec3(0.0, 0.0, 0.0),
				glm::vec3(0.5, 0.0, 0.0),
				glm::vec3(0.7, 0.6, 0.6),
				32.0),

	// Yellow Plastic
	material(	glm::vec3(0.0, 0.0, 0.0),
				glm::vec3(0.5, 0.5, 0.0),
				glm::vec3(0.6, 0.6, 0.5),
				32.0),

	// Green Plastic
	material(	glm::vec3(0.0, 0.0, 0.0),
				glm::vec3(0.1, 0.35, 0.1),
				glm::vec3(0.45, 0.55, 0.45),
				32.0),

	// Blue Plastic
	material(	glm::vec3(0.0, 0.0, 0.0),
				glm::vec3(0.0, 0.0, 0.5),
				glm::vec3(0.6, 0.6, 0.7),
				32.0),

	// White Plastic
	material(glm::vec3(0.0, 0.0, 0.0),
		glm::vec3(0.55, 0.55, 0.55),
		glm::vec3(0.7, 0.7, 0.7),
		32.0)
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
		if (framecount == 1) {

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

		if (framecount > 0) {
			animMat = glm::translate(animMat, glm::vec3(0.0, framecount * BrickGeode::ANIM_OFFSET, 0.0));
			framecount--;
		}

		obj->setToWorld(animMat);
		obj->draw(shaderProgram);
	}
	else { framedelay--; }
}