#include "Node.h"

Node::Node(MyMesh* m, Material* mat) {
	mesh = m;
	material = mat;
	parent = NULL;
	memset(m_model, 0.0f, sizeof(float) * 16);
}

Node::Node(void) {
	mesh = NULL;
	material = NULL;
	parent = NULL;
	memset(m_model, 0.0f, sizeof(float) * 16);
}

Node::~Node(void) {
	for (unsigned int i = 0; i < children.size(); ++i) {
		delete children[i];
	}
}

//void Node::Update() {
//	//root node
//	if (!parent) {
//		loadIdentity(MODEL);
//	}
//	else {
//		pushMatrix(MODEL);
//	}
//
//	translate(MODEL, translateParams[0], translateParams[1], translateParams[2]);
//	rotate(MODEL, rotateParams[0], rotateParams[1], rotateParams[2]);
//	scale(MODEL, scaleParams[0], scaleParams[1], scaleParams[2]);
//
//	for (vector<Node*>::iterator i = children.begin(); i != children.end(); ++i) {
//		(*i)->Update();
//	}
//
//	if (parent) {
//		popMatrix(MODEL);
//	}
//
//	//maybe bb here???
//}

void Node::AddChild(Node* child) {
	children.push_back(child);
	child->parent = this;
}