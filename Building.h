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
	Building(int length, int width, Group*);
	void reset();

	Group *group;

	static std::vector<GrammarRule*> rule_list;
	static const int NUM_RULES;

private:
	static const int MIN_RULES_APPLIED = 1;
	static const int MAX_RULES_APPLIED = 2;
	int width, length;

	/* In this 2d vector, 
	 *	0 represents no stud in that position
	 *	1 represents a stud in that position
	 */
	std::vector<std::vector<int>> studs;

	// Vector of bricks used when calling addBrick in construct
	std::vector<brick> bricks;

	int applyRandomRule();
	void applyRules();
	void construct();

	// Grammar Rules
	static GrammarRule *r1;
};

#endif