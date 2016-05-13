
#include "Geode.h"
class Camera :
	public Geode
{
public:
	Camera();
	~Camera();

	glm::mat4 getToWorld();
	void update(glm::mat4);
	void draw(GLuint);
private:
	glm::mat4 toWorld;
};

