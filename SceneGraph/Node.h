#pragma once
#include <vector>

#include "../AVTmathLib.h"
#include "../geometry.h"

class Node {
public:
	Node(MyMesh* mesh, Material* mat);
	Node(void);
	~Node(void);

	//float* getWorldTransform() const { return worldTransform; }

	void AddChild(Node* child);

	virtual void Update() {};

	std::vector<Node*>::const_iterator GetChildIteratorStart() { return children.begin(); }
	std::vector<Node*>::const_iterator GetChildIteratorEnd() { return children.end(); }

	Node* parent;
	MyMesh* mesh;
	Material* material;

	float m_model[16];
protected:
	//float worldTransform[16];
	std::vector<Node*> children;
};