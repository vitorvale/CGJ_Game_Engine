#include "Top.h"

void Top::Update() {
	pushMatrix(MODEL);

	translate(MODEL, 0.0f, 1.0f, 0.0f);
	scale(MODEL, 0.6f, 0.4f, 1.0f);

	float* model = get(MODEL);
	//std::copy(model, model + 16, m_model);
	memcpy(m_model, model, sizeof(float) * 16);

	popMatrix(MODEL);
}
