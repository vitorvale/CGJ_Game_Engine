#include <GL/glew.h>

#include "../geometry.h"
#include "../TimeUtil.h"

#include "../AVTmathLib.h"
#include <cstring> // memcpy????

#ifndef GameObject3D_H
#define GameObject3D_H

class GameObject3D {
public:
	enum class BB_TYPE { Spherical, Cubic };
	enum class TYPE { Orange, Cheerio, Table, Billboard, Grass };
protected:
	int objectId;
	float speed = 0.0f;
	bool isCollisionEnabled = false;
	float position[3];
	MyMesh* mesh;
	bool isEnabled;
	int meshId; // id in MicroMachines3D.cpp myMeshes vector
	BB_TYPE bb_type;
	TYPE type;
	Material material;

public:
	virtual void Update() { }
	virtual void Paused() { }
	GameObject3D(Material material_in) { material = material_in; }
	GameObject3D() = default;
	MyMesh* GetMesh() { return mesh; }
	int GetMeshId() { return meshId; }
	void SetSpeed(float value) { speed = value; }
	BB_TYPE GetBBType() { return bb_type; }
	TYPE GetType() { return type; }
	bool GetIsEnabled() { return isEnabled; }
	bool GetIsCollisionEnabled() { return isCollisionEnabled; }

	bool DetectCollisionWithObject(GameObject3D* other){ return false;}
	void ResolveCollisionWithObject(GameObject3D* other){ }

	virtual void reset() { }

	float* GetPosition() { return position; }
	
	void PrepareMeshMaterial()
	{

		memcpy(mesh->mat.ambient, material.ambient, 4 * sizeof(float));
		memcpy(mesh->mat.diffuse, material.diffuse, 4 * sizeof(float));
		memcpy(mesh->mat.specular, material.specular, 4 * sizeof(float));
		memcpy(mesh->mat.emissive, material.emissive, 4 * sizeof(float));
		mesh->mat.shininess = material.shininess;
		mesh->mat.texCount = material.texCount;
		memcpy(mesh->mat.albedo, material.albedo, 4 * sizeof(float));
		mesh->mat.metallic = material.metallic;
		mesh->mat.roughness = material.roughness;
		mesh->mat.ao = material.ao;

	}


};

#endif // !GameObject3D_H
