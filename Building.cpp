#include "Building.h"
#include <stdlib.h>
#include <time.h>

/* Declaration of grammar rules */
GrammarRule* Building::r1 = new GrammarRule(
	std::vector<std::vector<int>>({
	std::vector<int>({1}),
	std::vector<int>({0}),
}),
std::vector<std::vector<int>>({
	std::vector<int>({1}),
	std::vector<int>({1}),
}),
2,						// patternLength
1,						// patternWidth
glm::ivec2(1, 0),		// brickPos
glm::ivec2(1, 1));		// brickDims




std::vector<GrammarRule*> Building::rule_list = std::vector<GrammarRule*>({ r1 });
const int Building::NUM_RULES = rule_list.size();

Building::Building(int length, int width, Group* group)
{
	// x dimension
	this->length = length;
	// z dimension
	this->width = width;

	this->group = group;

	// Initialize rng
	srand(time(NULL));

	// Initialize 2D vector with 0s in all places
	studs = std::vector<std::vector<int>>(length, std::vector<int>(width, 0));

	// Create initial configuration (brick in center)
	//glm::ivec2 startBrickDims(0.0), startBrickPos(0.0);

	studs[0][0] = 1;
	brick newBrick;
	newBrick.pos = glm::vec2(0.0, 0.0);
	newBrick.dims = glm::vec2(1.0,1.0);
	bricks.push_back(newBrick);

	printf("Applying rules!\n");
	applyRules();
	construct();
}

int Building::applyRandomRule()
{
	// Get list of rules
	std::vector<GrammarRule*> *remainingRules = new std::vector<GrammarRule*>(Building::rule_list);

	// while list is not empty
	while (!remainingRules->empty()) {

		// Choose random rule and pop it from the list
		int index = rand() % (remainingRules->size());
		GrammarRule* rule = (*remainingRules)[index];
		remainingRules->erase(remainingRules->begin() + index);
		printf("Attempting to apply rule #%d...\n", index);

		for (int x = 0; x < length; x++) {
			for (int z = 0; z < width; z++) {
				bool matches = true;
				for (int xp = 0; xp < rule->patternLength; xp++) {
					for (int zp = 0; zp < rule->patternWidth; zp++) {
						// Bounds checking
						if ((x + xp < length) && (z + zp < width)) {

							if (studs[x + xp][z + zp] != rule->pattern[xp][zp]) {
								matches = false;
							}
						}
						else {
							// No match if part of the pattern is out of bounds
							matches = false;
						}
					}
				}
				if (matches) {
					printf("Match!\n");
					// now replace the pattern with the output
					for (int xp = 0; xp < rule->patternLength; xp++) {
						for (int zp = 0; zp < rule->patternWidth; zp++) {
							studs[x + xp][z + zp] = rule->output[xp][zp];
						}
					}
					brick newBrick;
					newBrick.pos = rule->brickPos + glm::ivec2(x, z);
					newBrick.dims = rule->brickDims;
					this->bricks.push_back(newBrick);
					return 1;
				}
			}
		}
	}
	
	// If no rules matched, return a failure
	return 0;
}

void Building::applyRules()
{
	// Generate random number of rules to apply
	int maxrules = (rand() % MAX_RULES_APPLIED) + MIN_RULES_APPLIED;
	printf("Going to apply %d rules!\n", maxrules);
	for (int rulenum = 0; rulenum < maxrules; rulenum++) {
		if (!this->applyRandomRule()) {
			printf("No rules matched.\n");
			return;
		}
	}
}

void Building::construct()
{
	for (int i = 0; i < bricks.size(); i++) {
		Window::addBrick(glm::vec3(bricks[i].pos.x, 0.0, bricks[i].pos.y), bricks[i].dims, this->group);
	}
}

void Building::reset()
{
	// Remove al bricks
	bricks.clear();
	group->clearChildren();

	// Initialize 2D vector with 0s in all places
	studs = std::vector<std::vector<int>>(length, std::vector<int>(width, 0));

	studs[0][0] = 1;
	brick newBrick;
	newBrick.pos = glm::vec2(0.0, 0.0);
	newBrick.dims = glm::vec2(1.0, 1.0);
	bricks.push_back(newBrick);

	printf("Applying rules!\n");
	applyRules();
	construct();
}