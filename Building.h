#ifndef _BUILDING_H_
#define _BUILDING_H_
#include "Window.h"
#include "Group.h"
#include "GrammarRule.h"
#include <vector>

struct brick
{
	glm::ivec2 pos;
	glm::ivec2 dims;
};

class Building
{
public:
	Building(int length, int width, int height, Group*);
	void reset();

	Group *group;

	static std::vector<GrammarRule*> rule_list;
	static const int NUM_RULES;

private:
	static const int MIN_RULES_APPLIED = 5;
	static const int MAX_RULES_APPLIED = 8;
	int width, length, height, min_rules_applied, max_rules_applied;

	/* In this 2d vector, 
	 *	0 represents open space
	 *  1 represents a space occupied by a stud
	 */
	std::vector<std::vector<int>> studs;

	// Vector of bricks used when calling addBrick in construct
	std::vector<brick> bricks;

	int applyRandomRule();
	void applyRules();
	void construct();

	// Grammar Rules
	static GrammarRule* r_1x1_adjacent[4];
	static GrammarRule* r_1x1_space[4];
	static GrammarRule* r_1x2_adjacent[4];
	static GrammarRule* r_1x3_adjacent[4];
	static GrammarRule* r_1x4_adjacent[4];
};

#endif