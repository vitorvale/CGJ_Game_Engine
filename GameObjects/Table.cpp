#include "Table.h"
#define FLOAT_MAX_VALUE 99999999999999999999999999999999999.99
#define FLOAT_MIN_VALUE -99999999999999999999999999999999999.99

Table::Table(Material material_in, MyMesh* meshPtr, float startPosition[3], int latestMeshId, bool isCollisionEnabled_in, float width, float lenght, float height) : GameObject3D(material_in)
{
	
	std::copy(startPosition, startPosition + 3, position);
	std::copy(startPosition, startPosition + 3, startPos);
	
	meshId = latestMeshId;
	mesh = meshPtr;
	isEnabled = true;
	isCollisionEnabled = isCollisionEnabled_in;
	bb_type = GameObject3D::BB_TYPE::Cubic;
	type = GameObject3D::TYPE::Table;

	

	//printf("position[0] - [%f], position[2] - [%f]\n", position[0], position[2]);

	float sizeVar[] = { width, lenght, height };

	memcpy(size, sizeVar, 3 * sizeof(float));


	float scale_value[3] = {0.0f, 0.0f, 0.0f};
	scale_value[0] = size[0];
	scale_value[1] = size[1];
	scale_value[2] = size[2];
	scale_bb(scale_value);
	

	
}

//void Table::PrepareMeshMaterial()
//{
//
//	float amb[] = { 0.9f, 0.8f, 0.5f, 1.0f };
//	float diff[] = { 0.9f, 0.8f, 0.5f, 1.0f };
//	float spec[] = { 0.8f, 0.8f, 0.8f, 1.0f };
//	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
//	float shininess = 800.0f;
//	int texcount = 0;
//
//	memcpy(mesh->mat.ambient, amb, 4 * sizeof(float));
//	memcpy(mesh->mat.diffuse, diff, 4 * sizeof(float));
//	memcpy(mesh->mat.specular, spec, 4 * sizeof(float));
//	memcpy(mesh->mat.emissive, emissive, 4 * sizeof(float));
//	mesh->mat.shininess = shininess;
//	mesh->mat.texCount = texcount;
//
//	
//}

//void Table::PrepareMeshMaterial(Material m)
//{
//
//	memcpy(mesh->mat.ambient, m.ambient, 4 * sizeof(float));
//	memcpy(mesh->mat.diffuse, m.diffuse, 4 * sizeof(float));
//	memcpy(mesh->mat.specular, m.specular, 4 * sizeof(float));
//	memcpy(mesh->mat.emissive, m.emissive, 4 * sizeof(float));
//	mesh->mat.shininess = m.shininess;
//	mesh->mat.texCount = m.texCount;
//}

void Table::Update()
{

	MoveTable();
	float offset[3] = { 0.0f, 0.0f, 0.0f };

	//if (!isCollisionEnabled) {
	//	translate(MODEL, position[0] - table_length / 2, position[1], position[2] - table_length / 2);
	//}
	translate(MODEL, position[0], position[1], position[2]);
	subtract(old_position, position, offset);
	translate_bb(offset);
	std::copy(position, position + 3, old_position);


	scale(MODEL, size[0], size[1], size[2]);
	CalculateBoundingBox();
}

void Table::Paused()
{

	MoveTable();
	float offset[3] = { 0.0f, 0.0f, 0.0f };

	//if (!isCollisionEnabled) {
	//	translate(MODEL, position[0] - table_length / 2, position[1], position[2] - table_length / 2);
	//}
	translate(MODEL, position[0], position[1], position[2]);
	subtract(old_position, position, offset);
	translate_bb(offset);
	std::copy(position, position + 3, old_position);


	scale(MODEL, size[0], size[1], size[2]);
	CalculateBoundingBox();
}

void Table::translate_bb(float offset[3]) {

	for (int k = 0; k < 8; k++) {
		for (int i = 0; i < 3; i++) {
			bb[k][i] += offset[i];
		}
	}

}

void Table::scale_bb(float values[3])
{
	for (int k = 0; k < 8; k++) {
		for (int i = 0; i < 3; i++) {
			bb[k][i] *= values[i];
		}
	}
}

void Table::CalculateBoundingBox() {
	float min_x = FLOAT_MAX_VALUE, max_x = FLOAT_MIN_VALUE, min_y = FLOAT_MAX_VALUE, max_y = FLOAT_MIN_VALUE, min_z = FLOAT_MAX_VALUE
		, max_z = FLOAT_MIN_VALUE;

	for (int k = 0; k < 8; k++) {
		if (bb[k][0] < min_x) {
			min_x = bb[k][0];
		}

		if (bb[k][0] > max_x) {
			max_x = bb[k][0];
		}
		if (bb[k][1] < min_y) {
			min_y = bb[k][1];
		}
		if (bb[k][1] > max_y) {
			max_y = bb[k][1];
		}
		if (bb[k][2] < min_z) {
			min_z = bb[k][2];
		}
		if (bb[k][2] > max_z) {
			max_z = bb[k][2];
		}

	}

	bb_bounds[0] = min_x;
	bb_bounds[1] = max_x;
	bb_bounds[2] = min_y;
	bb_bounds[3] = max_y;
	bb_bounds[4] = min_z;
	bb_bounds[5] = max_z;
}

void Table::SetDirection(float* value)
{
	std::copy(value, value + 3, direction);
}

void Table::PrintBB()
{
	printf("{\n");
	for (int k = 0; k < 8; k++) {
		printf("{");
		for (int i = 0; i < 3; i++) {
			printf("[%f] ", bb[k][i]);
		}
		printf("}\n");
	}
	printf("}\n");
}

void Table::MoveTable()
{
	float velocity[3];
	if (speed + acceleration >= 0) {
		speed += acceleration;
	}
	else {
		speed = 0;
	}
	
	constProduct(TimeUtil::deltaTime * speed, direction, velocity);
	if (!length(velocity) == 0) {
		std::copy(position, position + 3, old_position);
		//printf("velocity[0] - [%f], velocity[1] - [%f], velocity[2] - [%f]\n", velocity[0], velocity[1], velocity[2]);
	}
	add(position, velocity, position);
}


bool Table::DetectCollisionWithObject(GameObject3D* other)
{
	return false;
}

void Table::ResolveCollisionWithObject(GameObject3D* other)
{
}

void Table::reset() {
	std::copy(startPos, startPos + 3, position);
}
