#include "GameObject3D.h"
#include "../l3DBillboard.h"

#ifndef Billboard_H
#define Billboard_H

class Billboard : public GameObject3D
{
private:
    float direction[3] = { 0.0f, 0.0f, 0.0f };
    float size[3] = { 0.0f, 0.0f, 0.0f };
public:
    Billboard(Material material_in, MyMesh* meshPtr, float startPosition[3], int latestMeshId, bool isCollisionEnabled_in, float width, float lenght, float height);
    Billboard() = default;

    void Update() override;
    void Paused() override;
    void orientateBillboard(float* cam, int type);

};

#endif