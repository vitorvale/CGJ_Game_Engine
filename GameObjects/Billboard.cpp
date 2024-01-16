#include "Billboard.h"
#define FLOAT_MAX_VALUE 99999999999999999999999999999999999.99
#define FLOAT_MIN_VALUE -1.0

Billboard::Billboard(Material material_in, MyMesh* meshPtr, float startPosition[3], int latestMeshId, bool isCollisionEnabled_in, float width, float lenght, float height) : GameObject3D(material_in)
{

	std::copy(startPosition, startPosition + 3, position);

	meshId = latestMeshId;
	mesh = meshPtr;
	isEnabled = true;
	isCollisionEnabled = isCollisionEnabled_in;
	//bb_type = GameObject3D::BB_TYPE::Cubic;
	type = GameObject3D::TYPE::Billboard;



	//printf("position[0] - [%f], position[2] - [%f]\n", position[0], position[2]);

	float sizeVar[] = { width, lenght, height };

	memcpy(size, sizeVar, 3 * sizeof(float));



}

void Billboard::Update()
{

	translate(MODEL, position[0], position[1], position[2]);

	scale(MODEL, size[0], size[1], size[2]);

}

void Billboard::Paused()
{

	translate(MODEL, position[0], position[1], position[2]);

	scale(MODEL, size[0], size[1], size[2]);

}

void Billboard::orientateBillboard(float* cam, int type)
{
	/*pushMatrix(MODEL);
	translate(MODEL, 0.0, 3.0, 0.0f);*/

	if (type == 2)
		l3dBillboardSphericalBegin(cam, position);
	else if (type == 3)
		l3dBillboardCylindricalBegin(cam, position);

	if (type == 0 || type == 1) {     //Cheating matrix reset billboard techniques
		computeDerivedMatrix(VIEW_MODEL);

		//reset VIEW_MODEL
		if (type == 0) BillboardCheatSphericalBegin();
		else BillboardCheatCylindricalBegin();

		computeDerivedMatrix_PVM(); // calculate PROJ_VIEW_MODEL
	}
	else computeDerivedMatrix(PROJ_VIEW_MODEL);

}
