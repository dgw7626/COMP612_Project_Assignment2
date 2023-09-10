#pragma once
#include <Windows.h>
#include <freeglut.h>
#include <math.h>
#include <stdio.h>
#include "Transform.h"


/// <summary>
/// Called by GLUT when the window is closed. Release any previously allocated resources here.
/// </summary>
/// <param name="mesh"></param>
void close(MeshObject* mesh);
/// <summary>
/// it cleans the mesh object data.
/// </summary>
/// <param name="object"></param>
void freeMeshObject(MeshObject* object);
/// <summary>
/// Render the faces of the specified Mesh Object in OpenGL.
/// </summary>
/// <param name="object"></param>
void renderMeshObject(MeshObject* object);
/// <summary>
/// Initialise the specified Mesh Object Face from a string of face data in the Wavefront OBJ file format.
/// </summary>
/// <param name="face"></param>
/// <param name="faceData"></param>
/// <param name="maxFaceDataLength"></param>
void initMeshObjectFace(MeshObjectFace* face, char* faceData, int faceDataLength);
/// <summary>
/// Loads the meshobject and automatically caclulates the vertices, normals, face, facecounts, the file name must be given.
/// </summary>
/// <param name="fileName"></param>
/// <returns></returns>
MeshObject* loadMeshObject(const char* fileName);
/// <summary>
/// this method loads the ppm image and stored in GLint texture data.
/// </summary>
/// <param name="fileName"></param>
/// <returns></returns>
int loadPPM(char* fileName);