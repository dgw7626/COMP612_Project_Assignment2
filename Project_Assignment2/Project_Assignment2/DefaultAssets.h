#pragma once
#include "Transform.h"
/// <summary>
/// this method gets the freeglut Disk Object.
/// </summary>
/// <param name="parent"></param>
/// <param name="transform"></param>
/// <param name="innerRadius"></param>
/// <param name="outerRadius"></param>
/// <param name="slices"></param>
/// <param name="loops"></param>
/// <param name="axesEnabled"></param>
/// <param name="obj"></param>
void getTransformDisk(Transform parent, Transform transform, Material material, GLdouble innerRadius, GLdouble outerRadius, GLint slices, GLint loops, int axesEnabled, GLUquadricObj* obj);
/// <summary>
/// this method gets the freeglut Cube Object.
/// </summary>
/// <param name="parent"></param>
/// <param name="transform"></param>
/// <param name="material"></param>
/// <param name="axesEnabled"></param>
/// <param name="renderFillEnabled"></param>
void getTransformCube(Transform parent, Transform transform, Material material, int axesEnabled, int renderFillEnabled);
/// <summary>
/// this method gets the freeglut Sphere Object.
/// </summary>
/// <param name="parent"></param>
/// <param name="transform"></param>
/// <param name="material"></param>
/// <param name="radius"></param>
/// <param name="slices"></param>
/// <param name="stacks"></param>
/// <param name="axesEnabled"></param>
/// <param name="obj"></param>
void getTransformSphere(Transform parent, Transform transform, Material material, GLdouble radius, GLint slices, GLint stacks, int axesEnabled, GLUquadricObj* obj);
/// <summary>
/// this method gets the freeglut Cylinder Object
/// </summary>
/// <param name="parent"></param>
/// <param name="transform"></param>
/// <param name="material"></param>
/// <param name="baseRadius"></param>
/// <param name="topRadius"></param>
/// <param name="height"></param>
/// <param name="slices"></param>
/// <param name="stacks"></param>
/// <param name="axesEnabled"></param>
/// <param name="obj"></param>
void getTransformCylinder(Transform parent, Transform transform, Material material, GLdouble baseRadius, GLdouble topRadius, GLdouble height, GLint slices, GLint stacks, int axesEnabled, GLUquadricObj* obj);
/// <summary>
/// this method gets the OBJ transform.
/// </summary>
/// <param name="parent"></param>
/// <param name="transform"></param>
/// <param name="material"></param>
/// <param name="axesEnabled"></param>
/// <param name="obj"></param>
/// <param name="texture"></param>
void getTransformOBJ(Transform parent, Transform transform, Material material, int axesEnabled, MeshObject* obj, GLint texture);
/// <summary>
/// This method creates spot light with given GL_LIGHT parameter.
/// </summary>
/// <param name="lightName"></param>
/// <param name="material"></param>
/// <param name="parent"></param>
/// <param name="transform"></param>
/// <param name="spotExponent"></param>
/// <param name="spotCutoff"></param>
/// <param name="axesEnabled"></param>
void drawSpotlight(GLenum lightName, Material material, Transform parent, Transform transform, GLfloat spotExponent, GLfloat spotCutoff, int axesEnabled);