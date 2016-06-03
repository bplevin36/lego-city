#ifndef _GROUP_H_
#define _GROUP_H_
#include "Node.h"
#include <list>

class Group :
	public Node
{
public:
	Group();
	~Group();
	void addChild(Node*);
	void removeChild(Node*);
	void clearChildren();
	virtual void draw(GLuint);
	virtual void update(glm::mat4);
protected:
	std::list<Node*> children;
};

#endif