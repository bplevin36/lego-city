#ifndef _GRAMMARRULE_H_
#define _GRAMMARRULE_H_
#include <vector>
#include "Node.h"

class GrammarRule
{
public:
	GrammarRule(std::vector<std::vector<int>> pattern, glm::ivec2 brickPos, glm::ivec2 brickDims)
	{
		this->pattern = pattern;

		this->patternLength = this->pattern.size();
		this->patternWidth = this->pattern[0].size();
		this->brickDims = brickDims;
		this->brickPos = brickPos;

		this->output = std::vector<std::vector<int>>(this->pattern);
		
		for (int x = brickPos.x; x < brickPos.x + brickDims.x; x++) {
			for (int z = brickPos.y; z < brickPos.y + brickDims.y; z++) {
				this->output[x][z] = 1;
			}
		}
	};

	std::vector<std::vector<int>> pattern, output;
	int patternLength, patternWidth;
	glm::ivec2 brickPos, brickDims;
};
#endif