#pragma once
#include "Node.h"

class Wheel : public Node {
public:
	Wheel(MyMesh* m, Material* mat, float angle, float* offset);
	Wheel(void) : Node() {};
	~Wheel(void) {};

	virtual void Update();

	float angle;
	float offset[3];
};