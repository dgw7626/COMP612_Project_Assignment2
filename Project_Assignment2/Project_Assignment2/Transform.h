#pragma once
#include <freeglut.h>

typedef struct {
	GLfloat x; // Vector x
	GLfloat y; // Vector y
}Vector2f;
typedef struct {
	GLfloat r; // Color r
	GLfloat g; // Color g
	GLfloat b; // Color b
}Color3f;

typedef struct {
	GLfloat r; // Color r
	GLfloat g; // Color g
	GLfloat b; // Color b
	GLfloat a; // Color a
}Color4f;

typedef struct {
	GLfloat x; // Color x
	GLfloat y; // Color y
	GLfloat z; // Color z
}Vector3f;

typedef struct {
	GLfloat x; // Scale x 
	GLfloat y; // Scale y
	GLfloat z; // SCale z
}Scalef;

typedef struct {
	GLfloat rotX;	// enable x
	GLfloat rotY;   // enable y
	GLfloat rotZ;   // enable z
	GLfloat angleX; // angle x
	GLfloat angleY; // angle y
	GLfloat angleZ; // angle z
}Quaternionf;

typedef struct {
	Color4f ambient; // ambient colors
	Color4f diffuse; // diffuse colors
	Color4f specular; // sepcular colors
	GLfloat shininess; // brightness of the glfloat.
}Material;

typedef struct {
	Vector3f position; // Vector 3 positions
	Quaternionf rotation; //Quaternion rotations
	Scalef scale;	// Scale 
	Color3f color;	// Colors
}Transform;

typedef struct {
	int vertexIndex;	// Index of this vertex in the object's vertices array
	int texCoordIndex; // Index of the texture coordinate for this vertex in the object's texCoords array
	int normalIndex;	// Index of the normal for this vertex in the object's normals array
} MeshObjectFacePoint;

typedef struct {
	int pointCount;
	MeshObjectFacePoint* points;
} MeshObjectFace;

typedef struct {
	int vertexCount;
	Vector3f* vertices;
	int texCoordCount;
	Vector2f* texCoords;
	int normalCount;
	Vector3f* normals;
	int faceCount;
	MeshObjectFace* faces;
} MeshObject;

/// <summary>
/// this method sets the vector3 of x y z 
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="z"></param>
/// <returns>Vector3</returns>
Vector3f setVec3(GLfloat x, GLfloat y, GLfloat z);
/// <summary>
/// this method sets the Transform of Vector3, Quaternion, Scale and Color
/// </summary>
/// <param name="pos"></param>
/// <param name="rot"></param>
/// <param name="scale"></param>
/// <param name="color"></param>
/// <returns>Transform</returns>
Transform setTransform(Vector3f pos, Quaternionf rot, Scalef scale, Color3f color);
/// <summary>
/// this method draws the current position of the object axes (x,y,z)
/// </summary>
/// <param name="position"></param>
/// <param name="rot"></param>
/// <param name="scale"></param>
/// <param name="lineWidth"></param>
void drawTransformAxes(Vector3f position, Quaternionf rot, Scalef scale, GLfloat lineWidth);