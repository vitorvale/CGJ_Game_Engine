#include "GameObject3D.h"

#ifndef GrassElement_H
#define GrassElement_H

class GrassElement : public GameObject3D
{
private:
    float direction[3] = { 0.0f, 0.0f, 0.0f };
    float size[3] = { 0.0f, 0.0f, 0.0f };
public:
    GrassElement(Material material_in, MyMesh* meshPtr, float startPosition[3], int latestMeshId, bool isCollisionEnabled_in, float width, float lenght, float height);
    GrassElement() = default;

    void Update() override;
    void Paused() override;

};

#endif
