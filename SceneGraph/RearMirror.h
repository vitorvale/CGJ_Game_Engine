#pragma once
#include "Node.h"

class RearMirror : public Node {
public:
	RearMirror(MyMesh* m, Material* mat) {};
	RearMirror(void) : Node() {};
	~RearMirror(void) {};

	virtual void Update();
};