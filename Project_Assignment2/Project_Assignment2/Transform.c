#include "Transform.h"

/// <summary>
/// this method sets the vector3 of x y z 
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="z"></param>
/// <returns>Vector3</returns>
Vector3f setVec3(GLfloat x, GLfloat y, GLfloat z) {
	Vector3f vector3;
	vector3.x = x;
	vector3.y = y;
	vector3.z = z;
	return vector3;
}
/// <summary>
/// this method sets the Transform of Vector3, Quaternion, Scale and Color
/// </summary>
/// <param name="pos"></param>
/// <param name="rot"></param>
/// <param name="scale"></param>
/// <param name="color"></param>
/// <returns>Transform</returns>
Transform setTransform(Vector3f pos, Quaternionf rot, Scalef scale, Color3f color) {
	Transform transform;
	transform.position = pos;
	transform.rotation = rot;
	transform.scale = scale;
	transform.color = color;
	return transform;
}
/// <summary>
/// this method draws the current position of the object axes (x,y,z)
/// </summary>
/// <param name="position"></param>
/// <param name="rot"></param>
/// <param name="scale"></param>
/// <param name="lineWidth"></param>
void drawTransformAxes(Vector3f position, Quaternionf rot, Scalef scale, GLfloat lineWidth) {
	glPushMatrix();
	glTranslatef(position.x, position.y, position.z);
	glRotatef(rot.angleX, rot.rotX, 0, 0);
	glRotatef(rot.angleY, 0, rot.rotY, 0);
	glRotatef(rot.angleZ, 0, 0, rot.rotZ);
	glScalef(scale.x, scale.y, scale.z);
	glLineWidth(lineWidth);
	glBegin(GL_LINES);
	// x axis
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(2.0f, 0.0f, 0.0f);
	// y axis
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 2.0f, 0.0f);
	// z axis
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 2.0f);
	glEnd();
	glColor3f(1.0f, 1.0f, 1.0f);
	glPopMatrix();
}