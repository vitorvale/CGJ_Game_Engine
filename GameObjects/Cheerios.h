#include "GameObject3D.h"

#ifndef Cheerios_H
#define Cheerios_H

class Cheerios : public GameObject3D
{
public:

    float size[3];
    float roation[3];
    float acceleration = -0.01f;
    float direction[3] = { 0.0f, 0.0f, 0.0f };
    float old_position[3] = { 0.0f, 0.0f, 0.0f };
    float startPos[3] = { 0.0f, 0.0f, 0.0f };
    float angle;
    float radious;

    
    Cheerios(Material material_in, MyMesh* meshPtr, float startPosition[3], int latestMeshId, float extRadious);
    Cheerios() = default;

    /*void PrepareMeshMaterial();
    void PrepareMeshMaterial(Material m);*/
    void Update() override;
    void Paused() override;
    void reset() override;
    void MoveCheerio(float direction[3]);
    float getRadius() { return radious; }
    void SetDirection(float* value);

    void MoveCheerio();

    bool DetectCollisionWithObject(GameObject3D* other);
    void ResolveCollisionWithObject(GameObject3D* other);
};

#endif