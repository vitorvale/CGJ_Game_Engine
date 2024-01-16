#pragma once
#include "Node.h"

class Body : public Node {
public:
	Body(MyMesh* m, Material* mat);
	Body(void) : Node(){};
	~Body(void) {};

	virtual void Update();
};