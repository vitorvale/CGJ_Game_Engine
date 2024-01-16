//
// Based on AVT template demo light lightDemo.cpp
//
#pragma comment(lib, "DevIL.lib")
#pragma comment(lib, "ILU.lib")

#include <math.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

// include GLEW to access OpenGL 3.3 functions
#include <GL/glew.h>

// GLUT is the toolkit to interface with the OS
#include <GL/freeglut.h>

#include <IL/il.h>

// Use Very Simple Libs
#include "VSShaderlib.h"
#include "AVTmathLib.h"
#include "VertexAttrDef.h"
#include "Texture_Loader.h"
#include "avtFreeType.h"
#include "flare.h"
#include "l3DBillboard.h"
#include "meshFromAssimp.h"
#include "assimp/include/assimp/Importer.hpp"	//OO version Header!
#include "assimp/include/assimp/scene.h"

// Custom headers
#include "TimeUtil.h" // included x2??
#include "GameObjects/Billboard.h"
#include "GameObjects/GrassElement.h"
#include "GameObjects/Orange.h"
#include "GameObjects/Table.h"
#include "GameObjects/Car.h"
#include "GameObjects/Cheerios.h"
#include "Camera/PerspectiveCamera.h"
#include "Camera/OrthographicCamera.h"
#include "Lights/PointLight.h"
#include "Lights/DirectionalLight.h"
#include "GameHudManager.h"
#define MAX_LIGHTS 9

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
GLuint terrainVAO, terrainVBO, terrainEBO;
unsigned int NUM_STRIPS;
unsigned int NUM_VERTS_PER_STRIP;

using namespace std;

#define CAPTION "CGJ Assignment 1"
int WindowHandle = 0;
int WinX = 1280, WinY = 720;

unsigned int FrameCount = 0;
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad();

const unsigned int SCR_WIDTH = 640;
const unsigned int SCR_HEIGHT = 480;

unsigned int hdrFBO;
unsigned int shakeFBO;
unsigned int shakeColorBuffer[1];
unsigned int colorBuffers[2];
unsigned int rbo;
unsigned int pingpongFBO[2];
unsigned int pingpongColorbuffers[2];

bool bloom = true;
bool bloomKeyPressed = false;
float exposure = 1.0f;

VSShaderLib shader, shaderBlur, shaderBloomFinal, shaderText, shaderHDR, shaderShake;

int pbr = 1;


vector<GameObject3D*> gameObjectsRef;
unsigned int instanceXVBO, instanceZVBO;

vector<Camera> cameras;
vector<Camera*> camerasRef;
Camera* activeCameraRef;
int activeCameraId; // FIXME refactor

//External array storage defined in AVTmathLib.cpp

/// The storage for matrices
extern float mMatrix[COUNT_MATRICES][16];
extern float mCompMatrix[COUNT_COMPUTED_MATRICES][16];

/// The normal matrix
extern float mNormal3x3[9];
GLfloat translationsX[200];
GLfloat translationsZ[200];

GLint pvm_uniformId;
GLint vm_uniformId;
GLint model_uniformId;
GLint normal_uniformId;
GLint view_uniformId;
GLint lPos_uniformId;
GLint tex_loc, tex_loc1, tex_loc2, tex_loc3, tex_normalMap_loc, tex_cube_loc, texLoc, texLoc1, texLoc2, texLoc3, texLoc4, texLoc5;
GLint texMode_uniformId, shadowMode_uniformId, blurMode, grassMode_uniformId, time_uniformId, timeShake_uniformId, shaking_uniformId, terrainMode_uniformId;
GLint toggleFog_uniformId;
GLint albedo_uniformId, metallic_uniformId, roughness_uniformId, ao_uniformId, camPos_uniformId, togglePbr_uniformId;
int toggleFog;

GLint useNormalMap_loc, specularMap_loc, diffMapCount_loc;

GLuint TextureArray[7];

GLuint FlareTextureArray[5];
//Flare effect
FLARE_DEF AVTflare;
float lightScreenPos[3];  //Position of the light in Window Coordinates
bool flareEffect;
float flareLightPos[4] = { 20.0f, 10.0f, 0.0f, 1.0f };

// Camera Position
float camX, camY, camZ;

// Mouse Tracking Variables
int startX, startY, tracking = 0;

// Camera Spherical Coordinates
float alpha = 0.0f, beta = 90.0f;
float deltaAlpha = 0.0f, deltaBeta = 0.0f;
float r = 10.0f;


float lightPos[4] = {40.0f, 50.0f, 0.0f, 1.0f };

float position[4] = { 20.0f, 10.0f, 0.0f, 1.0f };
float position1[4] = { 20.0f, 10.0f, 75.0f, 1.0f };
float position2[4] = { 75.0f, 10.0f, 80.0f, 1.0f };
float position3[4] = { 140.0f, 10.0f, 75.0f, 1.0f };
float position4[4] = { 140.0f, 10.0f, 0.0f, 1.0f };
float position5[4] = { 75.0f, 10.0f, -75.0f, 1.0f };


float checkpoint1[6] = { -10.0f, 30.0f, -10.0f, 10.0f, 70.0f, 110.0f};
float checkpoint2[6] = { 130.0f, 170.0f, -10.0f, 10.0f, 70.0f, 110.0f };
float checkpoint3[6] = { 130.0f, 170.0f, -10.0f, 10.0f, -100.0f, -60.0f };
float checkpoint4[6] = { -10.0f, 30.0f, -10.0f, 10.0f, -100.0f, -60.0f };

bool c1, c2, c3, c4;
bool shaking = false;
float shakeTime = 0.0f;

// Frame counting and FPS computation
long myTime, timebase = 0, frame = 0;
float timeCounter = 0;
char s[32];

// Mesh primitives
MyMesh cubeMesh;
MyMesh sphereMesh;
MyMesh torusMesh;
MyMesh billboardMesh;
MyMesh quadMesh;
MyMesh coneMesh;

// Created an instance of the Importer class in the meshFromAssimp.cpp file
extern Assimp::Importer importer;
// the global Assimp scene object
extern const aiScene* scene;
extern const aiScene* backScene;
vector<MyMesh> backpackMesh;
vector<MyMesh> lampMesh;
char model_dir[50];
// scale factor for the Assimp model to fit in the window
//extern float scaleFactor;

// global game objects
Orange o1;
float o1Pos[3] = { 1.0f, -2.0f, 10.0f };
Orange o2, o3, o4, o5, o6, o7, o8, o9, o10, o11, o12, o13, o14, o15;
Table t, skybox;
int bumpmap = 0;
Billboard b1, b2, b3, b4, b5, b6, b7, b8, b9, b10, b11, b12;
int billboardType = 0;
GrassElement g1, g2, g3, g4, g5, g6, g7, g8, g9, g10, g11, g12, g13, g14, g15, g16, g;
Cheerios t111, t112, t113, t114, t115, t116, t117, t118, t119, t120, t121, t122, t123;
Cheerios t211, t212, t213, t214, t215, t216, t217, t218, t219, t220, t221, t222, t223;
Cheerios t311, t312, t313, t314, t315, t316, t317, t318, t319, t320, t321, t322, t323;
Cheerios t411, t412, t413, t414, t415, t416, t417, t418, t419, t420, t421, t422, t423;
Cheerios t511, t512, t513, t514, t515, t516, t517, t518, t519, t520, t521, t522, t523;
Cheerios t611, t612, t613, t614, t615, t616, t617, t618, t619, t620, t621, t622, t623;
Cheerios t711, t712, t713, t714, t715, t716, t717, t718, t719, t720, t721, t722, t723;
Cheerios t811, t812, t813, t814, t815, t816, t817, t818, t819, t820, t821, t822, t823;
Cheerios t911, t912, t913, t914, t915, t916, t917, t918, t919, t920, t921, t922, t923;
Cheerios t1011, t1012, t1013, t1014, t1015, t1016, t1017, t1018, t1019, t1020, t1021, t1022, t1023;
Cheerios t1111, t1112, t1113, t1114, t1115, t1116, t1117, t1118, t1119, t1120, t1121, t1122, t1123;
Cheerios t1211, t1212, t1213, t1214, t1215, t1216, t1217, t1218, t1219, t1220, t1221, t1222, t1223;
Table t2, t21, t22, t23, t24, t25, t26, t27, t28;//Road
Table butter1, butter2, butter3, butter4, butter5, butter6, butter7, butter99;
float tPos[3] = { -30.0f, 0.0f, -120.0f };
float table_length = 250.0f;
Car car;

PointLight pl1, pl2, pl3, pl4, pl5, pl6;
DirectionalLight dl;
LightProperties* lights[MAX_LIGHTS];

PerspectiveCamera pCam1;
PerspectiveCamera pCam2;
PerspectiveCamera rearviewCam;
PerspectiveCamera rearviewMirrorCam;
OrthographicCamera oCam;
bool flag = true;

GameHudManager gameManager;

bool rearViewFlag = false;

#define frand()			((float)rand()/RAND_MAX)
#define M_PI			3.14159265
#define MAX_PARTICULAS  1500
typedef struct {
	float	life;		// vida
	float	fade;		// fade
	float	r, g, b;    // color
	GLfloat x, y, z;    // posição
	GLfloat vx, vy, vz; // velocidade 
	GLfloat ax, ay, az; // aceleração
} Particle;
Particle particula[MAX_PARTICULAS];
int dead_num_particles = 0;
bool particlesActive;
bool restart = false;


inline double clamp(const double x, const double min, const double max) {
	return (x < min ? min : (x > max ? max : x));
}

inline int clampi(const int x, const int min, const int max) {
	return (x < min ? min : (x > max ? max : x));
}


bool pause = false;

void timer(int value)
{
	std::ostringstream oss;
	oss << CAPTION << ": " << FrameCount << " FPS @ (" << WinX << "x" << WinY << ")";
	std::string s = oss.str();
	glutSetWindow(WindowHandle);
	glutSetWindowTitle(s.c_str());
	FrameCount = 0;
	glutTimerFunc(1000, timer, 0);
}

void refresh(int value)
{
	//PUT YOUR CODE HERE
}

// ------------------------------------------------------------
//
// Reshape Callback Function
//
void resizeCallbackWrapper(int w, int h) {
	activeCameraRef->ResizeWindow(w, h);
}

void LoadCamera(int camId)
{
	activeCameraId = camId;
	activeCameraRef = camerasRef[camId];
	activeCameraRef->SetProjection();
}

void iniParticles(void)
{
	GLfloat v, theta, phi;
	int i;

	for (i = 0; i < MAX_PARTICULAS; i++)
	{
		v = 0.8 * frand() + 0.2;
		phi = frand() * M_PI;
		theta = 2.0 * frand() * M_PI;

		particula[i].x = 0.0f;
		particula[i].y = 10.0f;
		particula[i].z = 0.0f;
		particula[i].vx = v * cos(theta) * sin(phi);
		particula[i].vy = v * cos(phi);
		particula[i].vz = v * sin(theta) * sin(phi);
		particula[i].ax = 0.1f; /* simular um pouco de vento */
		particula[i].ay = -0.15f; /* simular a aceleraÁ„o da gravidade */
		particula[i].az = 0.0f;

		/* tom amarelado que vai ser multiplicado pela textura que varia entre branco e preto */
		particula[i].r = 0.882f;
		particula[i].g = 0.552f;
		particula[i].b = 0.211f;

		particula[i].life = 1.0f;		/* vida inicial */
		particula[i].fade = 0.0025f;	    /* step de decrÈscimo da vida para cada iteraÁ„o */
	}
}


// ------------------------------------------------------------
//
// Render stufff
//
void render_flare(FLARE_DEF* flare, int lx, int ly, int* m_viewport) {  //lx, ly represent the projected position of light on viewport

	int     dx, dy;          // Screen coordinates of "destination"
	int     px, py;          // Screen coordinates of flare element
	int		cx, cy;
	float    maxflaredist, flaredist, flaremaxsize, flarescale, scaleDistance;
	int     width, height, alpha;    // Piece parameters;
	int     i;
	float	diffuse[4];

	GLint loc;

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int screenMaxCoordX = m_viewport[0] + m_viewport[2] - 1;
	int screenMaxCoordY = m_viewport[1] + m_viewport[3] - 1;

	//viewport center
	cx = m_viewport[0] + (int)(0.5f * (float)m_viewport[2]) - 1;
	cy = m_viewport[1] + (int)(0.5f * (float)m_viewport[3]) - 1;

	// Compute how far off-center the flare source is.
	maxflaredist = sqrt(cx * cx + cy * cy);
	flaredist = sqrt((lx - cx) * (lx - cx) + (ly - cy) * (ly - cy));
	scaleDistance = (maxflaredist - flaredist) / maxflaredist;
	flaremaxsize = (int)(m_viewport[2] * flare->fMaxSize);
	flarescale = (int)(m_viewport[2] * flare->fScale);

	// Destination is opposite side of centre from source
	dx = clampi(cx + (cx - lx), m_viewport[0], screenMaxCoordX);
	dy = clampi(cy + (cy - ly), m_viewport[1], screenMaxCoordY);

	// Render each element. To be used Texture Unit 0

	glUniform1i(texMode_uniformId, 4); // draw modulated textured particles 
	glUniform1i(tex_loc3, 0);  //use TU 3

	for (i = 0; i < flare->nPieces; ++i)
	{
		// Position is interpolated along line between start and destination.
		px = (int)((1.0f - flare->element[i].fDistance) * lx + flare->element[i].fDistance * dx);
		py = (int)((1.0f - flare->element[i].fDistance) * ly + flare->element[i].fDistance * dy);
		px = clampi(px, m_viewport[0], screenMaxCoordX);
		py = clampi(py, m_viewport[1], screenMaxCoordY);

		// Piece size are 0 to 1; flare size is proportion of screen width; scale by flaredist/maxflaredist.
		width = (int)(scaleDistance * flarescale * flare->element[i].fSize);

		// Width gets clamped, to allows the off-axis flaresto keep a good size without letting the elements get big when centered.
		if (width > flaremaxsize)  width = flaremaxsize;

		height = (int)((float)m_viewport[3] / (float)m_viewport[2] * (float)width);
		memcpy(diffuse, flare->element[i].matDiffuse, 4 * sizeof(float));
		diffuse[3] *= scaleDistance;   //scale the alpha channel

		if (width > 1)
		{
			// send the material - diffuse color modulated with texture
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
			glUniform4fv(loc, 1, diffuse);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, FlareTextureArray[flare->element[i].textureId]);
			pushMatrix(MODEL);
			translate(MODEL, (float)(px - width * 0.0f), (float)(py - height * 0.0f), 0.0f);
			scale(MODEL, (float)width, (float)height, 1);
			computeDerivedMatrix(PROJ_VIEW_MODEL);
			glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
			glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
			computeNormalMatrix3x3();
			glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

			glBindVertexArray(quadMesh.vao);
			glDrawElements(quadMesh.type, quadMesh.numIndexes, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			popMatrix(MODEL);
		}
	}
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
}


// FIXME location should be saved per gameobject or globally(if only 1 shader)
void SendMeshMaterial(MyMesh* mesh, int billboardType)
{
	GLint loc;
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
	glUniform4fv(loc, 1, mesh->mat.ambient);
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
	glUniform4fv(loc, 1, mesh->mat.diffuse);
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
	glUniform4fv(loc, 1, mesh->mat.specular);
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
	glUniform1f(loc, mesh->mat.shininess);
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.albedo");
	glUniform4fv(loc, 1, mesh->mat.albedo);
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.metallic");
	glUniform1f(loc, mesh->mat.metallic);
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.roughness");
	glUniform1f(loc, mesh->mat.roughness);
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ao");
	glUniform1f(loc, mesh->mat.ao);
}

void SendLights(LightProperties* lProps, int lightId)
{
	//printf("pos: %f %f %f \n", lProps->position[0], lProps->position[1], lProps->position[2]);
	//printf("dir: %f %f %f \n", lProps->coneDirection[0], lProps->coneDirection[1], lProps->coneDirection[2]);

	float lightPos[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	float coneDir[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	if (lProps->isSpot == 0)
	{
		//if (lProps->isLocal == 0) 
			//printf("dir BEFORE VIEW: %f %f %f %f\n", lProps->position[0], lProps->position[1], lProps->position[2], lProps->position[3]);

		multMatrixPoint(VIEW, lProps->position, lightPos);

		if (lProps->isLocal == 0) {
			normalize(lightPos);
		}
			
	}
	else
	{
		multMatrixPoint(VIEW, lProps->position, lightPos);
        multMatrixPoint(VIEW, lProps->coneDirection, coneDir);

	}

	//When the type of internal state is boolean, zero integer or floating-point values 
	//are converted to FALSE and non-zero values are converted to TRUE.
	std::string lId = std::to_string(lightId);
	std::string isEnabled = "Lights[" + lId + "].isEnabled";
	std::string isLocal = "Lights[" + lId + "].isLocal";
	std::string isSpot = "Lights[" + lId + "].isSpot";
	std::string position = "Lights[" + lId + "].position";
	std::string halfVector = "Lights[" + lId + "].halfVector";
	std::string coneDirection = "Lights[" + lId + "].coneDirection";
	std::string spotCosCutoff = "Lights[" + lId + "].spotCosCutoff";
	std::string spotExponent = "Lights[" + lId + "].spotExponent";

	GLint loc;
	loc = glGetUniformLocation(shader.getProgramIndex(), isEnabled.c_str());
	glUniform1i(loc, lProps->isEnabled);
	loc = glGetUniformLocation(shader.getProgramIndex(), isLocal.c_str());
	glUniform1i(loc, lProps->isLocal);
	loc = glGetUniformLocation(shader.getProgramIndex(), isSpot.c_str());
	glUniform1i(loc, lProps->isSpot);
	loc = glGetUniformLocation(shader.getProgramIndex(), position.c_str());
	glUniform4fv(loc, 1, lightPos);
	loc = glGetUniformLocation(shader.getProgramIndex(), halfVector.c_str());
	glUniform3fv(loc, 1, lProps->halfVector);
	loc = glGetUniformLocation(shader.getProgramIndex(), coneDirection.c_str());
	glUniform4fv(loc, 1, coneDir);
	loc = glGetUniformLocation(shader.getProgramIndex(), spotCosCutoff.c_str());
	glUniform1f(loc, lProps->spotCosCutoff);
	loc = glGetUniformLocation(shader.getProgramIndex(), spotExponent.c_str());
	glUniform1f(loc, lProps->spotExponent);

	glUniform4fv(lPos_uniformId, 1, lightPos); //efeito capacete do mineiro, ou seja lighPos foi definido em eye coord 
}


// Recursive render of the Assimp Scene Graph

void aiRecursive_render(const aiScene* sc, const aiNode* nd, vector<MyMesh> mesh)
{
	GLint loc;

	// Get node transformation matrix
	aiMatrix4x4 m = nd->mTransformation;
	// OpenGL matrices are column major
	m.Transpose();

	// save model matrix and apply node transformation
	pushMatrix(MODEL);

	float aux[16];
	memcpy(aux, &m, sizeof(float) * 16);
	multMatrix(MODEL, aux);


	// draw all meshes assigned to this node
	for (unsigned int n = 0; n < nd->mNumMeshes; ++n) {


		// send the material
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
		glUniform4fv(loc, 1, mesh[nd->mMeshes[n]].mat.ambient);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
		glUniform4fv(loc, 1, mesh[nd->mMeshes[n]].mat.diffuse);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
		glUniform4fv(loc, 1, mesh[nd->mMeshes[n]].mat.specular);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.emissive");
		glUniform4fv(loc, 1, mesh[nd->mMeshes[n]].mat.emissive);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
		glUniform1f(loc, mesh[nd->mMeshes[n]].mat.shininess);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.texCount");
		glUniform1i(loc, mesh[nd->mMeshes[n]].mat.texCount);

		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.albedo");
		glUniform4fv(loc, 1, mesh[nd->mMeshes[n]].mat.albedo);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.metallic");
		glUniform1f(loc, mesh[nd->mMeshes[n]].mat.metallic);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.roughness");
		glUniform1f(loc, mesh[nd->mMeshes[n]].mat.roughness);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ao");
		glUniform1f(loc, 1.0f);

		int  diffMapCount = 0;  //read 2 diffuse textures

		//devido ao fragment shader suporta 2 texturas difusas simultaneas, 1 especular e 1 normal map


		glUniform1i(useNormalMap_loc, false); //GLSL normalMap variable initialized to 0
		glUniform1i(specularMap_loc, false);
		glUniform1ui(diffMapCount_loc, 0);
		glUniform1i(texMode_uniformId, 9);

		if (mesh[nd->mMeshes[n]].mat.texCount != 0)
			for (unsigned int i = 0; i < mesh[nd->mMeshes[n]].mat.texCount; ++i) {
				if (mesh[nd->mMeshes[n]].texTypes[i] == DIFFUSE) {
					if (diffMapCount == 0) {
						diffMapCount++;
						glUniform1i(diffMapCount_loc, diffMapCount);
						glUniform1i(tex_loc, mesh[nd->mMeshes[n]].texUnits[i] + 6);
					}
					else if (diffMapCount == 1) {
						diffMapCount++;
						glUniform1i(diffMapCount_loc, diffMapCount);
						glUniform1i(tex_loc1, mesh[nd->mMeshes[n]].texUnits[i] + 6);
					}
					else printf("Only supports a Material with a maximum of 2 diffuse textures\n");
				}
				else if (mesh[nd->mMeshes[n]].texTypes[i] == SPECULAR) {
					glUniform1i(tex_loc2, mesh[nd->mMeshes[n]].texUnits[i] + 6);
					glUniform1i(specularMap_loc, false);
				}
				else if (mesh[nd->mMeshes[n]].texTypes[i] == NORMALS) { //Normal map
					glUniform1i(useNormalMap_loc, false);
					glUniform1i(tex_normalMap_loc, mesh[nd->mMeshes[n]].texUnits[i] + 6);
				}
				else printf("Texture Map not supported\n");
			}

		// send matrices to OGL
		computeDerivedMatrix(PROJ_VIEW_MODEL);
		glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
		glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
		computeNormalMatrix3x3();
		glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

		// bind VAO
		glBindVertexArray(mesh[nd->mMeshes[n]].vao);

		if (!shader.isProgramValid()) {
			printf("Program Not Valid!\n");
			exit(1);
		}
		// draw
		glDrawElements(mesh[nd->mMeshes[n]].type, mesh[nd->mMeshes[n]].numIndexes, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	// draw all children
	for (unsigned int n = 0; n < nd->mNumChildren; ++n) {
		aiRecursive_render(sc, nd->mChildren[n], mesh);
	}
	popMatrix(MODEL);

	glUniform1i(useNormalMap_loc, false);
}

void restartObjects() {

	for (int objId = 8; objId < gameObjectsRef.size() - 1; objId++) {
		pushMatrix(MODEL);
		gameObjectsRef[objId]->reset();
		popMatrix(MODEL);
	}
}

void RenderMesh(MyMesh* mesh)
{
	// send matrices to OGL
	glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
	glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
	computeNormalMatrix3x3();
	glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

	// Render mesh
	glBindVertexArray(mesh->vao);

	if (!shader.isProgramValid()) {
		printf("Program Not Valid!\n");
		exit(1);
	}
	glDrawElements(mesh->type, mesh->numIndexes, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void RenderInstances(MyMesh* mesh)
{
	// send matrices to OGL
	glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
	glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
	computeNormalMatrix3x3();
	glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

	// Render mesh
	glBindVertexArray(mesh->vao);

	if (!shader.isProgramValid()) {
		printf("Program Not Valid!\n");
		exit(1);
	}
	glUniform1f(time_uniformId, timeCounter);
	glDrawElementsInstanced(mesh->type, mesh->numIndexes, GL_UNSIGNED_INT, 0, 100);
	glBindVertexArray(0);
}

void UpdateCarMeshes()
{
	car.Update();

	pushMatrix(MODEL);

	MyMesh* bodyMesh = car.GetBodyMesh();
	car.SetMeshMaterial(bodyMesh, car.GetBodyMaterial());
	SendMeshMaterial(bodyMesh, 0);

	loadMatrix(MODEL, car.GetRoot()->body->m_model);
	computeDerivedMatrix(PROJ_VIEW_MODEL);
	RenderMesh(bodyMesh);

	car.SetMeshMaterial(bodyMesh, car.GetTopMaterial());
	SendMeshMaterial(bodyMesh, 0);

	loadMatrix(MODEL, car.GetRoot()->top->m_model);
	computeDerivedMatrix(PROJ_VIEW_MODEL);
	RenderMesh(bodyMesh);

	MyMesh* wheelMesh = car.GetWheelMesh();
	car.SetMeshMaterial(wheelMesh, car.GetWheelMaterial());
	SendMeshMaterial(wheelMesh, 0);

	loadMatrix(MODEL, car.GetRoot()->topLeft->m_model);
	computeDerivedMatrix(PROJ_VIEW_MODEL);
	RenderMesh(wheelMesh);

	loadMatrix(MODEL, car.GetRoot()->topRight->m_model);
	computeDerivedMatrix(PROJ_VIEW_MODEL);
	RenderMesh(wheelMesh);

	loadMatrix(MODEL, car.GetRoot()->botRight->m_model);
	computeDerivedMatrix(PROJ_VIEW_MODEL);
	RenderMesh(wheelMesh);

	loadMatrix(MODEL, car.GetRoot()->botLeft->m_model);
	computeDerivedMatrix(PROJ_VIEW_MODEL);
	RenderMesh(wheelMesh);

	popMatrix(MODEL);
}

void DrawRearViewMirror()
{
	loadIdentity(MODEL);
	car.Update();

	pushMatrix(MODEL);

	loadMatrix(MODEL, car.GetRoot()->body->m_model);
	computeDerivedMatrix(PROJ_VIEW_MODEL);


	loadMatrix(MODEL, car.GetRoot()->top->m_model);
	computeDerivedMatrix(PROJ_VIEW_MODEL);


	loadMatrix(MODEL, car.GetRoot()->topLeft->m_model);
	computeDerivedMatrix(PROJ_VIEW_MODEL);

	loadMatrix(MODEL, car.GetRoot()->topRight->m_model);
	computeDerivedMatrix(PROJ_VIEW_MODEL);

	loadMatrix(MODEL, car.GetRoot()->botRight->m_model);
	computeDerivedMatrix(PROJ_VIEW_MODEL);

	loadMatrix(MODEL, car.GetRoot()->botLeft->m_model);
	computeDerivedMatrix(PROJ_VIEW_MODEL);


	MyMesh* rearMesh = car.GetRearMesh();
	car.SetMeshMaterial(rearMesh, car.GetWheelMaterial());
	SendMeshMaterial(rearMesh, 0);
	loadMatrix(MODEL, car.GetRoot()->rearMirror->m_model);
	computeDerivedMatrix(PROJ_VIEW_MODEL);
	RenderMesh(rearMesh);

	popMatrix(MODEL);
}

void LoadRearViewCamera() {
	loadIdentity(VIEW);
	float* dir = car.GetDirection();
	float targetOffset[3] = { dir[0], 0, dir[2] };
	constProduct(2.0f, targetOffset, targetOffset);
	targetOffset[1] = 1.0f;
	float pos[3] = { 0.0f, 0.0f, 0.0f };
	add(car.GetPosition(), targetOffset, pos);

	// move camera slightly right to center in car
	float right[3] = { 0 };
	float up[3] = { 0, 1, 0 };
	crossProduct(dir, up, right);
	constProduct(0.5f, right, right);
	add(right, pos, pos);

	float lookAt[3] = { 0 };
	float lookAtOffset[3] = { -dir[0], 0, -dir[2] };
	add(pos, lookAtOffset, lookAt);
	lookAt[1] = 1.6f;
	activeCameraRef->FollowTarget(lookAt, pos, deltaAlpha, deltaBeta);
}


void detectCheckpoints() {
	if ((car.GetBBBounds()[0] <= checkpoint1[1] && car.GetBBBounds()[1] >= checkpoint1[0]) &&
		(car.GetBBBounds()[2] <= checkpoint1[3] && car.GetBBBounds()[3] >= checkpoint1[2]) &&
		(car.GetBBBounds()[4] <= checkpoint1[5] && car.GetBBBounds()[5] >= checkpoint1[4])) {
		c1 = true;
	}
	else if ((car.GetBBBounds()[0] <= checkpoint2[1] && car.GetBBBounds()[1] >= checkpoint2[0]) &&
		(car.GetBBBounds()[2] <= checkpoint2[3] && car.GetBBBounds()[3] >= checkpoint2[2]) &&
		(car.GetBBBounds()[4] <= checkpoint2[5] && car.GetBBBounds()[5] >= checkpoint2[4]) && c1 == true) {
		c2 = true;
	}
	else if ((car.GetBBBounds()[0] <= checkpoint3[1] && car.GetBBBounds()[1] >= checkpoint3[0]) &&
		(car.GetBBBounds()[2] <= checkpoint3[3] && car.GetBBBounds()[3] >= checkpoint3[2]) &&
		(car.GetBBBounds()[4] <= checkpoint3[5] && car.GetBBBounds()[5] >= checkpoint3[4]) && c1 == true && c2==true) {
		c3 = true;
	}
	else if ((car.GetBBBounds()[0] <= checkpoint4[1] && car.GetBBBounds()[1] >= checkpoint4[0]) &&
		(car.GetBBBounds()[2] <= checkpoint4[3] && car.GetBBBounds()[3] >= checkpoint4[2]) &&
		(car.GetBBBounds()[4] <= checkpoint4[5] && car.GetBBBounds()[5] >= checkpoint4[4]) && c1 == true && c2 == true && c3 == true) {
		c1 = false;
		c2 = false;
		c3 = false;
		particlesActive = true;
		iniParticles();
		gameManager.IncrementPoints(1);
	}
}



void detectCollisions() {

	for (int obj_index_1 = 0; obj_index_1 < gameObjectsRef.size(); obj_index_1++)
	{
		GameObject3D* obj_a = gameObjectsRef[obj_index_1];
		if (!obj_a->GetIsCollisionEnabled())
			continue;

		for (int obj_index_2 = 0; obj_index_2 < gameObjectsRef.size(); obj_index_2++)
		{
			bool result = false;
			GameObject3D* obj_b = gameObjectsRef[obj_index_2];

			if (obj_b->GetIsCollisionEnabled()) {
				result = obj_a->DetectCollisionWithObject(obj_b);
			}

			if (result) {
				//printf("object [%s] colided with object %s\n", obj_a->GetType(), obj_b->GetType());
				obj_a->ResolveCollisionWithObject(obj_b);
			}
		}
	}

	//car.PrintBB();
	//printf("car position ( %f, %f, %f)\n", car.GetPosition()[0], car.GetPosition()[1], car.GetPosition()[2]);
	//printf("bb bounds ( %f, %f, %f, %f, %f, %f)\n", car.GetBBBounds()[0], car.GetBBBounds()[1], car.GetBBBounds()[2], car.GetBBBounds()[3], car.GetBBBounds()[4], car.GetBBBounds()[5]);

	for (int obj_index_1 = 0; obj_index_1 < gameObjectsRef.size(); obj_index_1++)
	{
		bool result = false;
		GameObject3D* obj_a = gameObjectsRef[obj_index_1];

		if (obj_a->GetIsCollisionEnabled() && car.GetIsCollisionEnabled()) {
			//printf("CAR    bb bounds ( %f, %f, %f, %f, %f, %f)\n", car.GetBBBounds()[0], car.GetBBBounds()[1], car.GetBBBounds()[2], car.GetBBBounds()[3], car.GetBBBounds()[4], car.GetBBBounds()[5]);
			//car.PrintBB();
			if (obj_a->GetType() == GameObject3D::TYPE::Table) {
				//Table* t = dynamic_cast<Table*>(obj_a);
				
				//printf("BUTTER bb bounds ( %f, %f, %f, %f, %f, %f)\n", t->GetBBBounds()[0], t->GetBBBounds()[1], t->GetBBBounds()[2], t->GetBBBounds()[3], t->GetBBBounds()[4], t->GetBBBounds()[5]);
				//printf("CAR    bb bounds ( %f, %f, %f, %f, %f, %f)\n", car.GetBBBounds()[0], car.GetBBBounds()[1], car.GetBBBounds()[2], car.GetBBBounds()[3], car.GetBBBounds()[4], car.GetBBBounds()[5]);
			}
			//Table* t = dynamic_cast<Table*>(obj_a);
			//t->PrintBB();
			//printf("bb bounds ( %f, %f, %f, %f, %f, %f)\n", t->GetBBBounds()[0], t->GetBBBounds()[1], t->GetBBBounds()[2], t->GetBBBounds()[3], t->GetBBBounds()[4], t->GetBBBounds()[5]);
			if (obj_a->GetType() == GameObject3D::TYPE::Table || obj_a->GetType() == GameObject3D::TYPE::Orange || obj_a->GetType() == GameObject3D::TYPE::Cheerio) {
				result = car.DetectCollisionWithObject(obj_a);
			}
		}

		if (result) {
			printf("car colided with object \n");
			car.ResolveCollisionWithObject(obj_a);
			if (!shaking) {
				shakeTime = 0.2f;
				shaking = true;
				//glUniform1i(shaking_uniformId, 1);
				shader.setUniform("shaking", 1);
			}

			if (obj_a->GetType() == GameObject3D::TYPE::Orange) {
				gameManager.DecreaseLives();
				c1 = c2 = c3 = false;
			}
		}
	}
}


void updateParticles()
{
	int i;
	float h;

	/* MÈtodo de Euler de integraÁ„o de eq. diferenciais ordin·rias
	h representa o step de tempo; dv/dt = a; dx/dt = v; e conhecem-se os valores iniciais de x e v */

	//h = 0.125f;
	h = 0.033;
	if (particlesActive) {

		for (i = 0; i < MAX_PARTICULAS; i++)
		{
			particula[i].x += (h * particula[i].vx);
			particula[i].y += (h * particula[i].vy);
			particula[i].z += (h * particula[i].vz);
			particula[i].vx += (h * particula[i].ax);
			particula[i].vy += (h * particula[i].ay);
			particula[i].vz += (h * particula[i].az);
			particula[i].life -= particula[i].fade;
		}
	}
}

void drawTable(bool drawRoad) {

	if(drawRoad){
		for (int objId = 0; objId < 8; objId++)
		{
			GameObject3D* objRef = gameObjectsRef[objId];
			if (objRef == nullptr) {
				continue;
			}


			objRef->PrepareMeshMaterial();
			MyMesh* mesh = objRef->GetMesh();
			SendMeshMaterial(mesh, 0);

			pushMatrix(MODEL);

			objRef->Update();

			glUniform1i(texMode_uniformId, 0);

			if (objRef->GetIsEnabled()) {
				glUniformMatrix4fv(view_uniformId, 1, GL_FALSE, mMatrix[VIEW]);
				computeDerivedMatrix(PROJ_VIEW_MODEL);
				glUniform1i(grassMode_uniformId, 0);
				RenderMesh(mesh);
			}

			popMatrix(MODEL);


		}
	}
	

	GameObject3D* objRef = gameObjectsRef[gameObjectsRef.size() - 1];

	objRef->PrepareMeshMaterial();
	MyMesh* mesh = objRef->GetMesh();
	SendMeshMaterial(mesh, 0);

	pushMatrix(MODEL);

	objRef->Update();
	glUniform1i(texMode_uniformId, 3); // multitexturing
	glUniform1i(tex_loc1, 1);
	glUniform1i(tex_loc2, 2);

	if (objRef->GetIsEnabled()) {
		glUniformMatrix4fv(view_uniformId, 1, GL_FALSE, mMatrix[VIEW]);
		computeDerivedMatrix(PROJ_VIEW_MODEL);
		RenderMesh(mesh);
	}

	popMatrix(MODEL);
}

void drawObjects(bool isShadow) {
	for (int objId = 8; objId < gameObjectsRef.size() - 1; objId++)
	{
		GameObject3D* objRef = gameObjectsRef[objId];
		if (objRef == nullptr) {
			continue;
		}

		objRef->PrepareMeshMaterial();
		MyMesh* mesh = objRef->GetMesh();
		if (objRef->GetType() == GameObject3D::TYPE::Billboard) {
			SendMeshMaterial(mesh, 1);
		}
		else {
			SendMeshMaterial(mesh, 0);
		}

		pushMatrix(MODEL);

		if (pause == false || objRef->GetType() == GameObject3D::TYPE::Billboard)
			objRef->Update();
		else
			objRef->Paused();

		// FIXME refactor
		if (objId == gameObjectsRef.size() - 1)
		{
			glUniform1i(texMode_uniformId, 3); // multitexturing
		}
		else if (objRef->GetType() == GameObject3D::TYPE::Billboard)
		{
			//printf("Billboard in renderScene\n");
			Billboard* bb = dynamic_cast<Billboard*>(objRef);

			if (activeCameraId == 2) {
				bb->orientateBillboard(pCam2.GetPosition(), 3);
			}
			else if (activeCameraId == 3) {
				bb->orientateBillboard(rearviewCam.GetPosition(), 3);
			}
			else if (activeCameraId == 4) {
				bb->orientateBillboard(rearviewMirrorCam.GetPosition(), 3);
			}

			glUniform1i(texMode_uniformId, 4);
		}
		else if (objRef->GetType() == GameObject3D::TYPE::Orange) {
			if (bumpmap)
				glUniform1i(texMode_uniformId, 6);
			else
				glUniform1i(texMode_uniformId, 0); //bump mapping: normal comes from normalMap
		}
		else
		{
			glUniform1i(texMode_uniformId, 0);
		}
		
		if (objRef->GetIsEnabled() && objId != 0) {
			if ((objId == gameObjectsRef.size() - 2 || objRef->GetType() == GameObject3D::TYPE::Billboard) && !isShadow) {
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glEnable(GL_BLEND);

			}
			if (objRef->GetType() != GameObject3D::TYPE::Billboard) {
				glUniformMatrix4fv(view_uniformId, 1, GL_FALSE, mMatrix[VIEW]);
				computeDerivedMatrix(PROJ_VIEW_MODEL);
			}
			if (!(isShadow && objRef->GetType() == GameObject3D::TYPE::Billboard)) // don't draw billboard shadow
			{
				if (objRef->GetType() == GameObject3D::TYPE::Grass) {
					glUniform1i(grassMode_uniformId, 1);
					RenderInstances(mesh);
				}
				else {
					glUniform1i(grassMode_uniformId, 0);
					RenderMesh(mesh);
				}
				
			}
			if ((objId == gameObjectsRef.size() - 2 || objRef->GetType() == GameObject3D::TYPE::Billboard) && !isShadow) {
				glDisable(GL_BLEND);

			}
		}

		popMatrix(MODEL);

		if (objRef->GetType() == GameObject3D::TYPE::Orange) {
			Orange* orange = dynamic_cast<Orange*>(objRef);
			orange->StoreBounds(t.GetBBBounds());
		}
	}
}

void RenderParticles()
{
	GLint loc;

	float particle_color[4];

	updateParticles();

	// draw fireworks particles
	//objId = 6;  //quad for particle

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureArray[6]); //particle.tga associated to TU0 
	glUniform1i(tex_loc, 0);  //use TU 0
	glUniform1i(texMode_uniformId, 8); // draw modulated textured particles 

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDepthMask(GL_FALSE);  //Depth Buffer Read Only


	for (int i = 0; i < MAX_PARTICULAS; i++)
	{
		if (particula[i].life > 0.0f) /* sÛ desenha as que ainda est„o vivas */
		{

			/* A vida da partÌcula representa o canal alpha da cor. Como o blend est· activo a cor final È a soma da cor rgb do fragmento multiplicada pelo
			alpha com a cor do pixel destino */

			particle_color[0] = particula[i].r;
			particle_color[1] = particula[i].g;
			particle_color[2] = particula[i].b;
			particle_color[3] = particula[i].life;

			// send the material - diffuse color modulated with texture
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
			glUniform4fv(loc, 1, particle_color);

			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.albedo");
			glUniform4fv(loc, 1, particle_color);

			pushMatrix(MODEL);
			translate(MODEL, particula[i].x, particula[i].y, particula[i].z);

			// send matrices to OGL
			float pos[3] = { (float)particula[i].x, (float)particula[i].y , (float)particula[i].z };
			if (activeCameraId == 2) {
				l3dBillboardCylindricalBegin(pCam2.GetPosition(), pos);
			}
			else if (activeCameraId == 3) {
				l3dBillboardCylindricalBegin(rearviewCam.GetPosition(), pos);
			}
			else if (activeCameraId == 4) {
				l3dBillboardCylindricalBegin(rearviewMirrorCam.GetPosition(), pos);
			}
			
			computeDerivedMatrix(PROJ_VIEW_MODEL);
			//printf("position of particle - (%f, %f, %f)\n", pos[0], pos[1], pos[2]);
			glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
			glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
			computeNormalMatrix3x3();
			glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

			glBindVertexArray(quadMesh.vao);
			glDrawElements(quadMesh.type, quadMesh.numIndexes, GL_UNSIGNED_INT, 0);
			popMatrix(MODEL);
		}
		else dead_num_particles++;
	}

	glDepthMask(GL_TRUE); //make depth buffer again writeable

	if (dead_num_particles == MAX_PARTICULAS) {
		particlesActive = false;
		c4 = false;
		dead_num_particles = 0;
		printf("All particles dead\n");
	}

}

void renderSkybox() {
	// Render Sky Box
	glUniform1i(texMode_uniformId, 5);

	skybox.PrepareMeshMaterial();
	SendMeshMaterial(skybox.GetMesh(), 0);

	//it won't write anything to the zbuffer; all subsequently drawn scenery to be in front of the sky box. 
	glDepthMask(GL_FALSE);
	glFrontFace(GL_CW); // set clockwise vertex order to mean the front

	pushMatrix(MODEL);
	pushMatrix(VIEW);  //se quiser anular a translaÁ„o

	//  Fica mais realista se n„o anular a translaÁ„o da c‚mara 
	// Cancel the translation movement of the camera - de acordo com o tutorial do Antons
	mMatrix[VIEW][12] = 0.0f;
	mMatrix[VIEW][13] = 0.0f;
	mMatrix[VIEW][14] = 0.0f;

	//scale(MODEL, 100.0f, 100.0f, 100.0f);
	//translate(MODEL, -0.5f, -0.5f, -0.5f);

	skybox.Update();

	// send matrices to OGL
	glUniformMatrix4fv(model_uniformId, 1, GL_FALSE, mMatrix[MODEL]); //TransformaÁ„o de modelaÁ„o do cubo unit·rio para o "Big Cube"
	computeDerivedMatrix(PROJ_VIEW_MODEL);
	glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);

	glBindVertexArray(skybox.GetMesh()->vao);
	glDrawElements(skybox.GetMesh()->type, skybox.GetMesh()->numIndexes, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	popMatrix(MODEL);
	popMatrix(VIEW);

	glFrontFace(GL_CCW); // restore counter clockwise vertex order to mean the front
	glDepthMask(GL_TRUE);

}

void renderAssimpObjects() 
{
	pushMatrix(MODEL);
	translate(MODEL, position[0], 0.0f, position[2]);
	scale(MODEL, 0.1f, 0.06f, 0.1f);
	aiRecursive_render(scene, scene->mRootNode, lampMesh);
	popMatrix(MODEL);

	pushMatrix(MODEL);
	translate(MODEL, position1[0], 0.0f, position1[2]);
	scale(MODEL, 0.1f, 0.06f, 0.1f);
	aiRecursive_render(scene, scene->mRootNode, lampMesh);
	popMatrix(MODEL);

	pushMatrix(MODEL);
	translate(MODEL, position2[0], 0.0f, position2[2]);
	scale(MODEL, 0.1f, 0.06f, 0.1f);
	aiRecursive_render(scene, scene->mRootNode, lampMesh);
	popMatrix(MODEL);

	pushMatrix(MODEL);
	translate(MODEL, position3[0], 0.0f, position3[2]);
	scale(MODEL, 0.1f, 0.06f, 0.1f);
	aiRecursive_render(scene, scene->mRootNode, lampMesh);
	popMatrix(MODEL);

	pushMatrix(MODEL);
	translate(MODEL, position4[0], 0.0f, position4[2]);
	scale(MODEL, 0.1f, 0.06f, 0.1f);
	aiRecursive_render(scene, scene->mRootNode, lampMesh);
	popMatrix(MODEL);

	pushMatrix(MODEL);
	translate(MODEL, position5[0], 0.0f, position5[2]);
	scale(MODEL, 0.1f, 0.06f, 0.1f);
	aiRecursive_render(scene, scene->mRootNode, lampMesh);
	popMatrix(MODEL);

	pushMatrix(MODEL);
	translate(MODEL, 0.0f, 2.0f, 3.0f);
	scale(MODEL, 1.0f, 1.0f, 1.0f);
	aiRecursive_render(backScene, backScene->mRootNode, backpackMesh);
	popMatrix(MODEL);
}

void UpdateCollisions() {
	if (shakeTime > 0.0f)
	{
		shakeTime -= TimeUtil::deltaTime;
		if (shakeTime <= 0.0f) {
			shaking = false;
			glUniform1i(shaking_uniformId, 0);
			shader.setUniform("shaking", 0);
		}	
	}
}

void drawTerrain()
{
	pushMatrix(MODEL);
	loadIdentity(MODEL);
	computeDerivedMatrix(PROJ_VIEW_MODEL);
	glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
	glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
	computeNormalMatrix3x3();
	glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

	glBindVertexArray(terrainVAO);
	// render the mesh triangle strip by triangle strip - each row at a time
	for (unsigned int strip = 0; strip < NUM_STRIPS; strip++)
	{
		glDrawElements(GL_TRIANGLE_STRIP,   // primitive type
			NUM_VERTS_PER_STRIP, // number of indices to render
			GL_UNSIGNED_INT,     // index data type
			(void*)(sizeof(unsigned int)
				* NUM_VERTS_PER_STRIP
				* strip)); // offset to starting index
	}
	glBindVertexArray(0);
	popMatrix(MODEL);
}

void renderScene(void) {
	// load identity matrices
	loadIdentity(VIEW);
	loadIdentity(MODEL);

	// FIXME refactor
	if (activeCameraId == 2)
	{
		float* dir = car.GetDirection();
		float targetOffset[3] = { -dir[0], 0, -dir[2] };
		constProduct(7.0f, targetOffset, targetOffset);
		targetOffset[1] = 5.0f;
		float pos[3] = { 0.0f, 0.0f, 0.0f };
		add(car.GetPosition(), targetOffset, pos);

		// move camera slightly right to center in car
		float right[3] = { 0 };
		float up[3] = { 0, 1, 0 };
		crossProduct(dir, up, right);
		constProduct(0.25f, right, right);
		add(right, pos, pos);

		activeCameraRef->FollowTarget(car.GetPosition(), pos, deltaAlpha, deltaBeta);
	}
	else if (activeCameraId == 3)
	{
		float* dir = car.GetDirection();
		float targetOffset[3] = { dir[0], 0, dir[2] };
		constProduct(1.2f, targetOffset, targetOffset);
		targetOffset[1] = 0.7f;
		float pos[3] = { 0.0f, 0.0f, 0.0f };
		add(car.GetPosition(), targetOffset, pos);

		// move camera slightly right to center in car
		float right[3] = { 0 };
		float up[3] = { 0, 1, 0 };
		crossProduct(dir, up, right);
		constProduct(0.5f, right, right);
		add(right, pos, pos);

		float lookAt[3] = { 0 };
		float lookAtOffset[3] = { dir[0], 0, dir[2] };
		constProduct(1.3f, lookAtOffset, lookAtOffset);
		add(pos, lookAtOffset, lookAt);
		activeCameraRef->FollowTarget(lookAt, pos, deltaAlpha, deltaBeta);
	}
	else
	{
		activeCameraRef->Update();
	}


	// use our shader
	glUseProgram(shader.getProgramIndex());


	glUniform1i(shaking_uniformId, 0);
	if (shaking) {
		glUniform1i(shaking_uniformId, 1);
	}
	shader.setUniform("shaking", 0);

	glUniform3fv(camPos_uniformId, 1, activeCameraRef->GetPosition());
	glUniform1i(togglePbr_uniformId, pbr);
	glUniform1i(toggleFog_uniformId, toggleFog);


	//send the light position in eye coordinates

		//glUniform4fv(lPos_uniformId, 1, lightPos); //efeito capacete do mineiro, ou seja lighPos foi definido em eye coord 

	//float res[4];
	//multMatrixPoint(VIEW, lightPos, res);   //lightPos definido em World Coord so is converted to eye space
	//glUniform4fv(lPos_uniformId, 1, res);
	
	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		SendLights(lights[i], i);
	}

	//FIXME REFACTOR AND OPTIMISE Associar os Texture Units aos Objects Texture
	//stone.tga loaded in TU0; checker.tga loaded in TU1;  lightwood.tga loaded in TU2
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureArray[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TextureArray[1]);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, TextureArray[2]);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, TextureArray[3]);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, TextureArray[4]);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_CUBE_MAP, TextureArray[5]);

	//Indicar aos tres samplers do GLSL quais os Texture Units a serem usados
	glUniform1i(tex_loc, 0);
	glUniform1i(tex_loc1, 1);
	glUniform1i(tex_loc2, 2);
	glUniform1i(tex_loc3, 3);
	glUniform1i(tex_normalMap_loc, 4);
	glUniform1i(tex_cube_loc, 5);

	glUniform1i(texMode_uniformId, 0); // FIXME refactor

	//printf("Elapsed time = [%f]\n", timeCounter);
	if (!pause) {
		timeCounter += static_cast<float>(TimeUtil::deltaTime);
	}
	
	glUniform1f(time_uniformId, timeCounter);
	shader.setUniform("timeTotal", timeCounter);

	glUniform1i(grassMode_uniformId, 0);

	renderSkybox();

	glEnable(GL_STENCIL_TEST);        // Escrever 1 no stencil buffer onde se for desenhar a reflex„o e a sombra
	glStencilFunc(GL_NEVER, 0x1, 0x1);
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

	drawTable(true);

	glUniform1i(shadowMode_uniformId, 0);  //iluminaÁ„o phong

	glStencilFunc(GL_EQUAL, 0x1, 0x1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	float res[4];
	float mat[16];
	GLfloat plano_chao[4] = { 0,1,0,0 };

	// Render the reflected geometry
	position[1] *= (-1.0f);  //mirror the position of light
	multMatrixPoint(VIEW, position, res);

	glUniform4fv(lPos_uniformId, 1, res);
	pushMatrix(MODEL);

	if (restart) {
		restartObjects();
		restart = false;
	}

	scale(MODEL, 1.0f, -1.0f, 1.0f);
	glCullFace(GL_FRONT);

	drawObjects(false);

	if (pause == false) {
		car.MoveCar();
	}
	UpdateCarMeshes();

	renderAssimpObjects();

	glCullFace(GL_BACK);
	popMatrix(MODEL);

	position[1] *= (-1.0f);  //reset the light position
	multMatrixPoint(VIEW, position, res);
	glUniform4fv(lPos_uniformId, 1, res);

	glStencilFunc(GL_EQUAL, 0x0, 0x1);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glUniform1i(terrainMode_uniformId, 1);
	glFrontFace(GL_CW);
	drawTerrain();
	glFrontFace(GL_CCW);
	glUniform1i(terrainMode_uniformId, 0);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glStencilFunc(GL_EQUAL, 0x1, 0x1);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);		// Blend specular Ground with reflected geometry
	drawTable(true);

	// Render the Shadows
	glUniform1i(shadowMode_uniformId, 1);  //Render with constant color
	shadow_matrix(mat, plano_chao, position);

	glDisable(GL_DEPTH_TEST); //To force the shadow geometry to be rendered even if behind the floor

	////Dark the color stored in color buffer
	glBlendFunc(GL_DST_COLOR, GL_ZERO);
	glStencilOp(GL_KEEP, GL_KEEP, GL_ZERO);

	pushMatrix(MODEL);
	multMatrix(MODEL, mat);
	drawObjects(true);

	renderAssimpObjects();

	UpdateCarMeshes();
	popMatrix(MODEL);


	glDisable(GL_STENCIL_TEST);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	//render the geometry
	glUniform1i(shadowMode_uniformId, 0);
	drawObjects(false);
	
	renderAssimpObjects();

	glUniform1i(texMode_uniformId, 0);
	UpdateCarMeshes();
	car.CalculateBoundingBox();

	detectCollisions();
	UpdateCollisions();
	detectCheckpoints();

	if (particlesActive)
	{
		RenderParticles();
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_BLEND);

	if (activeCameraId == 2 || activeCameraId == 3)
	{
		int camId = activeCameraId;
		int spot1, spot2;

		spot1 = car.GetSpotLight1()->isEnabled;
		spot2 = car.GetSpotLight2()->isEnabled;

		car.GetSpotLight1()->isEnabled = 0;
		car.GetSpotLight2()->isEnabled = 0;


		for (int i = 0; i < MAX_LIGHTS; i++)
		{
			SendLights(lights[i], i);
		}
		// rearview mirror stuff
		glClear(GL_DEPTH_BUFFER_BIT); // render new objects "in front" of the rest
		glEnable(GL_STENCIL_TEST);        // Escrever 1 no stencil buffer onde se for desenhar a reflex„o e a sombra
		glClear(GL_STENCIL_BUFFER_BIT);

		glStencilFunc(GL_NEVER, 0X1, 0X1);
		glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

		DrawRearViewMirror();
		pushMatrix(VIEW);
		LoadCamera(4);
		LoadRearViewCamera();
		
		glStencilFunc(GL_EQUAL, 0x1, 0x1);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

		glUniform1i(grassMode_uniformId, 0);
		renderSkybox();
		drawTable(false);
		drawObjects(false);
		renderAssimpObjects();

		glUniform1i(terrainMode_uniformId, 1);
		glFrontFace(GL_CW);
		drawTerrain();
		glFrontFace(GL_CCW);
		glUniform1i(terrainMode_uniformId, 0);

		popMatrix(VIEW);
		LoadCamera(camId);

		glDisable(GL_STENCIL_TEST);

		car.GetSpotLight1()->isEnabled = spot1;
		car.GetSpotLight2()->isEnabled = spot2;

		for (int i = 0; i < MAX_LIGHTS; i++)
		{
			SendLights(lights[i], i);
		}
	}

	// LENS FLARE
	if (flareEffect) {

		int flarePos[2];
		int m_viewport[4];
		glGetIntegerv(GL_VIEWPORT, m_viewport);

		pushMatrix(MODEL);
		loadIdentity(MODEL);
		translate(MODEL, flareLightPos[0], flareLightPos[1], flareLightPos[2]);
		computeDerivedMatrix(PROJ_VIEW_MODEL);  //pvm to be applied to lightPost. pvm is used in project function

		if (!project(flareLightPos, lightScreenPos, m_viewport))
			printf("Error in getting projected light in screen\n");  //Calculate the window Coordinates of the light position: the projected position of light on viewport
		flarePos[0] = clampi((int)lightScreenPos[0], m_viewport[0], m_viewport[0] + m_viewport[2] - 1);
		flarePos[1] = clampi((int)lightScreenPos[1], m_viewport[1], m_viewport[1] + m_viewport[3] - 1);
		popMatrix(MODEL);

		//viewer looking down at  negative z direction
		pushMatrix(PROJECTION);
		loadIdentity(PROJECTION);
		pushMatrix(VIEW);
		loadIdentity(VIEW);
		ortho(m_viewport[0], m_viewport[0] + m_viewport[2] - 1, m_viewport[1], m_viewport[1] + m_viewport[3] - 1, -1, 1);
		render_flare(&AVTflare, flarePos[0], flarePos[1], m_viewport);
		popMatrix(PROJECTION);
		popMatrix(VIEW);
	}

	if(!gameManager.UpdateHUD())
		pause=true;

}

void renderLoop(void) {
	TimeUtil::UpdateDeltaTime();

	FrameCount++;
	// 1. render scene into floating point framebuffer
		// -----------------------------------------------
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	renderScene();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/*glDisable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(shaderHDR.getProgramIndex());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorBuffers[1]);
	glUniform1i(texLoc, 0);
	renderQuad();*/

	// 2. blur bright fragments with two-pass Gaussian Blur 
	// --------------------------------------------------
	bool horizontal = true, first_iteration = true;
	unsigned int amount = 10;

	glUseProgram(shaderBlur.getProgramIndex());
	
	for (unsigned int i = 0; i < amount; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
		shaderBlur.setUniform("horizontal", horizontal);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorbuffers[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
		glUniform1i(texLoc1, 0);
		renderQuad();
		horizontal = !horizontal;
		if (first_iteration)
			first_iteration = false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 3. now shake the image in case of collision

	if (shaking) {
		glUseProgram(shaderShake.getProgramIndex());

		glUniform1f(timeShake_uniformId, timeCounter);
		shader.setUniform("time", timeCounter);

		glBindFramebuffer(GL_FRAMEBUFFER, shakeFBO);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);  // bind texture of other framebuffer (or scene if first iteration)
		glUniform1i(texLoc4, 0);
		renderQuad();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	

	// 4. now render floating point color buffer to 2D quad and tonemap HDR colors to default framebuffer's (clamped) color range
		// --------------------------------------------------------------------------------------------------------------------------
	glDisable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderBloomFinal.getProgramIndex());
	glActiveTexture(GL_TEXTURE0);
	if (shaking) {
		glBindTexture(GL_TEXTURE_2D, shakeColorBuffer[0]);
	}
	else {
		glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
	}
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);

	glUniform1i(texLoc2, 0);
	glUniform1i(texLoc3, 1);

	renderQuad();

	if (activeCameraId == 2)
	{

	}

	//std::cout << "bloom: " << (bloom ? "on" : "off") << "| exposure: " << exposure << std::endl;

	glutSwapBuffers();
}

// ------------------------------------------------------------
//
// Events from the Keyboard
//
void processKeys(unsigned char key, int xx, int yy)
{
	switch (key) {
	case 27:
		glutLeaveMainLoop();
		break;
	case '1':
		LoadCamera(0);
		break;
	case '2':
		LoadCamera(1);
		break;
	case '3':
		LoadCamera(2);
		break;
	case '4':
		LoadCamera(3); // see rearview mirror up close
		break;
	case 'b':
		if (bumpmap == 0) bumpmap = 1;
		else
			bumpmap = 0;
		break;
	case 'c':
		pl1.ToggleLight();
		pl2.ToggleLight();
		pl3.ToggleLight();
		pl4.ToggleLight();
		pl5.ToggleLight();
		pl6.ToggleLight();
		break;
	case 'p':
		if (pause==false) {
			car.SetSteerChange(car.GetRoatationSpeed());
		}
		break;
	case 'o':
		if (pause==false) {
			car.SetSteerChange(-car.GetRoatationSpeed());
		}
		break;
	case 'q':
		if (pause==false) {
			car.throttle = 1.0f;
		}
		break;
	case 'a':
		if (pause==false) {
			car.brake = 1.0f;
		}
		break;
	case 'h':
		car.GetSpotLight1()->isEnabled = car.GetSpotLight1()->isEnabled ? 0 : 1;
		car.GetSpotLight2()->isEnabled = car.GetSpotLight2()->isEnabled ? 0 : 1;
		break;
	case 'n':
		if (pause == false) {
			dl.ToggleLight();
		}
		break;
	case 'k': billboardType++; if (billboardType == 5) billboardType = 0;
		switch (billboardType) {
		case 0: printf("Cheating Spherical (matrix reset)\n"); break;
		case 1: printf("Cheating Cylindrical (matrix reset)\n"); break;
		case 2: printf("True Spherical\n"); break;
		case 3: printf("True Cylindrical\n"); break;
		case 4: printf("No billboarding\n"); break;
		}
		break;
	case 'f':
		if (pause == false) {
			toggleFog = toggleFog == 0 ? 1 : 0;
		}
		break;
	case 'l':
		flareEffect = !flareEffect;
		break;
	case 'e':
		particlesActive = true;
		iniParticles();
		break;
	case 'm': glEnable(GL_MULTISAMPLE); printf("Multisample Enabled\n"); break;

	case 's':
		if (gameManager.GetLives() != 0){
			printf("pause\n");
			if (pause == false) {
				pause = true;
				gameManager.TogglePause();
			}
			else
			{
				gameManager.TogglePause();
				pause = false;
			}
		}
		break;
	case 'r':
		if (gameManager.GetLives() == 0) {
			gameManager.RestartGame();
			restart = true;
			//gameManager.TogglePause();
			pause = false;
			car.throttle = 0.0f;
			car.brake = 0.0f;
			car.SetSteerChange(0.0f);
		}
		break;
	case 'x':
		if (pause == false) {
			pbr = pbr == 0 ? 1 : 0;
			printf("pbr mode: %d\n", pbr);
		}
		break;
	}

}



// ------------------------------------------------------------
//
// Mouse Events
//
void processMouseButtons(int button, int state, int xx, int yy)
{
	// start tracking the mouse
	if (state == GLUT_DOWN) {
		startX = xx;
		startY = yy;
		if (button == GLUT_LEFT_BUTTON)
			tracking = 1;
		else if (button == GLUT_RIGHT_BUTTON)
			tracking = 2;
	}

	//stop tracking the mouse
	else if (state == GLUT_UP) {
		if (tracking == 1) {
			alpha -= (xx - startX);
			beta += (yy - startY);
		}
		else if (tracking == 2) {
			r += (yy - startY) * 0.01f;
			if (r < 0.1f)
				r = 0.1f;
		}
		tracking = 0;
	}
}

// Track mouse motion while buttons are pressed
void processMouseMotion(int xx, int yy)
{
	int deltaX, deltaY;
	float alphaAux, betaAux;
	float rAux;

	deltaX = xx - startX;
	deltaAlpha = deltaX;
	deltaY = yy - startY;
	deltaBeta = deltaY;

	// left mouse button: move camera
	if (tracking == 1) {
		alphaAux = alpha + deltaX;
		betaAux = beta + deltaY;

		if (betaAux > 85.0f)
			betaAux = 85.0f;
		else if (betaAux < -85.0f)
			betaAux = -85.0f;
		rAux = r;
	}
	// right mouse button: zoom
	else if (tracking == 2) {

		alphaAux = alpha;
		betaAux = beta;
		rAux = r + (deltaY * 0.01f);
		if (rAux < 0.1f)
			rAux = 0.1f;
	}

	camX = rAux * sin(alphaAux * 3.14f / 180.0f) * cos(betaAux * 3.14f / 180.0f);
	camZ = rAux * cos(alphaAux * 3.14f / 180.0f) * cos(betaAux * 3.14f / 180.0f);
	camY = rAux * sin(betaAux * 3.14f / 180.0f);

	//  uncomment this if not using an idle or refresh func
	//	glutPostRedisplay();
}


void mouseWheel(int wheel, int direction, int x, int y) {

	r += direction * 0.1f;
	if (r < 0.1f)
		r = 0.1f;

	camX = r * sin(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camZ = r * cos(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camY = r * sin(beta * 3.14f / 180.0f);

	float pos[3] = { camX, camY, camZ };
	activeCameraRef->SetPosition(pos);

	//  uncomment this if not using an idle or refresh func
	//	glutPostRedisplay();
}

// --------------------------------------------------------
//
// Process key or mouse up
//
void processKeyUp(unsigned char key, int xx, int yy)
{

	switch (key) {
		case 'q':
			if (!pause) {
				car.throttle = 0.0f;
			}
			break;
		case 'a':
			if (!pause) {
				car.brake = 0.0f;
			}
			break;
		case 'o':
		case 'p':
			if (!pause) {
				car.SetSteerChange(0.0f);
			}
			break;
	}
}
// --------------------------------------------------------
//
// Shader Stuff
//
GLuint setupShaders() {

	// Shader for models
	shader.init();
	shader.loadShader(VSShaderLib::VERTEX_SHADER, "shaders/pointlight.vert");
	shader.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/pointlight.frag");
	shader.loadShader(VSShaderLib::GEOMETRY_SHADER, "shaders/pointlight.geom");

	// set semantics for the shader variables
	glBindFragDataLocation(shader.getProgramIndex(), 0, "colorOut");
	glBindFragDataLocation(shader.getProgramIndex(), 1, "BrightColor");
	glBindAttribLocation(shader.getProgramIndex(), VERTEX_COORD_ATTRIB, "position");
	glBindAttribLocation(shader.getProgramIndex(), NORMAL_ATTRIB, "normal");
	glBindAttribLocation(shader.getProgramIndex(), TEXTURE_COORD_ATTRIB, "texCoord");
	glBindAttribLocation(shader.getProgramIndex(), TANGENT_ATTRIB, "tangent");
	glBindAttribLocation(shader.getProgramIndex(), OFFSET_ATTRIB_X, "offsetX");
	glBindAttribLocation(shader.getProgramIndex(), OFFSET_ATTRIB_Z, "offsetZ");
	
	glLinkProgram(shader.getProgramIndex());

	texMode_uniformId = glGetUniformLocation(shader.getProgramIndex(), "texMode"); // different modes of texturing
	grassMode_uniformId = glGetUniformLocation(shader.getProgramIndex(), "grassMode");
	terrainMode_uniformId = glGetUniformLocation(shader.getProgramIndex(), "terrainMode");
	shaking_uniformId = glGetUniformLocation(shader.getProgramIndex(), "shaking");
	time_uniformId = glGetUniformLocation(shader.getProgramIndex(), "timeTotal");
	shadowMode_uniformId = glGetUniformLocation(shader.getProgramIndex(), "shadowMode");
	pvm_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_pvm");
	vm_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_viewModel");
	model_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_Model");
	view_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_View");
	normal_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_normal");
	toggleFog_uniformId = glGetUniformLocation(shader.getProgramIndex(), "toggleFog");
	lPos_uniformId = glGetUniformLocation(shader.getProgramIndex(), "l_pos");
	tex_loc = glGetUniformLocation(shader.getProgramIndex(), "texmap");
	tex_loc1 = glGetUniformLocation(shader.getProgramIndex(), "texmap1");
	tex_loc2 = glGetUniformLocation(shader.getProgramIndex(), "texmap2");
	tex_loc3 = glGetUniformLocation(shader.getProgramIndex(), "texmap3");
	tex_normalMap_loc = glGetUniformLocation(shader.getProgramIndex(), "normalMap");
	tex_cube_loc = glGetUniformLocation(shader.getProgramIndex(), "cubeMap");

	useNormalMap_loc = glGetUniformLocation(shader.getProgramIndex(), "useNormalMap");
	specularMap_loc = glGetUniformLocation(shader.getProgramIndex(), "specularMap");
	diffMapCount_loc = glGetUniformLocation(shader.getProgramIndex(), "diffMapCount");

	albedo_uniformId = glGetUniformLocation(shader.getProgramIndex(), "albedo");
	metallic_uniformId = glGetUniformLocation(shader.getProgramIndex(), "metallic");
	roughness_uniformId = glGetUniformLocation(shader.getProgramIndex(), "roughness");
	ao_uniformId = glGetUniformLocation(shader.getProgramIndex(), "ao");
	camPos_uniformId = glGetUniformLocation(shader.getProgramIndex(), "camPos");
	togglePbr_uniformId = glGetUniformLocation(shader.getProgramIndex(), "togglePbr");

	printf("InfoLog for Per Fragment Phong Lightning Shader\n%s\n\n", shader.getAllInfoLogs().c_str());

	shaderText.init();
	shaderText.loadShader(VSShaderLib::VERTEX_SHADER, "shaders/text.vert");
	shaderText.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/text.frag");
	glLinkProgram(shaderText.getProgramIndex());
	printf("InfoLog for Text Rendering Shader\n%s\n\n", shaderText.getAllInfoLogs().c_str());

	shaderBlur.init();
	shaderBlur.loadShader(VSShaderLib::VERTEX_SHADER, "shaders/blur.vert");
	shaderBlur.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/blur.frag");

	// set semantics for the shader variables
	glBindFragDataLocation(shaderBlur.getProgramIndex(), 0, "colorOut");
	glBindAttribLocation(shaderBlur.getProgramIndex(), VERTEX_COORD_ATTRIB, "position");
	glBindAttribLocation(shaderBlur.getProgramIndex(), TEXTURE_COORD_ATTRIB, "texCoord");

	glLinkProgram(shaderBlur.getProgramIndex());

	texLoc1 = glGetUniformLocation(shaderBlur.getProgramIndex(), "image");
	//blurMode = glGetUniformLocation(shaderBlur.getProgramIndex(), "horizontal");
	printf("InfoLog for Blur Rendering Shader\n%s\n\n", shaderBlur.getAllInfoLogs().c_str());

	shaderShake.init();
	shaderShake.loadShader(VSShaderLib::VERTEX_SHADER, "shaders/shake.vert");
	shaderShake.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/shake.frag");

	// set semantics for the shader variables
	glBindFragDataLocation(shaderShake.getProgramIndex(), 0, "colorOut");
	glBindAttribLocation(shaderShake.getProgramIndex(), VERTEX_COORD_ATTRIB, "position");
	glBindAttribLocation(shaderShake.getProgramIndex(), TEXTURE_COORD_ATTRIB, "texCoord");

	glLinkProgram(shaderShake.getProgramIndex());

	texLoc4 = glGetUniformLocation(shaderShake.getProgramIndex(), "image");
	timeShake_uniformId = glGetUniformLocation(shaderShake.getProgramIndex(), "time");
	printf("InfoLog for Shake Rendering Shader\n%s\n\n", shaderShake.getAllInfoLogs().c_str());

	shaderBloomFinal.init();
	shaderBloomFinal.loadShader(VSShaderLib::VERTEX_SHADER, "shaders/bloom.vert");
	shaderBloomFinal.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/bloom.frag");

	// set semantics for the shader variables
	glBindFragDataLocation(shaderBloomFinal.getProgramIndex(), 0, "colorOut");
	glBindAttribLocation(shaderBloomFinal.getProgramIndex(), VERTEX_COORD_ATTRIB, "position");
	glBindAttribLocation(shaderBloomFinal.getProgramIndex(), TEXTURE_COORD_ATTRIB, "texCoord");

	glLinkProgram(shaderBloomFinal.getProgramIndex());

	texLoc2 = glGetUniformLocation(shaderBloomFinal.getProgramIndex(), "scene");
	texLoc3 = glGetUniformLocation(shaderBloomFinal.getProgramIndex(), "bloomBlur");
	printf("InfoLog for Bloom Rendering Shader\n%s\n\n", shaderBloomFinal.getAllInfoLogs().c_str());

	shaderHDR.init();
	shaderHDR.loadShader(VSShaderLib::VERTEX_SHADER, "shaders/hdr.vert");
	shaderHDR.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/hdr.frag");

	// set semantics for the shader variables
	glBindFragDataLocation(shaderHDR.getProgramIndex(), 0, "FragColor");
	glBindAttribLocation(shaderHDR.getProgramIndex(), VERTEX_COORD_ATTRIB, "aPos");
	glBindAttribLocation(shaderHDR.getProgramIndex(), TEXTURE_COORD_ATTRIB, "aTexCoords");

	glLinkProgram(shaderHDR.getProgramIndex());

	texLoc = glGetUniformLocation(shaderHDR.getProgramIndex(), "hdrBuffer");
	printf("InfoLog for HDR Rendering Shader\n%s\n\n", shaderHDR.getAllInfoLogs().c_str());

	return(shader.isProgramLinked() && shaderText.isProgramLinked() && shaderBlur.isProgramLinked() && shaderShake.isProgramLinked() && shaderBloomFinal.isProgramLinked() && shaderHDR.isProgramLinked());
}

void initCameras() 
{
	// set the camera position based on its spherical coordinates
	camX = r * sin(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camZ = r * cos(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camY = r * sin(beta * 3.14f / 180.0f);
	//float pos[3] = { camX, camY, camZ };
	float pos[3] = { 75.0f, 150.0f, 0.0f };
	//float pos1[3] = { 0.0f, 0.0f, 5.0f };
	float pos1[3] = { 75.0f, 200.0f, 0.0f };
	float lookAt[3] = { 75.0f, 0.0f, 0.0f };
	float up[3] = { 0.0f, 0.0f, -1.0f };
	float up2[3] = { 0.0f, 1.0f, 0.0f };
	pCam1 = PerspectiveCamera(WinX, WinY, lookAt, pos1, up, 53.13f, 0.1f, 1000.0f);
	camerasRef.push_back(&pCam1);
	oCam = OrthographicCamera(WinX, WinY, lookAt, pos, up, 0.0f, 500.0f);
	camerasRef.push_back(&oCam);
	pCam2 = PerspectiveCamera(/*WinX*/1920, /*WinY*/1080, lookAt, pos, up2, 70.0f, 0.1f, 1000.0f);
	camerasRef.push_back(&pCam2);
	rearviewCam = PerspectiveCamera(1920, 1080, lookAt, pos, up2, 70.0f, 0.1f, 1000.0f);
	camerasRef.push_back(&rearviewCam);
	rearviewMirrorCam = PerspectiveCamera(1920, 1080, lookAt, pos, up2, 120.0f, 0.1f, 1000.0f);
	camerasRef.push_back(&rearviewMirrorCam);
	LoadCamera(0);
}

void initFog()
{
	toggleFog = 0;
	glUniform1i(toggleFog_uniformId, toggleFog);
}

void initMeshPrimitives()
{
	cubeMesh = createCube();
	sphereMesh = createSphere(1.0f, 10);
	torusMesh = createTorus(0.5f, 1.0f, 15, 20);
	billboardMesh = createQuad(6, 6);
	quadMesh = createQuad(2, 2);
	//radius should be 0.05, HEIGHT 1.0
	coneMesh = createCone(1.0, 0.05, 10);
}
void initLights()
{
	float position[4] = { 20.0f, 5.0f, 0.0f, 1.0f };
	float position1[4] = { 20.0f, 5.0f, 75.0f, 1.0f };
	float position2[4] = { 75.0f, 5.0f, 80.0f, 1.0f };
	float position3[4] = { 140.0f, 5.0f, 75.0f, 1.0f };
	float position4[4] = { 140.0f, 5.0f, 0.0f, 1.0f };
	float position5[4] = { 75.0f, 5.0f, -75.0f, 1.0f };
	pl1 = PointLight(position);
	pl2 = PointLight(position1);
	pl3 = PointLight(position2);
	pl4 = PointLight(position3);
	pl5 = PointLight(position4);
	pl6 = PointLight(position5);
	lights[0] = pl1.GetLightPtr();
	lights[1] = pl2.GetLightPtr();
	lights[2] = pl3.GetLightPtr();
	lights[3] = pl4.GetLightPtr();
	lights[4] = pl5.GetLightPtr();
	lights[5] = pl6.GetLightPtr();

	dl = DirectionalLight();
	//float pos[4] = { 0.0f, -500.0f, 0.0f, 0.0f };
	//memcpy(dl.light.position, pos, sizeof(float) * 4);
	lights[6] = dl.GetLightPtr();
	lights[7] = car.GetSpotLight1();
	lights[8] = car.GetSpotLight2();
}

void initTextures()
{
	/* Initialization of DevIL */
	if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION)
	{
		printf("wrong DevIL version \n");
		exit(0);
	}
	ilInit();

	glGenTextures(7, TextureArray);
	Texture2D_Loader(TextureArray, "orange_normal.tga", 0, false);
	Texture2D_Loader(TextureArray, "checkers_pattern.png", 1, true);
	Texture2D_Loader(TextureArray, "lightwood.tga", 2, true);
	Texture2D_Loader(TextureArray, "tree.tga", 3, false);
	Texture2D_Loader(TextureArray, "orange_normal.tga", 4, false);

	const char* filenames[] = { "posx.jpg", "negx.jpg", "posy.jpg", "negy.jpg", "posz.jpg", "negz.jpg" };

	TextureCubeMap_Loader(TextureArray, filenames, 5, true);

	Texture2D_Loader(TextureArray, "particle.tga", 6, true);
}

void initFlare()
{
	//// create geometry and VAO of the quad for flare elements
	//amesh = createQuad(1, 1);
	//myMeshes.push_back(amesh);

	//Flare elements textures
	glGenTextures(5, FlareTextureArray);
	Texture2D_Loader(FlareTextureArray, "crcl.tga", 0, true);
	Texture2D_Loader(FlareTextureArray, "flar.tga", 1, true);
	Texture2D_Loader(FlareTextureArray, "hxgn.tga", 2, true);
	Texture2D_Loader(FlareTextureArray, "ring.tga", 3, true);
	Texture2D_Loader(FlareTextureArray, "sun.tga", 4, true);


	////Load flare from file
	loadFlareFile(&AVTflare, "flare.txt");
	flareEffect = false;
}

int initObjectModels(vector<MyMesh>* objMesh, char* name, bool loadTextures, int index) {
	std::string filepath;

	memset(model_dir, '\0', sizeof(model_dir));
	strcat(model_dir, name);
	std::ostringstream oss;
	oss << model_dir << "/" << model_dir << ".obj";
	filepath = oss.str();   //path of OBJ file in the VS project

	strcat(model_dir, "/");  //directory path in the VS project

	//check if file exists
	ifstream fin(filepath.c_str());
	if (!fin.fail()) {
		fin.close();
		//break;
	}
	else
		printf("Couldn't open file: %s\n", filepath.c_str());

	if (!Import3DFromFile(index, filepath))
		return(0);
	//*objMesh = createMeshFromAssimp(scene, loadTextures);

	if (index == 0)
		*objMesh = createMeshFromAssimp(scene, loadTextures);
	else
		*objMesh = createMeshFromAssimp(backScene, loadTextures);
	return(1);
}

void initFramebuffers() {

	
	glGenFramebuffers(1, &hdrFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

	// create 2 floating point color buffers (1 for normal rendering, other for brightness threshold values)
	
	glGenTextures(2, colorBuffers);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, WinX, WinY, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// attach texture to framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
	}
	// create and attach depth buffer (renderbuffer)
	
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WinX, WinY); // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
	// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);
	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// framebuffer for shaking

	glGenFramebuffers(1, &shakeFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shakeFBO);

	// create 2 floating point color buffers (1 for normal rendering, other for brightness threshold values)

	glGenTextures(1, shakeColorBuffer);

	glBindTexture(GL_TEXTURE_2D, shakeColorBuffer[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, WinX, WinY, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// attach texture to framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, shakeColorBuffer[0], 0);
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// ping-pong-framebuffer for blurring
	
	glGenFramebuffers(2, pingpongFBO);
	glGenTextures(2, pingpongColorbuffers);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WinX, WinY, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
		// also check if framebuffers are complete (no need for depth buffer)
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void buildTerrain() {
	// load height map texture
	int width, height, nChannels;
	unsigned char* data = stbi_load("heightmap.png",
		&width, &height, &nChannels,
		0);
	if (data)
		std::cout << "Loaded heightmap of size " << height << " x " << width << std::endl;
	else
		std::cout << "Failed to load texture" << std::endl;

	// vertex generation
	std::vector<float> vertices;
	int rez = 1;
	// y value [0, 256] ???? EXPLAIN THIS
	float yScale = 64.0f / 256.0f, yShift = 5.0f;  // apply scale and down shift to the height data
	for (unsigned int i = 0; i < height; i++)
	{
		for (unsigned int j = 0; j < width; j++)
		{
			// retrieve texel for (i,j) tex coord
			unsigned char* texel = data + (j + width * i) * nChannels;
			// raw height at coordinate
			unsigned char y = texel[0];

			// vertex
			vertices.push_back(-height / 2.0f + height * i / (float)height);   // vx
			vertices.push_back((int)y * yScale - yShift);   // vy
			vertices.push_back(-width / 2.0f + width * j / (float)width);   // vz
		}
	}
	std::cout << "Loaded " << vertices.size() / 3 << " vertices" << std::endl;
	//release height map from memory
	stbi_image_free(data);

	// triangle strips index generation
	std::vector<unsigned int> indices;
	for (unsigned int i = 0; i < height - 1; i += rez)       // for each row a.k.a. each strip
	{
		for (unsigned int j = 0; j < width; j += rez)      // for each column
		{
			for (unsigned int k = 0; k < 2; k++)      // for each side of the strip
			{
				indices.push_back(j + width * (i + k * rez));
			}
		}
	}
	std::cout << "Loaded " << indices.size() << " indices" << std::endl;

	// each strip has NUM_VERTS_PER_STRIP - 2 triangles and the full mesh NUM_STRIPS * triangles
	NUM_STRIPS = (height - 1) / rez;
	NUM_VERTS_PER_STRIP = (width * 2) / rez;

	// register VAO
	glGenVertexArrays(1, &terrainVAO);
	glBindVertexArray(terrainVAO);

	glGenBuffers(1, &terrainVBO);
	glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
	glBufferData(GL_ARRAY_BUFFER,
		vertices.size() * sizeof(float),       // size of vertices buffer
		&vertices[0],                          // pointer to first element
		GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &terrainEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		indices.size() * sizeof(unsigned int), // size of indices buffer
		&indices[0],                           // pointer to first element
		GL_STATIC_DRAW);
}

// ------------------------------------------------------------
//
// Model loading and OpenGL setup
//
void init()
{
	initFramebuffers();
	freeType_init("fonts/arial.ttf");
	initTextures();
	initLights();
	initMeshPrimitives();
	initFog();
	initFlare();
	initObjectModels(&lampMesh, "lamp", false, 0);
	initObjectModels(&backpackMesh, "backpack", true, 1);

	buildTerrain();

	gameManager = GameHudManager(&shaderText);

	// GameObjects
	car = Car(&cubeMesh, &torusMesh, &quadMesh, true, 1.0f);

	float butter1_pos[3] = { 5.0f, 0.0f, 5.0f };
	//o2 = Orange(&sphereMesh, 5.2f, 0.0f, 100.0f, o2Pos, gameObjectsRef.size());
	//o2.PrepareMeshMaterial();
	//gameObjectsRef.push_back(&o2);

	float alt = 0.35;

	float rad = 1.5f;

	Material m1;

	float amb[] = { 0.3f, 0.3f, 0.0f, 0.5f };
	float diff[] = { 0.8f, 0.7f, 0.1f, 0.5f };
	float spec[] = { 0.3f, 0.3f, 0.3f, 1.0f };

	float albedo[] = { 0.8f, 0.7f, 0.1f, 0.5f };
	float metallic = 0.0f;
	float roughness = 0.4f;
	float ao = 1.0f;

	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess = 100.0f;
	int texcount = 0;

	memcpy(m1.ambient, amb, 4 * sizeof(float));
	memcpy(m1.diffuse, diff, 4 * sizeof(float));
	memcpy(m1.specular, spec, 4 * sizeof(float));
	memcpy(m1.emissive, emissive, 4 * sizeof(float));
	m1.shininess = shininess;
	m1.texCount = texcount;

	memcpy(m1.albedo, albedo, 4 * sizeof(float));
	m1.metallic = metallic;
	m1.roughness = roughness;
	m1.ao = ao;

	Material m_skybox;

	float ambSB[] = { 0.2f, 0.15f, 0.1f, 1.0f };
	float diffSB[] = { 0.8f, 0.6f, 0.4f, 1.0f };
	float specSB[] = { 0.8f, 0.8f, 0.8f, 1.0f };

	float emissiveSB[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininessSB = 100.0f;
	int texcountSB = 0;

	memcpy(m_skybox.ambient, ambSB, 4 * sizeof(float));
	memcpy(m_skybox.diffuse, diffSB, 4 * sizeof(float));
	memcpy(m_skybox.specular, specSB, 4 * sizeof(float));
	memcpy(m_skybox.emissive, emissiveSB, 4 * sizeof(float));
	m_skybox.shininess = shininessSB;
	m_skybox.texCount = texcountSB;

	float skybox_pos[3] = { -150.0f, -10.0f, -150.0f };
	skybox = Table(m_skybox, &cubeMesh, skybox_pos, gameObjectsRef.size(), false, 300.0f, 300.0f, 300.0f);
	skybox.PrepareMeshMaterial();
	//gameObjectsRef.push_back(&skybox);

	Material m2;

	float amb2[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	float diff2[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float spec2[] = { 0.3f, 0.3f, 0.3f, 1.0f };

	float emissive2[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess2 = 800.0f;
	int texcount2 = 0;

	float albedo2[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float metallic2 = 0.2f;
	float roughness2 = 0.6f;


	memcpy(m2.ambient, amb2, 4 * sizeof(float));
	memcpy(m2.diffuse, diff2, 4 * sizeof(float));
	memcpy(m2.specular, spec2, 4 * sizeof(float));
	memcpy(m2.emissive, emissive2, 4 * sizeof(float));
	m2.shininess = shininess2;
	m2.texCount = texcount2;

	memcpy(m2.albedo, albedo2, 4 * sizeof(float));
	m2.metallic = metallic2;
	m2.roughness = roughness2;
	m2.ao = 1.0f;

	float tPos2[3] = { 3.0f, -1.45f, -60.0f };

	t2 = Table(m2, &cubeMesh, tPos2, gameObjectsRef.size(), false, 15.0f, 1.5f, 130.0f);

	t2.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t2);

	float tPos21[3] = { -10.0f, -1.45f, 70.0f };

	t21 = Table(m2, &cubeMesh, tPos21, gameObjectsRef.size(), false, 40.0f, 1.5f, 40.0f);


	t21.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t21);

	float tPos22[3] = { 30.0f, -1.45f, 80.0f };

	t22 = Table(m2, &cubeMesh, tPos22, gameObjectsRef.size(), false, 100.0f, 1.5f, 20.0f);

	t22.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t22);

	float tPos23[3] = { 130.0f, -1.45f, 70.0f };

	t23 = Table(m2, &cubeMesh, tPos23, gameObjectsRef.size(), false, 40.0f, 1.5f, 40.0f);

	t23.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t23);

	float tPos24[3] = { 145.0f, -1.45f, -60.0f };

	t24 = Table(m2, &cubeMesh, tPos24, gameObjectsRef.size(), false, 15.0f, 1.5f, 130.0f);

	t24.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t24);

	float tPos25[3] = { -10.0f, -1.45f, -100.0f };

	t25 = Table(m2, &cubeMesh, tPos25, gameObjectsRef.size(), false, 40.0f, 1.5f, 40.0f);


	t25.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t25);

	float tPos26[3] = { 30.0f, -1.45f, -90.0f };

	t26 = Table(m2, &cubeMesh, tPos26, gameObjectsRef.size(), false, 100.0f, 1.5f, 20.0f);

	t26.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t26);

	float tPos27[3] = { 130.0f, -1.45f, -100.0f };

	t27 = Table(m2, &cubeMesh, tPos27, gameObjectsRef.size(), false, 40.0f, 1.5f, 40.0f);

	t27.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t27);

	//Butters
	float tPosButter1[3] = { 5.0f, 0.0f, 5.0f };
	butter1 = Table(m1, &cubeMesh, tPosButter1, gameObjectsRef.size(), true, 4.0f, 1.5f, 4.0f);
	butter1.PrepareMeshMaterial();
	gameObjectsRef.push_back(&butter1);

	float tPosButter2[3] = { 4.0f, 0.0f, 50.0f };
	butter2 = Table(m1, &cubeMesh, tPosButter2, gameObjectsRef.size(), true, 4.0f, 1.5f, 4.0f);
	butter2.PrepareMeshMaterial();
	gameObjectsRef.push_back(&butter2);

	float tPosButter3[3] = { 50.0f, 0.0f, 87.0f }; 
	butter3 = Table(m1, &cubeMesh, tPosButter3, gameObjectsRef.size(), true, 4.0f, 1.5f, 4.0f);
	butter3.PrepareMeshMaterial();
	gameObjectsRef.push_back(&butter3);

	float tPosButter4[3] = { 95.0f, 0.0f, 83.0f };
	butter4 = Table(m1, &cubeMesh, tPosButter4, gameObjectsRef.size(), true, 4.0f, 1.5f, 4.0f);
	butter4.PrepareMeshMaterial();
	gameObjectsRef.push_back(&butter4);

	float tPosButter5[3] = { 155.0f, 0.0f, 10.0f };
	butter5 = Table(m1, &cubeMesh, tPosButter5, gameObjectsRef.size(), true, 4.0f, 1.5f, 4.0f);
	butter5.PrepareMeshMaterial();
	gameObjectsRef.push_back(&butter5);

	float tPosButter6[3] = { 40.0f, 0.0f, -85.0f };
	butter6 = Table(m1, &cubeMesh, tPosButter6, gameObjectsRef.size(), true, 4.0f, 1.5f, 4.0f);
	butter6.PrepareMeshMaterial();
	gameObjectsRef.push_back(&butter6);

	float tPosButter7[3] = { 90.0f, 0.0f, -80.0f };
	butter7 = Table(m1, &cubeMesh, tPosButter7, gameObjectsRef.size(), true, 4.0f, 1.5f, 4.0f);
	butter7.PrepareMeshMaterial();
	gameObjectsRef.push_back(&butter7);

	//Billboards

	Material mtree;

	float tree_spec[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	float tree_shininess = 10.0f;
	float emissive_tree[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	int texcount_tree = 0;

	float tree_albedo[] = { 0.3f, 0.7f, 0.34f, 1.0f };
	float tree_metallic = 0.0f;
	float tree_roughness = 0.35f;

	//memcpy(mtree.ambient, amb3, 4 * sizeof(float));
	//memcpy(mtree.diffuse, diff3, 4 * sizeof(float));
	memcpy(mtree.specular, tree_spec, 4 * sizeof(float));
	memcpy(mtree.emissive, emissive_tree, 4 * sizeof(float));
	mtree.shininess = tree_shininess;
	mtree.texCount = texcount_tree;

	memcpy(mtree.albedo, tree_albedo, 4 * sizeof(float));
	mtree.metallic = tree_metallic;
	mtree.roughness = tree_roughness;
	mtree.ao = 1.0f;

	float tPosBillboard1[3] = { 0.0f, 3.0f, -10.0f };
	b1 = Billboard(mtree, &billboardMesh, tPosBillboard1, gameObjectsRef.size(), false, 1.0f, 1.0f, 1.0f);
	b1.PrepareMeshMaterial();
	gameObjectsRef.push_back(&b1);

	float tPosBillboard2[3] = { 0.0f, 3.0f, 25.0f };
	b2 = Billboard(mtree, &billboardMesh, tPosBillboard2, gameObjectsRef.size(), false, 1.0f, 1.0f, 1.0f);
	b2.PrepareMeshMaterial();
	gameObjectsRef.push_back(&b2);

	float tPosBillboard3[3] = { 20.0f, 3.0f, 25.0f };
	b3 = Billboard(mtree, &billboardMesh, tPosBillboard3, gameObjectsRef.size(), false, 1.0f, 1.0f, 1.0f);
	b3.PrepareMeshMaterial();
	gameObjectsRef.push_back(&b3);

	float tPosBillboard4[3] = { 0.0f, 3.0f, 45.0f };
	b4 = Billboard(mtree, &billboardMesh, tPosBillboard4, gameObjectsRef.size(), false, 1.0f, 1.0f, 1.0f);
	b4.PrepareMeshMaterial();
	gameObjectsRef.push_back(&b4);

	float tPosBillboard5[3] = { 50.0f, 3.0f, 103.0f };
	b5 = Billboard(mtree, &billboardMesh, tPosBillboard5, gameObjectsRef.size(), false, 1.0f, 1.0f, 1.0f);
	b5.PrepareMeshMaterial();
	gameObjectsRef.push_back(&b5);

	float tPosBillboard6[3] = { 120.0f, 3.0f, 103.0f };
	b6 = Billboard(mtree, &billboardMesh, tPosBillboard6, gameObjectsRef.size(), false, 1.0f, 1.0f, 1.0f);
	b6.PrepareMeshMaterial();
	gameObjectsRef.push_back(&b6);

	float tPosBillboard7[3] = { 50.0f, 3.0f, -97.0f };
	b7 = Billboard(mtree, &billboardMesh, tPosBillboard7, gameObjectsRef.size(), false, 1.0f, 1.0f, 1.0f);
	b7.PrepareMeshMaterial();
	gameObjectsRef.push_back(&b7);

	float tPosBillboard8[3] = { 120.0f, 3.0f, -97.0f };
	b8 = Billboard(mtree, &billboardMesh, tPosBillboard8, gameObjectsRef.size(), false, 1.0f, 1.0f, 1.0f);
	b8.PrepareMeshMaterial();
	gameObjectsRef.push_back(&b8);

	float tPosBillboard9[3] = { 163.0f, 3.0f, 0.0f };
	b9 = Billboard(mtree, &billboardMesh, tPosBillboard9, gameObjectsRef.size(), false, 1.0f, 1.0f, 1.0f);
	b9.PrepareMeshMaterial();
	gameObjectsRef.push_back(&b9);


	float tPosBillboard10[3] = { 163.0f, 3.0f, -60.0f };
	b10 = Billboard(mtree, &billboardMesh, tPosBillboard10, gameObjectsRef.size(), false, 1.0f, 1.0f, 1.0f);
	b10.PrepareMeshMaterial();
	gameObjectsRef.push_back(&b10);

	float tPosBillboard11[3] = { 163.0f, 3.0f, 60.0f };
	b11 = Billboard(mtree, &billboardMesh, tPosBillboard11, gameObjectsRef.size(), false, 1.0f, 1.0f, 1.0f);
	b11.PrepareMeshMaterial();
	gameObjectsRef.push_back(&b11);

	float tPosBillboard12[3] = { 20.0f, 3.0f, -50.0f };
	b12 = Billboard(mtree, &billboardMesh, tPosBillboard12, gameObjectsRef.size(), false, 1.0f, 1.0f, 1.0f);
	b12.PrepareMeshMaterial();
	gameObjectsRef.push_back(&b12);

	Material mgrass;

	float amb_grass[] = { 0.13f, 0.54f, 0.13f, 1.0f };
	float diff_grass[] = { 0.13f, 0.54f, 0.13f, 1.0f };
	float spec_grass[] = { 0.3f, 0.3f, 0.3f, 1.0f };

	float emissive_grass[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess_grass = 800.0f;
	int texcount_grass = 0;

	float grass_albedo[] = { 0.13f, 0.54f, 0.13f, 1.0f };
	float grass_metallic = 0.0f;
	float grass_roughness = 0.8f;


	memcpy(mgrass.ambient, amb_grass, 4 * sizeof(float));
	memcpy(mgrass.diffuse, diff_grass, 4 * sizeof(float));
	memcpy(mgrass.specular, spec_grass, 4 * sizeof(float));
	memcpy(mgrass.emissive, emissive_grass, 4 * sizeof(float));
	mgrass.shininess = shininess_grass;
	mgrass.texCount = texcount_grass;

	memcpy(mgrass.albedo, grass_albedo, 4 * sizeof(float));
	mgrass.metallic = grass_metallic;
	mgrass.roughness = grass_roughness;
	mgrass.ao = 1.0f;

	float tPosGrassEl1[3] = { -5.0f, 0.0f, 0.0f };
	g1 = GrassElement(mgrass, &coneMesh, tPosGrassEl1, gameObjectsRef.size(), false, 1.0f, 1.0f, 1.0f);
	g1.PrepareMeshMaterial();
	gameObjectsRef.push_back(&g1);

	float tPosGrassEl2[3] = { -5.0f, 0.0f, -2.0f };
	g2 = GrassElement(mgrass, &coneMesh, tPosGrassEl2, gameObjectsRef.size(), false, 1.0f, 1.0f, 1.0f);
	g2.PrepareMeshMaterial();
	gameObjectsRef.push_back(&g2);

	float tPosGrassEl3[3] = { -5.0f, 0.0f, 2.0f };
	g3 = GrassElement(mgrass, &coneMesh, tPosGrassEl3, gameObjectsRef.size(), false, 1.0f, 1.0f, 1.0f);
	g3.PrepareMeshMaterial();
	gameObjectsRef.push_back(&g3);

	float tPosGrassEl[3] = { -5.0f, 0.0f, 4.0f };
	g = GrassElement(mgrass, &coneMesh, tPosGrassEl, gameObjectsRef.size(), false, 1.0f, 1.0f, 1.0f);
	g.PrepareMeshMaterial();
	gameObjectsRef.push_back(&g);

	float tPosGrassEl4[3] = { 25.0f, 0.0f, 0.0f };
	g4 = GrassElement(mgrass, &coneMesh, tPosGrassEl4, gameObjectsRef.size(), false, 1.0f, 1.0f, 1.0f);
	g4.PrepareMeshMaterial();
	gameObjectsRef.push_back(&g4);

	float tPosGrassEl5[3] = { 25.0f, 0.0f, 2.0f };
	g5 = GrassElement(mgrass, &coneMesh, tPosGrassEl5, gameObjectsRef.size(), false, 1.0f, 1.0f, 1.0f);
	g5.PrepareMeshMaterial();
	gameObjectsRef.push_back(&g5);

	float tPosGrassEl6[3] = { 25.0f, 0.0f, -2.0f };
	g6 = GrassElement(mgrass, &coneMesh, tPosGrassEl6, gameObjectsRef.size(), false, 1.0f, 1.0f, 1.0f);
	g6.PrepareMeshMaterial();
	gameObjectsRef.push_back(&g6);

	float tPosGrassEl7[3] = { 25.0f, 0.0f, 4.0f };
	g7 = GrassElement(mgrass, &coneMesh, tPosGrassEl7, gameObjectsRef.size(), false, 1.0f, 1.0f, 1.0f);
	g7.PrepareMeshMaterial();
	gameObjectsRef.push_back(&g7);

	float tPosGrassEl8[3] = { 25.0f, 0.0f, -4.0f };
	g8 = GrassElement(mgrass, &coneMesh, tPosGrassEl8, gameObjectsRef.size(), false, 1.0f, 1.0f, 1.0f);
	g8.PrepareMeshMaterial();
	gameObjectsRef.push_back(&g8);

	float tPosGrassEl9[3] = { 25.0f, 0.0f, 6.0f };
	g9 = GrassElement(mgrass, &coneMesh, tPosGrassEl9, gameObjectsRef.size(), false, 1.0f, 1.0f, 1.0f);
	g9.PrepareMeshMaterial();
	gameObjectsRef.push_back(&g9);

	float tPosGrassEl10[3] = { 25.0f, 0.0f, -6.0f };
	g10 = GrassElement(mgrass, &coneMesh, tPosGrassEl10, gameObjectsRef.size(), false, 1.0f, 1.0f, 1.0f);
	g10.PrepareMeshMaterial();
	gameObjectsRef.push_back(&g10);

	float tPosGrassEl11[3] = { 25.0f, 0.0f, 8.0f };
	g11 = GrassElement(mgrass, &coneMesh, tPosGrassEl11, gameObjectsRef.size(), false, 1.0f, 1.0f, 1.0f);
	g11.PrepareMeshMaterial();
	gameObjectsRef.push_back(&g11);

	float tPosGrassEl12[3] = { -5.0f, 0.0f, -4.0f };
	g12 = GrassElement(mgrass, &coneMesh, tPosGrassEl12, gameObjectsRef.size(), false, 1.0f, 1.0f, 1.0f);
	g12.PrepareMeshMaterial();
	gameObjectsRef.push_back(&g12);

	float tPosGrassEl13[3] = { -5.0f, 0.0f, 6.0f };
	g13 = GrassElement(mgrass, &coneMesh, tPosGrassEl13, gameObjectsRef.size(), false, 1.0f, 1.0f, 1.0f);
	g13.PrepareMeshMaterial();
	gameObjectsRef.push_back(&g13);

	float tPosGrassEl14[3] = { -5.0f, 0.0f, -6.0f };
	g14 = GrassElement(mgrass, &coneMesh, tPosGrassEl14, gameObjectsRef.size(), false, 1.0f, 1.0f, 1.0f);
	g14.PrepareMeshMaterial();
	gameObjectsRef.push_back(&g14);

	float tPosGrassEl15[3] = { -5.0f, 0.0f, 8.0f };
	g15 = GrassElement(mgrass, &coneMesh, tPosGrassEl15, gameObjectsRef.size(), false, 1.0f, 1.0f, 1.0f);
	g15.PrepareMeshMaterial();
	gameObjectsRef.push_back(&g15);

	float tPosGrassEl16[3] = { -5.0f, 0.0f, -8.0f };
	g16 = GrassElement(mgrass, &coneMesh, tPosGrassEl16, gameObjectsRef.size(), false, 1.0f, 1.0f, 1.0f);
	g16.PrepareMeshMaterial();
	gameObjectsRef.push_back(&g16);


	int index = 0;
	float offset = 0.1f;
	for (int y = -10; y < 10; y += 2)
	{
		for (int x = -10; x < 10; x += 2)
		{
			float translation[2];
			translation[0] = (float)x / 10.0f + offset;
			translation[1] = (float)y / 10.0f + offset;
			translationsX[index] = translation[0];
			translationsZ[index++] = translation[1];
		}
	}

	/*for (int idx = 0; idx < 100; idx++)
	{
		printf("translationsX[%d] = [%f],  ", idx, translationsX[idx]);
		printf("translationsZ[%d] = [%f]\n", idx, translationsZ[idx]);
	}*/

	glGenBuffers(1, &instanceXVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceXVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 100, &translationsX, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(g1.GetMesh()->vao);
	glEnableVertexAttribArray(OFFSET_ATTRIB_X);
	glBindBuffer(GL_ARRAY_BUFFER, instanceXVBO); // this attribute comes from a different vertex buffer
	glVertexAttribPointer(OFFSET_ATTRIB_X, 1, GL_FLOAT, 0, sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glVertexAttribDivisor(OFFSET_ATTRIB_X, 1); // tell OpenGL this is an instanced vertex attribute.

	glGenBuffers(1, &instanceZVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceZVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 100, &translationsZ, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(g1.GetMesh()->vao);
	glEnableVertexAttribArray(OFFSET_ATTRIB_Z);
	glBindBuffer(GL_ARRAY_BUFFER, instanceZVBO); // this attribute comes from a different vertex buffer
	glVertexAttribPointer(OFFSET_ATTRIB_Z, 1, GL_FLOAT, 0, sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glVertexAttribDivisor(OFFSET_ATTRIB_Z, 1); // tell OpenGL this is an instanced vertex attribute.
	

	Material m4;

	float amb4[] = { 0.2f, 0.15f, 0.1f, 1.0f };
	float diff4[] = { 0.8f, 0.6f, 0.4f, 1.0f };
	float spec4[] = { 0.8f, 0.8f, 0.8f, 1.0f };

	float emissive4[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess4 = 800.0f;
	int texcount4 = 0;

	float cheerio_albedo[] = { 0.9f, 0.6f, 0.3f, 1.0f };
	float cheerio_metallic = 0.0f;
	float cheerio_roughness = 0.7f;

	memcpy(m4.ambient, amb4, 4 * sizeof(float));
	memcpy(m4.diffuse, diff4, 4 * sizeof(float));
	memcpy(m4.specular, spec4, 4 * sizeof(float));
	memcpy(m4.emissive, emissive4, 4 * sizeof(float));
	m4.shininess = shininess4;
	m4.texCount = texcount4;

	memcpy(m4.albedo, cheerio_albedo, 4 * sizeof(float));
	m4.metallic = cheerio_metallic;
	m4.roughness = cheerio_roughness;
	m4.ao = 1.0f;


	float tPos111[3] = { 2.0f, alt, 72.0f };
	t111 = Cheerios(m4, &torusMesh, tPos111, gameObjectsRef.size(), rad);

	t111.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t111);

	float tPos112[3] = { 4.0f, alt, 77.0f };
	t112 = Cheerios(m4, &torusMesh, tPos112, gameObjectsRef.size(), rad);

	t112.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t112);


	float tPos113[3] = { 6.0f, alt, 82.0f };
	t113 = Cheerios(m4, &torusMesh, tPos113, gameObjectsRef.size(), rad);

	t113.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t113);


	float tPos114[3] = { 9.0f, alt, 86.0f };
	t114 = Cheerios(m4, &torusMesh, tPos114, gameObjectsRef.size(), rad);

	t114.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t114);

	float tPos115[3] = { 12.0f, alt, 90.0f };
	t115 = Cheerios(m4, &torusMesh, tPos115, gameObjectsRef.size(), rad);

	t115.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t115);

	float tPos116[3] = { 15.0f, alt, 93.0f };
	t116 = Cheerios(m4, &torusMesh, tPos116, gameObjectsRef.size(), rad);

	t116.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t116);

	float tPos117[3] = { 19.0f, alt, 97.0f };
	t117 = Cheerios(m4, &torusMesh, tPos117, gameObjectsRef.size(), rad);

	t117.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t117);

	float tPos118[3] = { 23.0f, alt, 100.0f };
	t118 = Cheerios(m4, &torusMesh, tPos118, gameObjectsRef.size(), rad);

	t118.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t118);

	float tPos119[3] = { 27.0f, alt, 101.0f };
	t119 = Cheerios(m4, &torusMesh, tPos119, gameObjectsRef.size(), rad);

	t119.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t119);

	float tPos120[3] = { 19.0f, alt, 72.0f };
	t120 = Cheerios(m4, &torusMesh, tPos120, gameObjectsRef.size(), rad);

	t120.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t120);


	float tPos121[3] = { 22.0f, alt, 76.0f };
	t121 = Cheerios(m4, &torusMesh, tPos121, gameObjectsRef.size(), rad);

	t121.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t121);

	float tPos122[3] = { 26.0f, alt, 79.0f };
	t122 = Cheerios(m4, &torusMesh, tPos122, gameObjectsRef.size(), rad);

	t122.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t122);










	float rPos1 = 1.0f;
	float rPos2 = 18.0f;

	float tPos11[3] = { rPos1, alt, -57.0f };
	float tPos12[3] = { rPos2, alt, -57.0f };


	t511 = Cheerios(m4, &torusMesh, tPos11, gameObjectsRef.size(), rad);

	t511.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t511);

	t611 = Cheerios(m4, &torusMesh, tPos12, gameObjectsRef.size(), rad);

	t611.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t611);

	tPos11[2] = tPos12[2] += 11.0f;

	t512 = Cheerios(m4, &torusMesh, tPos11, gameObjectsRef.size(), rad);

	t512.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t512);

	t612 = Cheerios(m4, &torusMesh, tPos12, gameObjectsRef.size(), rad);

	t612.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t612);

	tPos11[2] = tPos12[2] += 11.0f;

	t513 = Cheerios(m4, &torusMesh, tPos11, gameObjectsRef.size(), rad);

	t513.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t513);

	t613 = Cheerios(m4, &torusMesh, tPos12, gameObjectsRef.size(), rad);

	t613.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t613);

	tPos11[2] = tPos12[2] += 11.0f;

	t514 = Cheerios(m4, &torusMesh, tPos11, gameObjectsRef.size(), rad);

	t514.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t514);

	t614 = Cheerios(m4, &torusMesh, tPos12, gameObjectsRef.size(), rad);

	t614.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t614);

	tPos11[2] = tPos12[2] += 11.0f;

	t515 = Cheerios(m4, &torusMesh, tPos11, gameObjectsRef.size(), rad);

	t515.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t515);

	t615 = Cheerios(m4, &torusMesh, tPos12, gameObjectsRef.size(), rad);

	t615.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t615);

	tPos11[2] = tPos12[2] += 11.0f;

	t516 = Cheerios(m4, &torusMesh, tPos11, gameObjectsRef.size(), rad);

	t516.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t516);

	t616 = Cheerios(m4, &torusMesh, tPos12, gameObjectsRef.size(), rad);

	t616.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t616);

	tPos11[2] = tPos12[2] += 11.0f;

	t517 = Cheerios(m4, &torusMesh, tPos11, gameObjectsRef.size(), rad);

	t517.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t517);

	t617 = Cheerios(m4, &torusMesh, tPos12, gameObjectsRef.size(), rad);

	t617.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t617);

	tPos11[2] = tPos12[2] += 11.0f;

	t518 = Cheerios(m4, &torusMesh, tPos11, gameObjectsRef.size(), rad);

	t518.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t518);

	t618 = Cheerios(m4, &torusMesh, tPos12, gameObjectsRef.size(), rad);

	t618.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t618);

	tPos11[2] = tPos12[2] += 11.0f;

	t519 = Cheerios(m4, &torusMesh, tPos11, gameObjectsRef.size(), rad);

	t519.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t519);

	t619 = Cheerios(m4, &torusMesh, tPos12, gameObjectsRef.size(), rad);

	t619.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t619);

	tPos11[2] = tPos12[2] += 11.0f;

	t520 = Cheerios(m4, &torusMesh, tPos11, gameObjectsRef.size(), rad);

	t520.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t520);

	t620 = Cheerios(m4, &torusMesh, tPos12, gameObjectsRef.size(), rad);

	t620.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t620);

	tPos11[2] = tPos12[2] += 11.0f;

	t521 = Cheerios(m4, &torusMesh, tPos11, gameObjectsRef.size(), rad);

	t521.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t521);

	t621 = Cheerios(m4, &torusMesh, tPos12, gameObjectsRef.size(), rad);

	t621.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t621);

	tPos11[2] = tPos12[2] += 11.0f;

	t522 = Cheerios(m4, &torusMesh, tPos11, gameObjectsRef.size(), rad);

	t522.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t522);

	t622 = Cheerios(m4, &torusMesh, tPos12, gameObjectsRef.size(), rad);

	t622.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t622);



	float tPos211[3] = { 132.0f, alt, 101.0f };
	t211 = Cheerios(m4, &torusMesh, tPos211, gameObjectsRef.size(), rad);

	t211.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t211);

	float tPos212[3] = {136.0f, alt, 100.0f};
	t212 = Cheerios(m4, &torusMesh, tPos212, gameObjectsRef.size(), rad);

	t212.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t212);

	float tPos213[3] = { 140.0f, alt, 98.0f };
	t213 = Cheerios(m4, &torusMesh, tPos213, gameObjectsRef.size(), rad);

	t213.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t213);

	float tPos214[3] = { 144.0f, alt, 95.0f };
	t214 = Cheerios(m4, &torusMesh, tPos214, gameObjectsRef.size(), rad);

	t214.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t214);

	float tPos215[3] = { 148.0f, alt, 91.0f };
	t215 = Cheerios(m4, &torusMesh, tPos215, gameObjectsRef.size(), rad);

	t215.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t215);

	float tPos216[3] = { 152.0f, alt, 87.0f };
	t216 = Cheerios(m4, &torusMesh, tPos216, gameObjectsRef.size(), rad);

	t216.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t216);

	float tPos217[3] = { 155.0f, alt, 83.0f };
	t217 = Cheerios(m4, &torusMesh, tPos217, gameObjectsRef.size(), rad);

	t217.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t217);


	float tPos218[3] = { 158.0f, alt, 78.0f };
	t218 = Cheerios(m4, &torusMesh, tPos218, gameObjectsRef.size(), rad);

	t218.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t218);

	float tPos219[3] = { 160.0f, alt, 73.0f };
	t219 = Cheerios(m4, &torusMesh, tPos219, gameObjectsRef.size(), rad);

	t219.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t219);

	float tPos220[3] = { 133.0f, alt, 79.0f };
	t220 = Cheerios(m4, &torusMesh, tPos220, gameObjectsRef.size(), rad);

	t220.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t220);


	float tPos221[3] = { 138.0f, alt, 76.0f };
	t221 = Cheerios(m4, &torusMesh, tPos221, gameObjectsRef.size(), rad);

	t221.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t221);

	float tPos222[3] = { 142.0f, alt, 72.0f };
	t222 = Cheerios(m4, &torusMesh, tPos222, gameObjectsRef.size(), rad);

	t222.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t222);


	float rPos3 = 80.0f;
	float rPos4 = 102.0f;

	float tPos13[3] = { 31.0f, alt, rPos3 };
	float tPos14[3] = { 31.0f, alt, rPos4 };

	t711 = Cheerios(m4, &torusMesh, tPos13, gameObjectsRef.size(), rad);

	t711.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t711);

	t811 = Cheerios(m4, &torusMesh, tPos14, gameObjectsRef.size(), rad);

	t811.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t811);

	tPos13[0] = tPos14[0] += 11.0f;

	t712 = Cheerios(m4, &torusMesh, tPos13, gameObjectsRef.size(), rad);

	t712.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t712);

	t812 = Cheerios(m4, &torusMesh, tPos14, gameObjectsRef.size(), rad);

	t812.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t812);

	tPos13[0] = tPos14[0] += 11.0f;

	t713 = Cheerios(m4, &torusMesh, tPos13, gameObjectsRef.size(), rad);

	t713.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t713);

	t813 = Cheerios(m4, &torusMesh, tPos14, gameObjectsRef.size(), rad);

	t813.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t813);

	tPos13[0] = tPos14[0] += 11.0f;

	t714 = Cheerios(m4, &torusMesh, tPos13, gameObjectsRef.size(), rad);

	t714.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t714);

	t814 = Cheerios(m4, &torusMesh, tPos14, gameObjectsRef.size(), rad);

	t814.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t814);

	tPos13[0] = tPos14[0] += 11.0f;

	t715 = Cheerios(m4, &torusMesh, tPos13, gameObjectsRef.size(), rad);

	t715.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t715);

	t815 = Cheerios(m4, &torusMesh, tPos14, gameObjectsRef.size(), rad);

	t815.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t815);

	tPos13[0] = tPos14[0] += 11.0f;

	t716 = Cheerios(m4, &torusMesh, tPos13, gameObjectsRef.size(), rad);

	t716.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t716);

	t816 = Cheerios(m4, &torusMesh, tPos14, gameObjectsRef.size(), rad);

	t816.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t816);

	tPos13[0] = tPos14[0] += 11.0f;

	t717 = Cheerios(m4, &torusMesh, tPos13, gameObjectsRef.size(), rad);

	t717.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t717);

	t817 = Cheerios(m4, &torusMesh, tPos14, gameObjectsRef.size(), rad);

	t817.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t817);

	tPos13[0] = tPos14[0] += 11.0f;

	t718 = Cheerios(m4, &torusMesh, tPos13, gameObjectsRef.size(), rad);

	t718.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t718);

	t818 = Cheerios(m4, &torusMesh, tPos14, gameObjectsRef.size(), rad);

	t818.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t818);

	tPos13[0] = tPos14[0] += 11.0f;

	t719 = Cheerios(m4, &torusMesh, tPos13, gameObjectsRef.size(), rad);

	t719.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t719);

	t819 = Cheerios(m4, &torusMesh, tPos14, gameObjectsRef.size(), rad);

	t819.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t819);

	tPos13[0] = tPos14[0] += 9.0f;

	t720 = Cheerios(m4, &torusMesh, tPos13, gameObjectsRef.size(), rad);

	t720.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t720);

	t820 = Cheerios(m4, &torusMesh, tPos14, gameObjectsRef.size(), rad);

	t820.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t820);


	float tPos311[3] = { 160.0f, alt, -62.0f };
	t311 = Cheerios(m4, &torusMesh, tPos311, gameObjectsRef.size(), rad);

	t311.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t311);

	float tPos312[3] = { 158.0f, alt, -67.0f };
	t312 = Cheerios(m4, &torusMesh, tPos312, gameObjectsRef.size(), rad);

	t312.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t312);


	float tPos313[3] = { 155.0f, alt, -72.0f };
	t313 = Cheerios(m4, &torusMesh, tPos313, gameObjectsRef.size(), rad);

	t313.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t313);


	float tPos314[3] = { 152.0f, alt, -76.0f };
	t314 = Cheerios(m4, &torusMesh, tPos314, gameObjectsRef.size(), rad);

	t314.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t314);

	float tPos315[3] = { 148.0f, alt, -80.0f };
	t315 = Cheerios(m4, &torusMesh, tPos315, gameObjectsRef.size(), rad);

	t315.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t315);

	float tPos316[3] = { 144.0f, alt, -84.0f };
	t316 = Cheerios(m4, &torusMesh, tPos316, gameObjectsRef.size(), rad);

	t316.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t316);

	float tPos317[3] = { 140.0f, alt, -88.0f };
	t317 = Cheerios(m4, &torusMesh, tPos317, gameObjectsRef.size(), rad);

	t317.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t317);

	float tPos318[3] = { 136.0f, alt, -91.0f };
	t318 = Cheerios(m4, &torusMesh, tPos318, gameObjectsRef.size(), rad);

	t318.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t318);

	float tPos319[3] = { 132.0f, alt, -92.0f };
	t319 = Cheerios(m4, &torusMesh, tPos319, gameObjectsRef.size(), rad);

	t319.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t319);

	float tPos320[3] = { 142.0f, alt, -62.0f };
	t320 = Cheerios(m4, &torusMesh, tPos320, gameObjectsRef.size(), rad);

	t320.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t320);


	float tPos321[3] = { 138.0f, alt, -66.0f };
	t321 = Cheerios(m4, &torusMesh, tPos321, gameObjectsRef.size(), rad);

	t321.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t321);

	float tPos322[3] = { 133.0f, alt, -69.0f };
	t322 = Cheerios(m4, &torusMesh, tPos322, gameObjectsRef.size(), rad);

	t322.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t322);



	rPos1 = 144.0f;
	rPos2 = 162.0f;

	tPos11[0] = rPos1;
	tPos12[0] = rPos2,
	tPos11[2] = tPos12[2] = -57.0f;


	t911 = Cheerios(m4, &torusMesh, tPos11, gameObjectsRef.size(), rad);

	t911.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t911);

	t1011 = Cheerios(m4, &torusMesh, tPos12, gameObjectsRef.size(), rad);

	t1011.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1011);

	tPos11[2] = tPos12[2] += 11.0f;

	t912 = Cheerios(m4, &torusMesh, tPos11, gameObjectsRef.size(), rad);

	t912.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t912);

	t1012 = Cheerios(m4, &torusMesh, tPos12, gameObjectsRef.size(), rad);

	t1012.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1012);

	tPos11[2] = tPos12[2] += 11.0f;

	t913 = Cheerios(m4, &torusMesh, tPos11, gameObjectsRef.size(), rad);

	t913.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t913);

	t1013 = Cheerios(m4, &torusMesh, tPos12, gameObjectsRef.size(), rad);

	t1013.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1013);

	tPos11[2] = tPos12[2] += 11.0f;

	t914 = Cheerios(m4, &torusMesh, tPos11, gameObjectsRef.size(), rad);

	t914.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t914);

	t1014 = Cheerios(m4, &torusMesh, tPos12, gameObjectsRef.size(), rad);

	t1014.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1014);

	tPos11[2] = tPos12[2] += 11.0f;

	t915 = Cheerios(m4, &torusMesh, tPos11, gameObjectsRef.size(), rad);

	t915.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t915);

	t1015 = Cheerios(m4, &torusMesh, tPos12, gameObjectsRef.size(), rad);

	t1015.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1015);

	tPos11[2] = tPos12[2] += 11.0f;

	t916 = Cheerios(m4, &torusMesh, tPos11, gameObjectsRef.size(), rad);

	t916.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t916);

	t1016 = Cheerios(m4, &torusMesh, tPos12, gameObjectsRef.size(), rad);

	t1016.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1016);

	tPos11[2] = tPos12[2] += 11.0f;

	t917 = Cheerios(m4, &torusMesh, tPos11, gameObjectsRef.size(), rad);

	t917.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t917);

	t1017 = Cheerios(m4, &torusMesh, tPos12, gameObjectsRef.size(), rad);

	t1017.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1017);

	tPos11[2] = tPos12[2] += 11.0f;

	t918 = Cheerios(m4, &torusMesh, tPos11, gameObjectsRef.size(), rad);

	t918.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t918);

	t1018 = Cheerios(m4, &torusMesh, tPos12, gameObjectsRef.size(), rad);

	t1018.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1018);

	tPos11[2] = tPos12[2] += 11.0f;

	t919 = Cheerios(m4, &torusMesh, tPos11, gameObjectsRef.size(), rad);

	t919.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t919);

	t1019 = Cheerios(m4, &torusMesh, tPos12, gameObjectsRef.size(), rad);

	t1019.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1019);

	tPos11[2] = tPos12[2] += 11.0f;

	t920 = Cheerios(m4, &torusMesh, tPos11, gameObjectsRef.size(), rad);

	t920.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t920);

	t1020 = Cheerios(m4, &torusMesh, tPos12, gameObjectsRef.size(), rad);

	t1020.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1020);

	tPos11[2] = tPos12[2] += 11.0f;

	t921 = Cheerios(m4, &torusMesh, tPos11, gameObjectsRef.size(), rad);

	t921.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t921);

	t1021 = Cheerios(m4, &torusMesh, tPos12, gameObjectsRef.size(), rad);

	t1021.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1021);

	tPos11[2] = tPos12[2] += 11.0f;

	t922 = Cheerios(m4, &torusMesh, tPos11, gameObjectsRef.size(), rad);

	t922.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t922);

	t1022 = Cheerios(m4, &torusMesh, tPos12, gameObjectsRef.size(), rad);

	t1022.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1022);





	float tPos411[3] = { 2.0f, alt, -62.0f };
	t411 = Cheerios(m4, &torusMesh, tPos411, gameObjectsRef.size(), rad);

	t411.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t411);

	float tPos412[3] = { 4.0f, alt, -67.0f };
	t412 = Cheerios(m4, &torusMesh, tPos412, gameObjectsRef.size(), rad);

	t412.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t412);


	float tPos413[3] = { 6.0f, alt, -72.0f };
	t413 = Cheerios(m4, &torusMesh, tPos413, gameObjectsRef.size(), rad);

	t413.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t413);


	float tPos414[3] = { 9.0f, alt, -76.0f };
	t414 = Cheerios(m4, &torusMesh, tPos414, gameObjectsRef.size(), rad);

	t414.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t414);

	float tPos415[3] = { 12.0f, alt, -80.0f };
	t415 = Cheerios(m4, &torusMesh, tPos415, gameObjectsRef.size(), rad);

	t415.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t415);

	float tPos416[3] = { 15.0f, alt, -84.0f };
	t416 = Cheerios(m4, &torusMesh, tPos416, gameObjectsRef.size(), rad);

	t416.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t416);

	float tPos417[3] = { 19.0f, alt, -88.0f };
	t417 = Cheerios(m4, &torusMesh, tPos417, gameObjectsRef.size(), rad);

	t417.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t417);

	float tPos418[3] = { 23.0f, alt, -91.0f };
	t418 = Cheerios(m4, &torusMesh, tPos418, gameObjectsRef.size(), rad);

	t418.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t418);

	float tPos419[3] = { 27.0f, alt, -92.0f };
	t419 = Cheerios(m4, &torusMesh, tPos419, gameObjectsRef.size(), rad);

	t419.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t419);

	float tPos420[3] = { 19.0f, alt, -62.0f };
	t420 = Cheerios(m4, &torusMesh, tPos420, gameObjectsRef.size(), rad);

	t420.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t420);


	float tPos421[3] = { 22.0f, alt, -66.0f };
	t421 = Cheerios(m4, &torusMesh, tPos421, gameObjectsRef.size(), rad);

	t421.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t421);

	float tPos422[3] = { 26.0f, alt, -69.0f };
	t422 = Cheerios(m4, &torusMesh, tPos422, gameObjectsRef.size(), rad);

	t422.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t422);



	rPos3 = -93.0f;
	rPos4 = -71.0f;

	tPos13[0] = 31.0f;
	tPos13[2] = rPos3;
	tPos14[0] = 31.0f;
	tPos14[2] = rPos4;

	t1111 = Cheerios(m4, &torusMesh, tPos13, gameObjectsRef.size(), rad);

	t1111.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1111);

	t1211 = Cheerios(m4, &torusMesh, tPos14, gameObjectsRef.size(), rad);

	t1211.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1211);

	tPos13[0] = tPos14[0] += 11.0f;

	t1112 = Cheerios(m4, &torusMesh, tPos13, gameObjectsRef.size(), rad);

	t1112.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1112);

	t1212 = Cheerios(m4, &torusMesh, tPos14, gameObjectsRef.size(), rad);

	t1212.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1212);

	tPos13[0] = tPos14[0] += 11.0f;

	t1113 = Cheerios(m4, &torusMesh, tPos13, gameObjectsRef.size(), rad);

	t1113.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1113);

	t1213 = Cheerios(m4, &torusMesh, tPos14, gameObjectsRef.size(), rad);

	t1213.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1213);

	tPos13[0] = tPos14[0] += 11.0f;

	t1114 = Cheerios(m4, &torusMesh, tPos13, gameObjectsRef.size(), rad);

	t1114.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1114);

	t1214 = Cheerios(m4, &torusMesh, tPos14, gameObjectsRef.size(), rad);

	t1214.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1214);

	tPos13[0] = tPos14[0] += 11.0f;

	t1115 = Cheerios(m4, &torusMesh, tPos13, gameObjectsRef.size(), rad);

	t1115.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1115);

	t1215 = Cheerios(m4, &torusMesh, tPos14, gameObjectsRef.size(), rad);

	t1215.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1215);

	tPos13[0] = tPos14[0] += 11.0f;

	t1116 = Cheerios(m4, &torusMesh, tPos13, gameObjectsRef.size(), rad);

	t1116.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1116);

	t1216 = Cheerios(m4, &torusMesh, tPos14, gameObjectsRef.size(), rad);

	t1216.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1216);

	tPos13[0] = tPos14[0] += 11.0f;

	t1117 = Cheerios(m4, &torusMesh, tPos13, gameObjectsRef.size(), rad);

	t1117.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1117);

	t1217 = Cheerios(m4, &torusMesh, tPos14, gameObjectsRef.size(), rad);

	t1217.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1217);

	tPos13[0] = tPos14[0] += 11.0f;

	t1118 = Cheerios(m4, &torusMesh, tPos13, gameObjectsRef.size(), rad);

	t1118.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1118);

	t1218 = Cheerios(m4, &torusMesh, tPos14, gameObjectsRef.size(), rad);

	t1218.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1218);

	tPos13[0] = tPos14[0] += 11.0f;

	t1119 = Cheerios(m4, &torusMesh, tPos13, gameObjectsRef.size(), rad);

	t1119.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1119);

	t1219 = Cheerios(m4, &torusMesh, tPos14, gameObjectsRef.size(), rad);

	t1219.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1219);

	tPos13[0] = tPos14[0] += 9.0f;

	t1120 = Cheerios(m4, &torusMesh, tPos13, gameObjectsRef.size(), rad);

	t1120.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1120);

	t1220 = Cheerios(m4, &torusMesh, tPos14, gameObjectsRef.size(), rad);

	t1220.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t1220);
	Material m3;
	float amb3[] = { 1.0f, 0.5f, 0.3f, 1.0f };
	float diff3[] = { 0.8f, 0.6f, 0.4f, 1.0f };
	float spec3[] = { 0.0f, 0.0f, 0.0f, 1.0f };

	float emissive3[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess3 = 0.0f;
	int texcount3 = 0;

	float orange_albedo[] = { 1.0f, 0.5f, 0.0f, 1.0f };
	float orange_metallic = 0.0f;
	float orange_roughness = 0.5f;

	memcpy(m3.ambient, amb3, 4 * sizeof(float));
	memcpy(m3.diffuse, diff3, 4 * sizeof(float));
	memcpy(m3.specular, spec3, 4 * sizeof(float));
	memcpy(m3.emissive, emissive3, 4 * sizeof(float));
	m3.shininess = shininess3;
	m3.texCount = texcount3;

	memcpy(m3.albedo, orange_albedo, 4 * sizeof(float));
	m3.metallic = orange_metallic;
	m3.roughness = orange_roughness;
	m3.ao = 1.0f;

	o1 = Orange(m3, &sphereMesh, 1.0f, gameObjectsRef.size(), tPos, table_length, true);
	o1.PrepareMeshMaterial();
	gameObjectsRef.push_back(&o1);

	o2 = Orange(m3, &sphereMesh, 1.0f, gameObjectsRef.size(), tPos, table_length, true);
	o2.PrepareMeshMaterial();
	gameObjectsRef.push_back(&o2);

	o3 = Orange(m3, &sphereMesh, 1.0f, gameObjectsRef.size(), tPos, table_length, true);
	o3.PrepareMeshMaterial();
	gameObjectsRef.push_back(&o3);

	o4 = Orange(m3, &sphereMesh, 1.0f, gameObjectsRef.size(), tPos, table_length, true);
	o4.PrepareMeshMaterial();
	gameObjectsRef.push_back(&o4);

	o5 = Orange(m3, &sphereMesh, 1.0f, gameObjectsRef.size(), tPos, table_length, true);
	o5.PrepareMeshMaterial();
	gameObjectsRef.push_back(&o5);

	o6 = Orange(m3, &sphereMesh, 1.0f, gameObjectsRef.size(), tPos, table_length, true);
	o6.PrepareMeshMaterial();
	gameObjectsRef.push_back(&o6);

	o7 = Orange(m3, &sphereMesh, 1.0f, gameObjectsRef.size(), tPos, table_length, true);
	o7.PrepareMeshMaterial();
	gameObjectsRef.push_back(&o7);

	o8 = Orange(m3, &sphereMesh, 1.0f, gameObjectsRef.size(), tPos, table_length, true);
	o8.PrepareMeshMaterial();
	gameObjectsRef.push_back(&o8);

	o9 = Orange(m3, &sphereMesh, 1.0f, gameObjectsRef.size(), tPos, table_length, true);
	o9.PrepareMeshMaterial();
	gameObjectsRef.push_back(&o9);

	o10 = Orange(m3, &sphereMesh, 1.0f, gameObjectsRef.size(), tPos, table_length, true);
	o10.PrepareMeshMaterial();
	gameObjectsRef.push_back(&o10);

	o11 = Orange(m3, &sphereMesh, 1.0f, gameObjectsRef.size(), tPos, table_length, true);
	o11.PrepareMeshMaterial();
	gameObjectsRef.push_back(&o11);

	o12 = Orange(m3, &sphereMesh, 1.0f, gameObjectsRef.size(), tPos, table_length, true);
	o12.PrepareMeshMaterial();
	gameObjectsRef.push_back(&o12);

	o13 = Orange(m3, &sphereMesh, 1.0f, gameObjectsRef.size(), tPos, table_length, true);
	o13.PrepareMeshMaterial();
	gameObjectsRef.push_back(&o13);

	o14 = Orange(m3, &sphereMesh, 1.0f, gameObjectsRef.size(), tPos, table_length, true);
	o14.PrepareMeshMaterial();
	gameObjectsRef.push_back(&o14);

	o15 = Orange(m3, &sphereMesh, 1.0f, gameObjectsRef.size(), tPos, table_length, true);
	o15.PrepareMeshMaterial();
	gameObjectsRef.push_back(&o15);

	float tPosbutter99[3] = { 0.0f, 0.0f, -10.0f };
	butter99 = Table(m1, &cubeMesh, tPosbutter99, gameObjectsRef.size(), true, 5.0f, 1.5f, 5.0f);
	butter99.PrepareMeshMaterial();
	gameObjectsRef.push_back(&butter99);

	t = Table(m1, &cubeMesh, tPos, gameObjectsRef.size(), false, table_length, 0.001f, table_length);
	t.PrepareMeshMaterial();
	gameObjectsRef.push_back(&t);


	// some GL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);		 // cull back face
	glFrontFace(GL_CCW); // set counter-clockwise vertex order to mean the front
	//glEnable(GL_MULTISAMPLE);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	//glEnable(GL_FRAMEBUFFER_SRGB);  //it does automatic gamma correction but the outcome is not pleasing to the eyes
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(VERTEX_COORD_ATTRIB);
		glVertexAttribPointer(VERTEX_COORD_ATTRIB, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(TEXTURE_COORD_ATTRIB);
		glVertexAttribPointer(TEXTURE_COORD_ATTRIB, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

// ------------------------------------------------------------
//
// Main function
//
int main(int argc, char** argv) {

	//  GLUT initialization
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);

	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WinX, WinY);
	WindowHandle = glutCreateWindow(CAPTION);

	initCameras();

	//  Callback Registration
	glutDisplayFunc(renderLoop);
	glutReshapeFunc(resizeCallbackWrapper); 

	glutTimerFunc(0, timer, 0);
	glutIdleFunc(renderLoop);  // Use it for maximum performance
	//glutTimerFunc(0, refresh, 0);    //use it to to get 60 FPS whatever

//	Mouse and Keyboard Callbacks
	glutKeyboardFunc(processKeys);
	glutMouseFunc(processMouseButtons);
	glutMotionFunc(processMouseMotion);
	glutMouseWheelFunc(mouseWheel);
	glutKeyboardUpFunc(processKeyUp);

	//	return from main loop
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	//	Init GLEW
	glewExperimental = GL_TRUE;
	glewInit();

	printf("Vendor: %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version: %s\n", glGetString(GL_VERSION));
	printf("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	if (!setupShaders())
		return(1);

	init();

	//  GLUT main loop
	glutMainLoop();

	return(0);
}

unsigned int getTextureId(char* name) {
	int i;

	for (i = 0; i < NTEXTURES; ++i)
	{
		if (strncmp(name, flareTextureNames[i], strlen(name)) == 0)
			return i;
	}
	return -1;
}

void loadFlareFile(FLARE_DEF* flare, char* filename)
{
	int     n = 0;
	FILE* f;
	char    buf[256];
	int fields;

	memset(flare, 0, sizeof(FLARE_DEF));

	f = fopen(filename, "r");
	if (f)
	{
		fgets(buf, sizeof(buf), f);
		sscanf(buf, "%f %f", &flare->fScale, &flare->fMaxSize);

		while (!feof(f))
		{
			char            name[8] = { '\0', };
			double          dDist = 0.0, dSize = 0.0;
			float			color[4];
			int				id;

			fgets(buf, sizeof(buf), f);
			fields = sscanf(buf, "%4s %lf %lf ( %f %f %f %f )", name, &dDist, &dSize, &color[3], &color[0], &color[1], &color[2]);
			if (fields == 7)
			{
				for (int i = 0; i < 4; ++i) color[i] = clamp(color[i] / 255.0f, 0.0f, 1.0f);
				id = getTextureId(name);
				if (id < 0) printf("Texture name not recognized\n");
				else
					flare->element[n].textureId = id;
				flare->element[n].fDistance = (float)dDist;
				flare->element[n].fSize = (float)dSize;
				memcpy(flare->element[n].matDiffuse, color, 4 * sizeof(float));
				++n;
			}
		}

		flare->nPieces = n;
		fclose(f);
	}
	else printf("Flare file opening error\n");
}
