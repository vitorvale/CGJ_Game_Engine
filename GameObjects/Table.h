#include "GameObject3D.h"

#ifndef Table_H
#define Table_H

class Table : public GameObject3D
{
private: 
    float table_length;
	float acceleration = -0.01f;
	float direction[3] = { 0.0f, 0.0f, 0.0f };
	float old_position[3] = { 0.0f, 0.0f, 0.0f };
	float startPos[3] = { 0.0f, 0.0f, 0.0f };
	float bb[8][3] = { {1.0f, 0.0f, 0.0f}
					,{ 0.0f, 0.0f, 1.0f}
					,{ 1.0f, 0.0f, 1.0f}
					,{ 0.0f, 0.0f, 0.0f}
					,{ 1.0f, 1.0f, 1.0f}
					,{ 1.0f, 1.0f, 0.0f}
					,{ 0.0f, 1.0f, 1.0f}
					,{ 0.0f, 1.0f, 0.0f} };
	float size[3];

	float bb_bounds[6] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
public:
    Table(Material material_in, MyMesh* meshPtr, float startPosition[3], int latestMeshId, bool isCollisionEnabled_in, float width, float lenght, float height);
    Table() = default;

    /*void PrepareMeshMaterial();
	void PrepareMeshMaterial(Material m);*/
    void Update() override;
	void Paused() override;
	void reset() override;

	void translate_bb(float offset[3]);

	void scale_bb(float values[3]);
	float* GetBBBounds() { return bb_bounds; }
	void SetDirection(float* value);
	void PrintBB();
	void MoveTable();

	void CalculateBoundingBox();
	bool DetectCollisionWithObject(GameObject3D* other);
	void ResolveCollisionWithObject(GameObject3D* other);
};

#endif