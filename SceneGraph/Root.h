#pragma once
#include "Body.h"
#include "Top.h"
#include "Wheel.h"
#include "RearMirror.h"

class Root : public Node {
public:
	Root(MyMesh* mesh, Material* mat, float* position, float angle);
	Root() : Node() {};
	~Root(void) {};

	virtual void Update();

	float position[3];
	float rotationAngle;

	Body* body;
	Top* top;
	Wheel* topLeft;
	Wheel* topRight;
	Wheel* botLeft;
	Wheel* botRight;
	RearMirror* rearMirror;
};