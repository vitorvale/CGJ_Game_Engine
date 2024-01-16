#include "Body.h"

void Body::Update() {
	pushMatrix(MODEL);

	scale(MODEL, 2.0f, 0.5f, 1.0f);

	float* model = get(MODEL);
	//std::copy(model, model + 16, m_model);
	memcpy(m_model, model, sizeof(float) * 16);

	for (std::vector<Node*>::iterator i = children.begin(); i != children.end(); ++i) {
		(*i)->Update();
	}

	popMatrix(MODEL);
}