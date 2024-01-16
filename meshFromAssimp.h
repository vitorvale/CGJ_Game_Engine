#pragma once
#include <assert.h>
#include <stdlib.h>
#include <unordered_map>

// assimp include files. These three are usually needed.
#include "assimp/Importer.hpp"	
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#include "AVTmathLib.h"
#include "VertexAttrDef.h"
#include "geometry.h"
#include "Texture_Loader.h"

bool Import3DFromFile(int index, const std::string& pFile);
std::vector<struct MyMesh> createMeshFromAssimp(const aiScene* sc, bool loadTextures);