#include "Wheel.h"

Wheel::Wheel(MyMesh* m, Material* mat, float angle, float* offset) : Node(m, mat) {
	this->angle = angle;
	//std::copy(offset, offset + 3, this->offset);
	memcpy(this->offset, offset, sizeof(float) * 3);
}

void Wheel::Update() {
	pushMatrix(MODEL);

	translate(MODEL, offset[0], offset[1], offset[2]);

	scale(MODEL, 0.5f, 2.0f, 1.0f);
	rotate(MODEL, -90, 1.0f, 0.0f, 0.0f);
	scale(MODEL, 0.3f, 0.3f, 0.3f);

	rotate(MODEL, angle, 0.0f, 1.0f, 0.0f);

	float* model = get(MODEL);
	//std::copy(model, model + 16, m_model);
	memcpy(m_model, model, sizeof(float) * 16);

	popMatrix(MODEL);
}