#include "Building.h"
#include <stdlib.h>
#include <time.h>
#include <algorithm>

/* Declaration of grammar rules */

// 1x1 adjacent: For facade details
GrammarRule* Building::r_1x1_adjacent[4] = {

new GrammarRule(
	std::vector<std::vector<int>>({		// pattern
std::vector<int>({ 1 }),
std::vector<int>({ 0 })
}),
glm::ivec2(1, 0),						// brickPos
glm::ivec2(1, 1)						// brickDims
),

new GrammarRule(
	std::vector<std::vector<int>>({		// pattern
	std::vector<int>({ 1, 0 })
}),
glm::ivec2(0, 1),						// brickPos
glm::ivec2(1, 1)						// brickDims
),

new GrammarRule(
	std::vector<std::vector<int>>({		// pattern
	std::vector<int>({ 0 }),
	std::vector<int>({ 1 })
}),
glm::ivec2(0, 0),						// brickPos
glm::ivec2(1, 1)						// brickDims
),

new GrammarRule(
	std::vector<std::vector<int>>({		// pattern
	std::vector<int>({ 0, 1 })
}),
glm::ivec2(0, 0),						// brickPos
glm::ivec2(1, 1)						// brickDims
)
};

// 1x1 separated by a space
GrammarRule* Building::r_1x1_space[4] = {

new GrammarRule(
	std::vector<std::vector<int>>({		// pattern
		std::vector<int>({ 1 }),
		std::vector<int>({ 0 }),
		std::vector<int>({ 0 })
}),
glm::ivec2(2, 0),						// brickPos
glm::ivec2(1, 1)						// brickDims
),

new GrammarRule(
	std::vector<std::vector<int>>({		// pattern
		std::vector<int>({ 1, 0, 0 })
}),
glm::ivec2(0, 2),						// brickPos
glm::ivec2(1, 1)						// brickDims
),

new GrammarRule(
	std::vector<std::vector<int>>({		// pattern
	std::vector<int>({ 0 }),
	std::vector<int>({ 0 }),
	std::vector<int>({ 1 })
}),
glm::ivec2(0, 0),						// brickPos
glm::ivec2(1, 1)						// brickDims
),

new GrammarRule(
	std::vector<std::vector<int>>({		// pattern
	std::vector<int>({ 0, 0, 1 })
}),
glm::ivec2(0, 0),						// brickPos
glm::ivec2(1, 1)						// brickDims
)
};

// 1x2 adjacent lengthwise: For jutting features and courtyards
GrammarRule* Building::r_1x2_adjacent[4] = {

	new GrammarRule(
		std::vector<std::vector<int>>({		// pattern
	std::vector<int>({ 1 }),
	std::vector<int>({ 0 }),
	std::vector<int>({ 0 })
}),
glm::ivec2(1, 0),						// brickPos
glm::ivec2(2, 1)						// brickDims
),

new GrammarRule(
	std::vector<std::vector<int>>({		// pattern
	std::vector<int>({ 1, 0, 0 })
}),
glm::ivec2(0, 1),						// brickPos
glm::ivec2(1, 2)						// brickDims
),

new GrammarRule(
	std::vector<std::vector<int>>({		// pattern
	std::vector<int>({ 0 }),
	std::vector<int>({ 0 }),
	std::vector<int>({ 1 })
}),
glm::ivec2(0, 0),						// brickPos
glm::ivec2(2, 1)						// brickDims
),

new GrammarRule(
	std::vector<std::vector<int>>({		// pattern
	std::vector<int>({ 0, 0, 1 })
}),
glm::ivec2(0, 0),						// brickPos
glm::ivec2(1, 2)						// brickDims
)
};

// 1x3 adjacent lengthwise: For jutting features and courtyards
GrammarRule* Building::r_1x3_adjacent[4] = {

new GrammarRule(
	std::vector<std::vector<int>>({		// pattern
std::vector<int>({ 1 }),
std::vector<int>({ 0 }),
std::vector<int>({ 0 }),
std::vector<int>({ 0 })
}),
glm::ivec2(1, 0),						// brickPos
glm::ivec2(3, 1)						// brickDims
),

new GrammarRule(
	std::vector<std::vector<int>>({		// pattern
	std::vector<int>({ 1, 0, 0, 0 })
}),
glm::ivec2(0, 1),						// brickPos
glm::ivec2(1, 3)						// brickDims
),

new GrammarRule(
	std::vector<std::vector<int>>({		// pattern
	std::vector<int>({ 0 }),
	std::vector<int>({ 0 }),
	std::vector<int>({ 0 }),
	std::vector<int>({ 1 })
}),
glm::ivec2(0, 0),						// brickPos
glm::ivec2(3, 1)						// brickDims
),

new GrammarRule(
	std::vector<std::vector<int>>({		// pattern
	std::vector<int>({ 0, 0, 0, 1 })
}),
glm::ivec2(0, 0),						// brickPos
glm::ivec2(1, 3)						// brickDims
)
};

// 1x4 adjacent lengthwise: For jutting features and courtyards
GrammarRule* Building::r_1x4_adjacent[4] = {

new GrammarRule(
	std::vector<std::vector<int>>({		// pattern
std::vector<int>({ 1 }),
std::vector<int>({ 0 }),
std::vector<int>({ 0 }),
std::vector<int>({ 0 }),
std::vector<int>({ 0 })
}),
glm::ivec2(1, 0),						// brickPos
glm::ivec2(4, 1)						// brickDims
),

new GrammarRule(
	std::vector<std::vector<int>>({		// pattern
	std::vector<int>({ 1, 0, 0, 0, 0 })
}),
glm::ivec2(0, 1),						// brickPos
glm::ivec2(1, 4)						// brickDims
),

new GrammarRule(
	std::vector<std::vector<int>>({		// pattern
	std::vector<int>({ 0 }),
	std::vector<int>({ 0 }),
	std::vector<int>({ 0 }),
	std::vector<int>({ 0 }),
	std::vector<int>({ 1 })
}),
glm::ivec2(0, 0),						// brickPos
glm::ivec2(4, 1)						// brickDims
),

new GrammarRule(
	std::vector<std::vector<int>>({		// pattern
	std::vector<int>({ 0, 0, 0, 0, 1 })
}),
glm::ivec2(0, 0),						// brickPos
glm::ivec2(1, 4)						// brickDims
)
};

// 2x2 adjacent by a full edge: For solid foundations
GrammarRule* Building::r_2x2_adjacent[4] = {

new GrammarRule(
	std::vector<std::vector<int>>({		// pattern
std::vector<int>({ 1, 1 }),
std::vector<int>({ 0, 0 }),
std::vector<int>({ 0, 0 })
}),
glm::ivec2(1, 0),						// brickPos
glm::ivec2(2, 2)						// brickDims
),

new GrammarRule(
	std::vector<std::vector<int>>({		// pattern
	std::vector<int>({ 1, 0, 0 }),
	std::vector<int>({ 1, 0, 0 })
}),
glm::ivec2(0, 1),						// brickPos
glm::ivec2(2, 2)						// brickDims
),

new GrammarRule(
	std::vector<std::vector<int>>({		// pattern
	std::vector<int>({ 0, 0 }),
	std::vector<int>({ 0, 0 }),
	std::vector<int>({ 1, 1 })
}),
glm::ivec2(0, 0),						// brickPos
glm::ivec2(2, 2)						// brickDims
),

new GrammarRule(
	std::vector<std::vector<int>>({		// pattern
	std::vector<int>({ 0, 0, 1 }),
	std::vector<int>({ 0, 0, 1 })
}),
glm::ivec2(0, 0),						// brickPos
glm::ivec2(2, 2)						// brickDims
)

};

// 2x3 adjacent by a full edge: For solid foundations
GrammarRule* Building::r_2x3_adjacent[8] = {

new GrammarRule(
	std::vector<std::vector<int>>({		// pattern
std::vector<int>({ 1, 1 }),
std::vector<int>({ 0, 0 }),
std::vector<int>({ 0, 0 }),
std::vector<int>({ 0, 0 })
}),
glm::ivec2(1, 0),						// brickPos
glm::ivec2(3, 2)						// brickDims
),

new GrammarRule(
	std::vector<std::vector<int>>({		// pattern
	std::vector<int>({ 1, 1, 1 }),
	std::vector<int>({ 0, 0, 0 }),
	std::vector<int>({ 0, 0, 0 })
}),
glm::ivec2(1, 0),						// brickPos
glm::ivec2(2, 3)						// brickDims
),

new GrammarRule(
	std::vector<std::vector<int>>({		// pattern
	std::vector<int>({ 1, 0, 0, 0 }),
	std::vector<int>({ 1, 0, 0, 0 })
}),
glm::ivec2(0, 1),						// brickPos
glm::ivec2(2, 3)						// brickDims
),

new GrammarRule(
	std::vector<std::vector<int>>({		// pattern
	std::vector<int>({ 1, 0, 0 }),
	std::vector<int>({ 1, 0, 0 }),
	std::vector<int>({ 1, 0, 0 })
}),
glm::ivec2(0, 1),						// brickPos
glm::ivec2(3, 2)						// brickDims
),

new GrammarRule(
	std::vector<std::vector<int>>({		// pattern
	std::vector<int>({ 0, 0 }),
	std::vector<int>({ 0, 0 }),
	std::vector<int>({ 0, 0 }),
	std::vector<int>({ 1, 1 })
}),
glm::ivec2(0, 0),						// brickPos
glm::ivec2(3, 2)						// brickDims
),

new GrammarRule(
	std::vector<std::vector<int>>({		// pattern
	std::vector<int>({ 0, 0, 0 }),
	std::vector<int>({ 0, 0, 0 }),
	std::vector<int>({ 1, 1, 1 })
}),
glm::ivec2(0, 0),						// brickPos
glm::ivec2(2, 3)						// brickDims
),

new GrammarRule(
	std::vector<std::vector<int>>({		// pattern
	std::vector<int>({ 0, 0, 0, 1 }),
	std::vector<int>({ 0, 0, 0, 1 })
}),
glm::ivec2(0, 0),						// brickPos
glm::ivec2(2, 3)						// brickDims
),

new GrammarRule(
	std::vector<std::vector<int>>({		// pattern
	std::vector<int>({ 0, 0, 1 }),
	std::vector<int>({ 0, 0, 1 }),
	std::vector<int>({ 0, 0, 1 })
}),
glm::ivec2(0, 0),						// brickPos
glm::ivec2(3, 2)						// brickDims
)

};

// 2x4 adjacent by a full edge: For solid foundations
GrammarRule* Building::r_2x4_adjacent[8] = {

	new GrammarRule(
		std::vector<std::vector<int>>({		// pattern
	std::vector<int>({ 1, 1 }),
	std::vector<int>({ 0, 0 }),
	std::vector<int>({ 0, 0 }),
	std::vector<int>({ 0, 0 }),
	std::vector<int>({ 0, 0 })
}),
glm::ivec2(1, 0),						// brickPos
glm::ivec2(4, 2)						// brickDims
),

new GrammarRule(
	std::vector<std::vector<int>>({		// pattern
	std::vector<int>({ 1, 1, 1, 1 }),
	std::vector<int>({ 0, 0, 0, 0 }),
	std::vector<int>({ 0, 0, 0, 0 })
}),
glm::ivec2(1, 0),						// brickPos
glm::ivec2(2, 4)						// brickDims
),

new GrammarRule(
	std::vector<std::vector<int>>({		// pattern
	std::vector<int>({ 1, 0, 0, 0, 0 }),
	std::vector<int>({ 1, 0, 0, 0, 0 })
}),
glm::ivec2(0, 1),						// brickPos
glm::ivec2(2, 4)						// brickDims
),

new GrammarRule(
	std::vector<std::vector<int>>({		// pattern
	std::vector<int>({ 1, 0, 0 }),
	std::vector<int>({ 1, 0, 0 }),
	std::vector<int>({ 1, 0, 0 }),
	std::vector<int>({ 1, 0, 0 })
}),
glm::ivec2(0, 1),						// brickPos
glm::ivec2(4, 2)						// brickDims
),

new GrammarRule(
	std::vector<std::vector<int>>({		// pattern
	std::vector<int>({ 0, 0 }),
	std::vector<int>({ 0, 0 }),
	std::vector<int>({ 0, 0 }),
	std::vector<int>({ 0, 0 }),
	std::vector<int>({ 1, 1 })
}),
glm::ivec2(0, 0),						// brickPos
glm::ivec2(4, 2)						// brickDims
),

new GrammarRule(
	std::vector<std::vector<int>>({		// pattern
	std::vector<int>({ 0, 0, 0, 0 }),
	std::vector<int>({ 0, 0, 0, 0 }),
	std::vector<int>({ 1, 1, 1, 1 })
}),
glm::ivec2(0, 0),						// brickPos
glm::ivec2(2, 4)						// brickDims
),

new GrammarRule(
	std::vector<std::vector<int>>({		// pattern
	std::vector<int>({ 0, 0, 0, 0, 1 }),
	std::vector<int>({ 0, 0, 0, 0, 1 })
}),
glm::ivec2(0, 0),						// brickPos
glm::ivec2(2, 4)						// brickDims
),

new GrammarRule(
	std::vector<std::vector<int>>({		// pattern
	std::vector<int>({ 0, 0, 1 }),
	std::vector<int>({ 0, 0, 1 }),
	std::vector<int>({ 0, 0, 1 }),
	std::vector<int>({ 0, 0, 1 })
}),
glm::ivec2(0, 0),						// brickPos
glm::ivec2(4, 2)						// brickDims
)

};

std::vector<GrammarRule*> Building::rule_list = std::vector<GrammarRule*>({ 
	r_1x1_adjacent[0], r_1x1_adjacent[1], r_1x1_adjacent[2], r_1x1_adjacent[3],
	/*r_1x1_space[0], r_1x1_space[1], r_1x1_space[2], r_1x1_space[3],*/
	r_1x2_adjacent[0], r_1x2_adjacent[1], r_1x2_adjacent[2], r_1x2_adjacent[3],
	r_1x3_adjacent[0], r_1x3_adjacent[1], r_1x3_adjacent[2], r_1x3_adjacent[3],
	r_1x4_adjacent[0], r_1x4_adjacent[1], r_1x4_adjacent[2], r_1x4_adjacent[3],
	r_2x2_adjacent[0], r_2x2_adjacent[1], r_2x2_adjacent[2], r_2x2_adjacent[3],
	r_2x3_adjacent[0], r_2x3_adjacent[1], r_2x3_adjacent[2], r_2x3_adjacent[3],
	r_2x3_adjacent[4], r_2x3_adjacent[5], r_2x3_adjacent[6], r_2x3_adjacent[7],
	r_2x4_adjacent[0], r_2x4_adjacent[1], r_2x4_adjacent[2], r_2x4_adjacent[3],
	r_2x4_adjacent[4], r_2x4_adjacent[5], r_2x4_adjacent[6], r_2x4_adjacent[7]
});

const int Building::NUM_RULES = rule_list.size();

Building::Building(int length, int width, int height, Group* group)
{
	// x dimension
	this->length = length;
	if (this->length < 0) { this->length = 1;  }

	// z dimension
	this->width = width;
	if (this->width < 0) { this->width = 1; }

	// y dimension
	this->height = height;
	if (this->height < 0) { this->height = 1; }

	this->max_rules_applied = 2 * std::max(this->length, this->width);
	this->min_rules_applied = std::min(this->length, this->width);

	this->group = group;

	// Initialize rng
	srand(time(NULL));

	// Initialize 2D vector with 0s in all places
	studs = std::vector<std::vector<int>>(length, std::vector<int>(width, 0));

	// Create initial configuration (brick in center)
	brick startBrick;
	startBrick.pos = glm::ivec2(length / 2, width / 2);
	startBrick.dims = glm::ivec2(1);
	if (length % 2 == 0) { 
		startBrick.pos.x -= 1;
		startBrick.dims.x = 2; 
	}
	if (width % 2 == 0) { 
		startBrick.pos.y -= 1;
		startBrick.dims.y = 2; 
	}
	bricks.push_back(startBrick);

	for (int x = 0; x < startBrick.dims.x; x++) {
		for (int z = 0; z < startBrick.dims.y; z++) {
			studs[startBrick.pos.x + x][startBrick.pos.y + z] = 1;
		}
	}

	applyRules();
	construct();
}

int Building::applyRandomRule()
{
	// Get list of rules
	std::vector<GrammarRule*> *remainingRules = new std::vector<GrammarRule*>(Building::rule_list);

	// while list is not empty
	while (!remainingRules->empty()) {
		// Create an empty list of match positions
		std::vector<glm::ivec2> matches;

		// Choose random rule and pop it from the list
		int index = rand() % (remainingRules->size());
		GrammarRule* rule = (*remainingRules)[index];
		remainingRules->erase(remainingRules->begin() + index);
		//printf("Attempting to apply rule #%d...\n", index);

		for (int x = 0; x < length; x++) {
			for (int z = 0; z < width; z++) {

				bool match = true; // Becomes false if the pattern is not matched

				for (int xp = 0; xp < rule->patternLength; xp++) {
					for (int zp = 0; zp < rule->patternWidth; zp++) {

						/*
						if (x == 3 && z == 3) {
							printf("xp = %d, zp = %d\n", xp, zp);
							printf("studs[x + xp][z + zp] = %d, rule->pattern[xp][zp] = %d\n\n", studs[x + xp][z + zp], rule->pattern[xp][zp]);
						}
						*/

						// Bounds checking
						if ((x + xp < length) && (z + zp < width)) {

							if (studs[x + xp][z + zp] != rule->pattern[xp][zp]) {
								match = false; // We found a mismatch with the pattern
							}
						}
						else {
							// No match if part of the pattern is out of bounds
							match = false;
						}
					}
				}
				if (match) {
					//printf("Match!\n");
					// Add to our list of match positions
					matches.push_back(glm::ivec2(x, z));
				}
			}
		}

		if (matches.size() > 0) {
			// If we have some match positions, choose one randomly
			index = rand() % (matches.size());
			glm::ivec2 pos = matches[index];

			// now replace the pattern with the output
			for (int xp = 0; xp < rule->patternLength; xp++) {
				for (int zp = 0; zp < rule->patternWidth; zp++) {
					studs[pos.x + xp][pos.y + zp] = rule->output[xp][zp];
				}
			}
			brick newBrick;
			newBrick.pos = rule->brickPos + glm::ivec2(pos.x, pos.y);
			newBrick.dims = rule->brickDims;
			this->bricks.push_back(newBrick);
			return 1;
		}
	}
	
	// If no rules matched, return a failure
	return 0;
}

void Building::applyRules()
{
	// Generate random number of rules to apply
	int maxrules = (rand() % (max_rules_applied - min_rules_applied + 1)) + min_rules_applied;
	//printf("Going to apply %d rules!\n", maxrules);
	for (int rulenum = 0; rulenum < maxrules; rulenum++) {
		if (!this->applyRandomRule()) {
			//printf("No rules matched.\n");
			return;
		}
	}
}

void Building::construct()
{
	// Add same brick pattern to every floor
	for (int i = 0; i < bricks.size(); i++) {
		for (int j = 0; j < this->height; j++) {
			int framedelay = (i * 10) + (j * (bricks.size() * 10));
			Window::addBrick(glm::vec3(bricks[i].pos.x, j, bricks[i].pos.y), bricks[i].dims, this->group, framedelay);
		}
	}
}

void Building::reset()
{
	// Remove all bricks
	bricks.clear();
	group->clearChildren();

	// Initialize 2D vector with 0s in all places
	studs = std::vector<std::vector<int>>(length, std::vector<int>(width, 0));

	// Create initial configuration (brick in center)
	brick startBrick;
	startBrick.pos = glm::ivec2(length / 2, width / 2);
	startBrick.dims = glm::ivec2(1);
	if (length % 2 == 0) {
		startBrick.pos.x -= 1;
		startBrick.dims.x = 2;
	}
	if (width % 2 == 0) {
		startBrick.pos.y -= 1;
		startBrick.dims.y = 2;
	}
	bricks.push_back(startBrick);

	for (int x = 0; x < startBrick.dims.x; x++) {
		for (int z = 0; z < startBrick.dims.y; z++) {
			studs[startBrick.pos.x + x][startBrick.pos.y + z] = 1;
		}
	}

	applyRules();
	construct();
}