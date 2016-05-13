#ifndef _GROUP_H_
#define _GROUP_H_
#include "C:\Users\Ben\Downloads\CSE167-Starter-Code-2-master\CSE167-Starter-Code-2-master\Node.h"
#include <list>

class Group :
	public Node
{
public:
	Group();
	~Group();
	void addChild(Node*);
	void removeChild(Node*);
	virtual void draw(GLuint);
	virtual void update(glm::mat4);
protected:
	std::list<Node*> children;
};

#endif