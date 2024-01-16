#include "Root.h"

Root::Root(MyMesh* m, Material* mat, float* pos, float angle) : Node(m, mat) {

	//std::copy(pos, pos + 3, position);
	memcpy(position, pos, sizeof(float) * 3);
	rotationAngle = angle;

	body = new Body();
	AddChild(body);

	top = new Top();
	body->AddChild(top);

	float tloffset[3] = { 0.8f, 0.0f, -0.02f };
	topLeft = new Wheel();
	memcpy(topLeft->offset, tloffset, sizeof(float) * 3);
	body->AddChild(topLeft);

	float troffset[3] = { 0.8f, 0.0f, 1.03f };
	topRight = new Wheel();
	memcpy(topRight->offset, troffset, sizeof(float) * 3);
	body->AddChild(topRight);

	float bloffset[3] = { 0.15f, 0.0f, -0.02f };
	botLeft = new Wheel();
	memcpy(botLeft->offset, bloffset, sizeof(float) * 3);
	body->AddChild(botLeft);

	float broffset[3] = { 0.15f, 0.0f, 1.03f };
	botRight = new Wheel();
	memcpy(botRight->offset, broffset, sizeof(float) * 3);
	body->AddChild(botRight);

	rearMirror = new RearMirror();
	body->AddChild(rearMirror);
}

void Root::Update() {
	pushMatrix(MODEL);
	
	translate(MODEL, position[0], position[1], position[2]);
	rotate(MODEL, -rotationAngle, 0.0f, 1.0f, 0.0f);

	float* model = get(MODEL);
	//std::copy(model, model + 16, m_model);
	memcpy(m_model, model, sizeof(float) * 16);

	for (std::vector<Node*>::iterator i = children.begin(); i != children.end(); ++i) {
		(*i)->Update();
	}

	popMatrix(MODEL);
}