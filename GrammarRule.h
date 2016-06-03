#ifndef _GRAMMARRULE_H_
#define _GRAMMARRULE_H_
#include <vector>
#include "Node.h"

class GrammarRule
{
public:
	GrammarRule(std::vector<std::vector<int>> pattern, std::vector<std::vector<int>> output,
		int patternLength, int patternWidth, int brickType, glm::vec2 brickPos)
	{
		this->pattern = pattern;
		this->output = output;
		this->patternLength = patternLength;
		this->patternWidth = patternWidth;
		this->brickType = brickType;
		this->brickPos = brickPos;
	}

	std::vector<std::vector<int>> pattern, output;
	int patternLength, patternWidth, brickType;
	glm::vec2 brickPos;
};
#endif