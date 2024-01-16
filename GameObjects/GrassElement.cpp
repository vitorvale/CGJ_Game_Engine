#include "GrassElement.h"
#define FLOAT_MAX_VALUE 99999999999999999999999999999999999.99
#define FLOAT_MIN_VALUE -1.0

GrassElement::GrassElement(Material material_in, MyMesh* meshPtr, float startPosition[3], int latestMeshId, bool isCollisionEnabled_in, float width, float lenght, float height) : GameObject3D(material_in)
{

	std::copy(startPosition, startPosition + 3, position);

	meshId = latestMeshId;
	mesh = meshPtr;
	isEnabled = true;
	isCollisionEnabled = isCollisionEnabled_in;
	//bb_type = GameObject3D::BB_TYPE::Cubic;
	type = GameObject3D::TYPE::Grass;



	//printf("position[0] - [%f], position[2] - [%f]\n", position[0], position[2]);

	float sizeVar[] = { width, lenght, height };

	memcpy(size, sizeVar, 3 * sizeof(float));



}

void GrassElement::Update()
{

	translate(MODEL, position[0], position[1], position[2]);

	scale(MODEL, size[0], size[1], size[2]);

}

void GrassElement::Paused()
{

	translate(MODEL, position[0], position[1], position[2]);

	scale(MODEL, size[0], size[1], size[2]);

}
