#pragma once
#include "Node.h"

class Top : public Node {
public:
	Top(MyMesh* m, Material* mat) {};
	Top(void) : Node() {};
	~Top(void) {};

	virtual void Update();
};