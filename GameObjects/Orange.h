#include "GameObject3D.h"
#include "TimeUtil.h"
#include <string>
#include <iostream>

#ifndef ORANGE_H
#define ORANGE_H

class Orange: public GameObject3D
{
private:
    float radius;
    bool onTable; 
    float direction[3];
    float rotationAngle;
    float rotationSpeed;
    float table_length;
    float *table_position;
    double int_time_updated;
    double int_time_respawn;
    float max_speed = 5;
    float startSpeed = 0;
    float bb_bounds[6] = { 0.0f, 120.0f, -4.0f, -2.0f, 0.0f, 120.0f };
    float startPos[3] = { 0.0f, 0.0f, 0.0f };
public:
    Orange(Material material_in, MyMesh* meshPtr, float radius, int latestMeshId, float table_position[3], float table_length, bool isCollisionEnabled_in);
    Orange() = default;

    /*void PrepareMeshMaterial();*/
    void Update();
    void Paused();
    void reset();

    bool isOnTable();
    void StoreBounds(float* table_bounds);
    int setOnTable(bool newOnTable) { onTable = newOnTable; }

    float getSpeed() { return speed; }
    float getRadius() { return radius; }
    float setSpeed(float newSpeed) { speed = newSpeed; }

    void MoveOrange();
    void SetDirection(float newDirection[3]);
    void RespawnOrange();
    bool DetectCollisionWithObject(GameObject3D* other);
    void ResolveCollisionWithObject(GameObject3D* other);
};

#endif