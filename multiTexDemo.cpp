////
//// AVT Multitexturing light with spotlight
////   
////
//// This demo was built for learning purposes only.
//// Some code could be severely optimised, but I tried to
//// keep as simple and clear as possible.
////
//// The code comes with no warranties, use it at your own risk.
//// You may use it, or parts of it, wherever you want.
////
////
////Author: João Madeiras Pereira
//
//
////USE KEY 'l' TO ENABLE SPOTLIGTH
//
//
//#pragma comment(lib, "DevIL.lib")
//#pragma comment(lib, "ILU.lib")
////#pragma comment(lib, "ILUT.lib")
//
//#include <math.h>
//#include <iostream>
//#include <sstream>
//
//#include <string>
//
//// include GLEW to access OpenGL 3.3 functions
//#include <GL/glew.h>
//
//// GLUT is the toolkit to interface with the OS
//#include <GL/freeglut.h>
//
//#include <IL/il.h>
////#include <IL/ilu.h>
////#include <IL/ilut.h>
//
//// Use Very Simple Libs
//#include "VSShaderlib.h"
//#include "AVTmathLib.h"
//#include "VertexAttrDef.h"
//#include "geometry.h"
//#include "Texture_Loader.h"
//
//#define CAPTION "AVT MultiTexture Demo"
//#define FPS 30
//int WindowHandle = 0;
//int WinX = 1024, WinY = 768;
//
//unsigned int FrameCount = 0;
//
//VSShaderLib shader;
//
//std::vector<struct MyMesh> myMeshes;
//
////External array storage defined in AVTmathLib.cpp
//
///// The storage for matrices
//extern float mMatrix[COUNT_MATRICES][16];
//extern float mCompMatrix[COUNT_COMPUTED_MATRICES][16];
//
///// The normal matrix
//extern float mNormal3x3[9];
//
//GLint pvm_uniformId;
//GLint vm_uniformId;
//GLint normal_uniformId;
//GLint lPos_uniformId;
//GLint tex_loc, tex_loc1, tex_loc2;
//GLint texMode_uniformId;
//
//GLuint TextureArray[3];
//
//	
//// Camera Position
//float camX, camY, camZ;
//
//// Mouse Tracking Variables
//int startX, startY, tracking = 0;
//
//// Camera Spherical Coordinates
//float alpha = 57.0f, beta = 18.0f;
//float r = 45.0f;
//
//// Frame counting and FPS computation
//long myTime,timebase = 0,frame = 0;
//char s[32];
//
////float lightPos[4] = {4.0f, 1.0f, 2.0f, 1.0f};
//float lightPos[4] = { 0.0f, 5.0f, 0.0f, 1.0f };
//
//bool spotlight_mode = false;
//
//
//void timer(int value)
//{
//	std::ostringstream oss;
//	oss << CAPTION << ": " << FrameCount << " FPS @ (" << WinX << "x" << WinY << ")";
//	std::string s = oss.str();
//	glutSetWindow(WindowHandle);
//	glutSetWindowTitle(s.c_str());
//    FrameCount = 0;
//    glutTimerFunc(1000, timer, 0);
//}
//
//void refresh(int value)
//{
//	glutPostRedisplay();
//	glutTimerFunc(1000/FPS, refresh, 0);
//}
//
//// ------------------------------------------------------------
////
//// Reshape Callback Function
////
//
//void changeSize(int w, int h) {
//
//	float ratio;
//	// Prevent a divide by zero, when window is too short
//	if(h == 0)
//		h = 1;
//	// set the viewport to be the entire window
//	glViewport(0, 0, w, h);
//	// set the projection matrix
//	ratio = (1.0f * w) / h;
//	loadIdentity(PROJECTION);
//	perspective(53.13f, ratio, 0.1f, 1000.0f);
//}
//
//
//// ------------------------------------------------------------
////
//// Render stufff
////
//
//void renderScene(void) {
//
//	GLint loc;
//	float res[4];
//	
//	FrameCount++;
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	// load identity matrices
//	loadIdentity(VIEW);
//	loadIdentity(MODEL);
//	// set the camera using a function similar to gluLookAt
//	lookAt(camX, camY, camZ, 0,0,0, 0,1,0);
//	// use our shader
//	glUseProgram(shader.getProgramIndex());
//
//	//send the light position in eye coordinates
//	glUniform4fv(lPos_uniformId, 1, lightPos); //efeito capacete do mineiro, ou seja lighPos foi definido em eye coord 
//	
//	/*
//	multMatrixPoint(VIEW, lightPos,res);   //lightPos definido em World Coord so it is converted to eye space
//	glUniform4fv(lPos_uniformId, 1, res);
//	*/
//
//	loc = glGetUniformLocation(shader.getProgramIndex(), "spotlight_mode");
//	if(spotlight_mode)
//		glUniform1i(loc, 1);
//	else
//		glUniform1i(loc, 0);
//
//	loc = glGetUniformLocation(shader.getProgramIndex(), "coneDir"); 
//
//	/*
//	float coneDir[4] = { -camX, -camY, -camZ, 0.0f };  //direction in world coordinates
//
//	multMatrixPoint(VIEW, coneDir, res);  //Transform the cone dir to eye coordinates
//	glUniform4fv(loc, 1, res);
//	*/
//
//	float coneDir[4] = { 0.0f, 0.0f, -1.0f, 0.0f };  //already in eye coordinates
//	glUniform4fv(loc, 1,coneDir);  
//	loc = glGetUniformLocation(shader.getProgramIndex(), "spotCosCutOff");
//	glUniform1f(loc, 0.93f);
//
//	
//	//Associar os Texture Units aos Objects Texture
//	//stone.tga loaded in TU0; checker.tga loaded in TU1;  lightwood.tga loaded in TU2
//
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, TextureArray[0]);  
//
//	glActiveTexture(GL_TEXTURE1);
//	glBindTexture(GL_TEXTURE_2D, TextureArray[1]);
//
//	glActiveTexture(GL_TEXTURE2);
//	glBindTexture(GL_TEXTURE_2D, TextureArray[2]);
//
//	//Indicar aos tres samplers do GLSL quais os Texture Units a serem usados
//	glUniform1i(tex_loc, 0);  
//	glUniform1i(tex_loc1, 1); 
//	glUniform1i(tex_loc2, 2); 
//
//
//	// Draw the floor
//	// myMeshes[0] contains the cube object
//
//	// send the material
//	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
//	glUniform4fv(loc, 1, myMeshes[0].mat.ambient);
//	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
//	glUniform4fv(loc, 1, myMeshes[0].mat.diffuse);
//	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
//	glUniform4fv(loc, 1, myMeshes[0].mat.specular);
//	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
//	//glUniform1f(loc, mesh[objId].mat.shininess);
//	glUniform1f(loc, 20.0f);
//	pushMatrix(MODEL);
//	translate(MODEL, 0.0f, -1.15f, 0.0f);
//	scale(MODEL, 75.0f, 0.01f, 75.0f);
//	translate(MODEL, -0.5f, -0.5f, -0.5f); //centrar o cubo na origem
//
//	// send matrices to OGL
//	computeDerivedMatrix(PROJ_VIEW_MODEL);
//	glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
//	glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
//	computeNormalMatrix3x3();
//	glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);
//
//	// Render mesh
//	glUniform1i(texMode_uniformId, 0); //modulate Phong color with texel color
//	glBindVertexArray(myMeshes[0].vao);
//	glDrawElements(myMeshes[0].type, myMeshes[0].numIndexes, GL_UNSIGNED_INT, 0);
//	glBindVertexArray(0);
//	popMatrix(MODEL);
//		
//	//Draw the other objects
//	int objId = 0; //id of the object mesh - to be used as index of mesh: Mymeshes[objID] means the current mesh
//	
//	for (int i = 0 ; i < 3; ++i) {
//		for (int j = 0; j < 4; ++j) {
//
//			// send the material
//			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
//			glUniform4fv(loc, 1, myMeshes[objId].mat.ambient);
//			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
//			glUniform4fv(loc, 1, myMeshes[objId].mat.diffuse);
//			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
//			glUniform4fv(loc, 1, myMeshes[objId].mat.specular);
//			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
//			glUniform1f(loc, myMeshes[objId].mat.shininess);
//			pushMatrix(MODEL);
//			translate(MODEL, i*3.7f, 0.0f, j*3.7f);
//
//			// send matrices to OGL
//			computeDerivedMatrix(PROJ_VIEW_MODEL);
//			glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
//			glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
//			computeNormalMatrix3x3();
//			glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);
//
//			// Render mesh
//
//			if (i==0) glUniform1i(texMode_uniformId, 0); // modulate Phong color with texel color
//			else if( i==1) glUniform1i(texMode_uniformId, 1); // multitexturing
//			else glUniform1i(texMode_uniformId, 2); // diffuse color is replaced by texel color, with specular area
//
//			glBindVertexArray(myMeshes[objId].vao);
//			glDrawElements(myMeshes[objId].type, myMeshes[objId].numIndexes, GL_UNSIGNED_INT, 0);
//			glBindVertexArray(0);
//
//			popMatrix(MODEL);
//			objId = (objId+1)%6; 
//		}
//	}
//
//	glBindTexture(GL_TEXTURE_2D, 0);		
//	glutSwapBuffers();
//}
//
//// ------------------------------------------------------------
////
//// Events from the Keyboard
////
//
//void processKeys(unsigned char key, int xx, int yy)
//{
//	switch(key) {
//
//		case 27:
//			glutLeaveMainLoop();
//			break;
//
//		case 'r':
//			// Camera Spherical Coordinates
//			alpha = 57.0f; beta = 18.0f;
//			r = 45.0f;
//			camX = r * sin(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
//			camZ = r * cos(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
//			camY = r * sin(beta * 3.14f / 180.0f);
//			break;
//
//		case 'c': 
//			printf("Camera Spherical Coordinates (%f, %f, %f)\n", alpha, beta, r);
//			break;
//
//		case 'l':
//			if (!spotlight_mode) {
//				spotlight_mode = true;
//				printf("Point light disabled. Spot light enabled");
//			}
//			else {
//				spotlight_mode = false;
//				printf("Spot light disabled. Point light enabled");
//			}
//			break;
//
//		case 'm': glEnable(GL_MULTISAMPLE); break;
//		case 'n': glDisable(GL_MULTISAMPLE); break;
//	}
//}
//
//
//// ------------------------------------------------------------
////
//// Mouse Events
////
//
//void processMouseButtons(int button, int state, int xx, int yy)
//{
//	// start tracking the mouse
//	if (state == GLUT_DOWN)  {
//		startX = xx;
//		startY = yy;
//		if (button == GLUT_LEFT_BUTTON)
//			tracking = 1;
//		else if (button == GLUT_RIGHT_BUTTON)
//			tracking = 2;
//	}
//
//	//stop tracking the mouse
//	else if (state == GLUT_UP) {
//		if (tracking == 1) {
//			alpha -= (xx - startX);
//			beta += (yy - startY);
//		}
//		else if (tracking == 2) {
//			r += (yy - startY) * 0.01f;
//			if (r < 0.1f)
//				r = 0.1f;
//		}
//		tracking = 0;
//	}
//}
//
//// Track mouse motion while buttons are pressed
//
//void processMouseMotion(int xx, int yy)
//{
//
//	int deltaX, deltaY;
//	float alphaAux, betaAux;
//	float rAux;
//
//	deltaX =  - xx + startX;
//	deltaY =    yy - startY;
//
//	// left mouse button: move camera
//	if (tracking == 1) {
//
//
//		alphaAux = alpha + deltaX;
//		betaAux = beta + deltaY;
//
//		if (betaAux > 85.0f)
//			betaAux = 85.0f;
//		else if (betaAux < -85.0f)
//			betaAux = -85.0f;
//		rAux = r;
//	}
//	// right mouse button: zoom
//	else if (tracking == 2) {
//
//		alphaAux = alpha;
//		betaAux = beta;
//		rAux = r + (deltaY * 0.01f);
//		if (rAux < 0.1f)
//			rAux = 0.1f;
//	}
//
//	camX = rAux * sin(alphaAux * 3.14f / 180.0f) * cos(betaAux * 3.14f / 180.0f);
//	camZ = rAux * cos(alphaAux * 3.14f / 180.0f) * cos(betaAux * 3.14f / 180.0f);
//	camY = rAux *   						       sin(betaAux * 3.14f / 180.0f);
//
////  uncomment this if not using an idle func
//	//glutPostRedisplay();
//}
//
//
//void mouseWheel(int wheel, int direction, int x, int y) {
//
//	r += direction * 0.1f;
//	if (r < 0.1f)
//		r = 0.1f;
//
//	camX = r * sin(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
//	camZ = r * cos(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
//	camY = r *   						     sin(beta * 3.14f / 180.0f);
//
////  uncomment this if not using an idle func
//	//glutPostRedisplay();
//}
//
//// --------------------------------------------------------
////
//// Shader Stuff
////
//
//
//GLuint setupShaders() {
//
//	// Shader for models
//	shader.init();
//	shader.loadShader(VSShaderLib::VERTEX_SHADER, "shaders/texture_demo.vert");
//	shader.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/texture_demo.frag");
//
//	// set semantics for the shader variables
//	glBindFragDataLocation(shader.getProgramIndex(), 0,"colorOut");
//	glBindAttribLocation(shader.getProgramIndex(), VERTEX_COORD_ATTRIB, "position");
//	glBindAttribLocation(shader.getProgramIndex(), NORMAL_ATTRIB, "normal");
//	glBindAttribLocation(shader.getProgramIndex(), TEXTURE_COORD_ATTRIB, "texCoord");
//
//	glLinkProgram(shader.getProgramIndex());
//
//	texMode_uniformId = glGetUniformLocation(shader.getProgramIndex(), "texMode"); // different modes of texturing
//	pvm_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_pvm");
//	vm_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_viewModel");
//	normal_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_normal");
//	lPos_uniformId = glGetUniformLocation(shader.getProgramIndex(), "l_pos");
//	tex_loc = glGetUniformLocation(shader.getProgramIndex(), "texmap");
//	tex_loc1 = glGetUniformLocation(shader.getProgramIndex(), "texmap1");
//	tex_loc2 = glGetUniformLocation(shader.getProgramIndex(), "texmap2");
//	
//	printf("InfoLog for Shader\n%s\n\n", shader.getAllInfoLogs().c_str());
//	
//	return(shader.isProgramLinked());
//}
//
//// ------------------------------------------------------------
////
//// Model loading and OpenGL setup
////
//
//void init()
//{
//	MyMesh amesh;
//
//	// set the camera position based on its spherical coordinates
//	camX = r * sin(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
//	camZ = r * cos(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
//	camY = r *   						     sin(beta * 3.14f / 180.0f);
//
//	/* Initialization of DevIL */
//	if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION)
//	{
//		printf("wrong DevIL version \n");
//		exit(0);
//	}
//	ilInit();
//
//	//as libs abaixo não são necessárias
//	/*
//	iluInit();
//	ilutRenderer(ILUT_OPENGL);
//	*/
//
//	//Texture Object definition
//	
//	glGenTextures(3, TextureArray);
//	Texture2D_Loader(TextureArray, "stone.tga", 0);
//	Texture2D_Loader(TextureArray, "checker.png", 1);
//	Texture2D_Loader(TextureArray, "lightwood.tga", 2);
//
//	float amb[]= {0.2f, 0.15f, 0.1f, 1.0f};
//	float diff[] = {0.8f, 0.6f, 0.4f, 1.0f};
//	float spec[] = {0.8f, 0.8f, 0.8f, 1.0f};
//
//	float amb1[]= {0.3f, 0.0f, 0.0f, 1.0f};
//	float diff1[] = {0.8f, 0.1f, 0.1f, 1.0f};
//	float spec1[] = {0.3f, 0.3f, 0.3f, 1.0f};
//	
//
//	float emissive[] = {0.0f, 0.0f, 0.0f, 1.0f};
//	float shininess= 100.0f;
//	int texcount = 0;
//
//	// create geometry and VAO of the cube
//	amesh = createCube();
//	memcpy(amesh.mat.ambient, amb1, 4 * sizeof(float));
//	memcpy(amesh.mat.diffuse, diff1, 4 * sizeof(float));
//	memcpy(amesh.mat.specular, spec1, 4 * sizeof(float));
//	memcpy(amesh.mat.emissive, emissive, 4 * sizeof(float));
//	amesh.mat.shininess = shininess;
//	amesh.mat.texCount = texcount;
//	myMeshes.push_back(amesh);
//
//	// create geometry and VAO of the pawn
//	amesh = createPawn();
//	memcpy(amesh.mat.ambient, amb,4*sizeof(float));
//	memcpy(amesh.mat.diffuse, diff,4*sizeof(float));
//	memcpy(amesh.mat.specular, spec,4*sizeof(float));
//	memcpy(amesh.mat.emissive, emissive,4*sizeof(float));
//	amesh.mat.shininess = shininess;
//	amesh.mat.texCount = texcount;
//	myMeshes.push_back(amesh);
//
//	// create geometry and VAO of the sphere
//	amesh = createSphere(1.0f, 20);
//	memcpy(amesh.mat.ambient, amb, 4 * sizeof(float));
//	memcpy(amesh.mat.diffuse, diff, 4 * sizeof(float));
//	memcpy(amesh.mat.specular, spec, 4 * sizeof(float));
//	memcpy(amesh.mat.emissive, emissive, 4 * sizeof(float));
//	amesh.mat.shininess = shininess;
//	amesh.mat.texCount = texcount;
//	myMeshes.push_back(amesh);
//
//	// create geometry and VAO of the cylinder
//	amesh = createCylinder(1.5f, 0.5f, 20);
//	memcpy(amesh.mat.ambient, amb, 4 * sizeof(float));
//	memcpy(amesh.mat.diffuse, diff, 4 * sizeof(float));
//	memcpy(amesh.mat.specular, spec, 4 * sizeof(float));
//	memcpy(amesh.mat.emissive, emissive, 4 * sizeof(float));
//	amesh.mat.shininess = shininess;
//	amesh.mat.texCount = texcount;
//	myMeshes.push_back(amesh);
//	
//	// create geometry and VAO of the cone
//	amesh = createCone(2.5f, 1.2f, 20);
//	memcpy(amesh.mat.ambient, amb, 4 * sizeof(float));
//	memcpy(amesh.mat.diffuse, diff, 4 * sizeof(float));
//	memcpy(amesh.mat.specular, spec, 4 * sizeof(float));
//	memcpy(amesh.mat.emissive, emissive, 4 * sizeof(float));
//	amesh.mat.shininess = shininess;
//	amesh.mat.texCount = texcount;
//	myMeshes.push_back(amesh);
//
//	// create geometry and VAO of the torus
//	amesh = createTorus(0.5f, 1.5f, 20, 20);
//	memcpy(amesh.mat.ambient, amb, 4 * sizeof(float));
//	memcpy(amesh.mat.diffuse, diff, 4 * sizeof(float));
//	memcpy(amesh.mat.specular, spec, 4 * sizeof(float));
//	memcpy(amesh.mat.emissive, emissive, 4 * sizeof(float));
//	amesh.mat.shininess = shininess;
//	amesh.mat.texCount = texcount;
//	myMeshes.push_back(amesh);
//
//	// some GL settings
//	glEnable(GL_DEPTH_TEST);
//	glEnable(GL_CULL_FACE);
//	glEnable(GL_MULTISAMPLE);
//	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
//
//}
//
//// ------------------------------------------------------------
////
//// Main function
////
//
//
//int main(int argc, char **argv) {
//
////  GLUT initialization
//	glutInit(&argc, argv);
//	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA|GLUT_MULTISAMPLE);
//
//	glutInitContextVersion (3, 3);
//	glutInitContextProfile (GLUT_CORE_PROFILE );
//	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);
//
//	glutInitWindowPosition(100,100);
//	glutInitWindowSize(WinX, WinY);
//	WindowHandle = glutCreateWindow(CAPTION);
//
//
////  Callback Registration
//	glutDisplayFunc(renderScene);
//	glutReshapeFunc(changeSize);
//	//glutIdleFunc(renderScene);
//
////	Mouse and Keyboard Callbacks
//	glutKeyboardFunc(processKeys);
//	glutMouseFunc(processMouseButtons);
//	glutMotionFunc(processMouseMotion);
//	glutMouseWheelFunc ( mouseWheel ) ;
//	glutTimerFunc(0,timer,0);
//	glutTimerFunc(0, refresh, 0);
//
//
////	return from main loop
//	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
//
////	Init GLEW
//	glewExperimental = GL_TRUE;
//	glewInit();
//
//	printf ("Vendor: %s\n", glGetString (GL_VENDOR));
//	printf ("Renderer: %s\n", glGetString (GL_RENDERER));
//	printf ("Version: %s\n", glGetString (GL_VERSION));
//	printf ("GLSL: %s\n", glGetString (GL_SHADING_LANGUAGE_VERSION));
//
//	if (!setupShaders())
//		return(1);
//
//	init();
//
//	//  GLUT main loop
//	glutMainLoop();
//
//	return(0);
//
//}
//
