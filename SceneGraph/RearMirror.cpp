#include "RearMirror.h"

void RearMirror::Update() {
	pushMatrix(MODEL);

	translate(MODEL, 1.2, 1.0f, 0.5f);
	rotate(MODEL, -90, 0.0f, 1.0f, 0.0f);
	scale(MODEL, 0.5f, 0.4f, 1.0f);

	float* model = get(MODEL);
	memcpy(m_model, model, sizeof(float) * 16);

	popMatrix(MODEL);
}
