#ifndef _GRAMMARRULE_H_
#define _GRAMMARRULE_H_
#include <vector>
#include "Node.h"

class GrammarRule
{
public:
	GrammarRule(std::vector<std::vector<int>> pattern, std::vector<std::vector<int>> output,
		int patternLength, int patternWidth, glm::ivec2 brickPos, glm::ivec2 brickDims)
	{
		this->pattern = pattern;
		this->output = output;
		this->patternLength = patternLength;
		this->patternWidth = patternWidth;
		this->brickDims = brickDims;
		this->brickPos = brickPos;
	}

	std::vector<std::vector<int>> pattern, output;
	int patternLength, patternWidth;
	glm::ivec2 brickPos, brickDims;
};
#endif