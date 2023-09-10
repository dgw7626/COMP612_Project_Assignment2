#include "DefaultAssets.h"
#include "ObjLoader.h"

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
void getTransformDisk(Transform parent, Transform transform, Material material, GLdouble innerRadius, GLdouble outerRadius, GLint slices, GLint loops, int axesEnabled, GLUquadricObj* obj) {
	glPushMatrix();
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	//get the ambient, diffuse, specular values
	GLfloat ambientMaterial[4] = { material.ambient.r, material.ambient.g, material.ambient.b, material.ambient.a };
	GLfloat diffuseMaterial[4] = { material.diffuse.r, material.diffuse.g, material.diffuse.b, material.diffuse.a };
	GLfloat specularMaterial[4] = { material.specular.r, material.specular.g, material.specular.b, material.specular.a };
	//apply those values to the face front only.
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambientMaterial);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseMaterial);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specularMaterial);
	glMaterialf(GL_FRONT, GL_SHININESS, material.shininess);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_TEXTURE_2D);
	//get the parent position and rotations.
	glTranslatef(parent.position.x, parent.position.y, parent.position.z);
	glRotatef(parent.rotation.angleY, 0, parent.rotation.rotY, 0);
	glRotatef(parent.rotation.angleZ, 0, 0, parent.rotation.rotZ);
	glRotatef(parent.rotation.angleX, parent.rotation.rotX, 0, 0);
	//display the current origin axes of the object.
	if (axesEnabled) {
		drawTransformAxes(transform.position, transform.rotation, (Scalef) { 1, 1, 1 }, 0.5f);
	}
	//get the transform positions and rotations, colors.
	glColor3f(transform.color.r, transform.color.g, transform.color.b);
	glTranslatef(transform.position.x, transform.position.y, transform.position.z);
	glRotatef(transform.rotation.angleX, transform.rotation.rotX, 0, 0);
	glRotatef(transform.rotation.angleY, 0, transform.rotation.rotY, 0);
	glRotatef(transform.rotation.angleZ, 0, 0, transform.rotation.rotZ);
	glScalef(transform.scale.x, transform.scale.y, transform.scale.z);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	gluDisk(obj, innerRadius, outerRadius, slices, loops);
	glDisable(GL_COLOR_MATERIAL);
	glPopMatrix();
}

/// <summary>
/// this method gets the freeglut Cube Object.
/// </summary>
/// <param name="parent"></param>
/// <param name="transform"></param>
/// <param name="material"></param>
/// <param name="axesEnabled"></param>
/// <param name="renderFillEnabled"></param>
void getTransformCube(Transform parent, Transform transform, Material material, int axesEnabled, int renderFillEnabled) {
	glPushMatrix();
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	//get the ambient, diffuse, specular values
	GLfloat ambientMaterial[4] = { material.ambient.r, material.ambient.g, material.ambient.b, material.ambient.a };
	GLfloat diffuseMaterial[4] = { material.diffuse.r, material.diffuse.g, material.diffuse.b, material.diffuse.a };
	GLfloat specularMaterial[4] = { material.specular.r, material.specular.g, material.specular.b, material.specular.a };
	//apply those values to the face front only.
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambientMaterial);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseMaterial);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specularMaterial);
	glMaterialf(GL_FRONT, GL_SHININESS, material.shininess);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_TEXTURE_2D);
	//get the parent position and rotations.
	glTranslatef(parent.position.x, parent.position.y, parent.position.z);
	glRotatef(parent.rotation.angleY, 0, parent.rotation.rotY, 0);
	glRotatef(parent.rotation.angleZ, 0, 0, parent.rotation.rotZ);
	glRotatef(parent.rotation.angleX, parent.rotation.rotX, 0, 0);
	//display the current origin axes of the object.
	if (axesEnabled) {
		drawTransformAxes(transform.position, transform.rotation, (Scalef) { 1, 1, 1 }, 0.5f);
	}
	glColor3f(transform.color.r, transform.color.g, transform.color.b);
	//get the transform positions and rotations, colors.
	glTranslatef(transform.position.x, transform.position.y, transform.position.z);
	glRotatef(transform.rotation.angleX, transform.rotation.rotX, 0, 0);
	glRotatef(transform.rotation.angleY, 0, transform.rotation.rotY, 0);
	glRotatef(transform.rotation.angleZ, 0, 0, transform.rotation.rotZ);
	glScalef(transform.scale.x, transform.scale.y, transform.scale.z);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	renderFillEnabled ? glutSolidCube(1) : glutWireCube(1);
	glDisable(GL_COLOR_MATERIAL);
	glPopMatrix();

}
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
void getTransformSphere(Transform parent, Transform transform, Material material, GLdouble radius, GLint slices, GLint stacks, int axesEnabled, GLUquadricObj* obj) {
	glPushMatrix();

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	//get the ambient, diffuse, specular values
	GLfloat ambientMaterial[4] = { material.ambient.r, material.ambient.g, material.ambient.b, material.ambient.a };
	GLfloat diffuseMaterial[4] = { material.diffuse.r, material.diffuse.g, material.diffuse.b, material.diffuse.a };
	GLfloat specularMaterial[4] = { material.specular.r, material.specular.g, material.specular.b, material.specular.a };
	//apply those values to the face front only.
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambientMaterial);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseMaterial);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specularMaterial);
	glMaterialf(GL_FRONT, GL_SHININESS, material.shininess);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_TEXTURE_2D);
	//get the parent position and rotations.
	glTranslatef(parent.position.x, parent.position.y, parent.position.z);
	glRotatef(parent.rotation.angleY, 0, parent.rotation.rotY, 0);
	glRotatef(parent.rotation.angleZ, 0, 0, parent.rotation.rotZ);
	glRotatef(parent.rotation.angleX, parent.rotation.rotX, 0, 0);
	//display the current origin axes of the object.
	if (axesEnabled) {
		drawTransformAxes(transform.position, transform.rotation, (Scalef) { 1, 1, 1 }, 0.5f);
	}
	//get the transform positions and rotations, colors.
	glColor3f(transform.color.r, transform.color.g, transform.color.b);
	glTranslatef(transform.position.x, transform.position.y, transform.position.z);
	glRotatef(transform.rotation.angleX, transform.rotation.rotX, 0, 0);
	glRotatef(transform.rotation.angleY, 0, transform.rotation.rotY, 0);
	glRotatef(transform.rotation.angleZ, 0, 0, transform.rotation.rotZ);
	glScalef(transform.scale.x, transform.scale.y, transform.scale.z);

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	gluSphere(obj, radius, slices, stacks);
	glDisable(GL_COLOR_MATERIAL);
	glPopMatrix();
}
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
void getTransformCylinder(Transform parent, Transform transform, Material material, GLdouble baseRadius, GLdouble topRadius, GLdouble height, GLint slices, GLint stacks, int axesEnabled, GLUquadricObj* obj) {
	glPushMatrix();

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	//get the ambient, diffuse, specular values
	GLfloat ambientMaterial[4] = { material.ambient.r, material.ambient.g, material.ambient.b, material.ambient.a };
	GLfloat diffuseMaterial[4] = { material.diffuse.r, material.diffuse.g, material.diffuse.b, material.diffuse.a };
	GLfloat specularMaterial[4] = { material.specular.r, material.specular.g, material.specular.b, material.specular.a };
	//apply those values to the face front only.
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambientMaterial);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseMaterial);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specularMaterial);
	glMaterialf(GL_FRONT, GL_SHININESS, material.shininess);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_TEXTURE_2D);
	//get the parent position and rotations.
	glTranslatef(parent.position.x, parent.position.y, parent.position.z);
	glRotatef(parent.rotation.angleY, 0, parent.rotation.rotY, 0);
	glRotatef(parent.rotation.angleZ, 0, 0, parent.rotation.rotZ);
	glRotatef(parent.rotation.angleX, parent.rotation.rotX, 0, 0);
	//display the current origin axes of the object.
	if (axesEnabled) {
		drawTransformAxes(transform.position, transform.rotation, (Scalef) { 1, 1, 1 }, 0.5f);
	}
	//get the transform positions and rotations, colors.
	glColor3f(transform.color.r, transform.color.g, transform.color.b);
	glTranslatef(transform.position.x, transform.position.y, transform.position.z);
	glRotatef(transform.rotation.angleX, transform.rotation.rotX, 0, 0);
	glRotatef(transform.rotation.angleY, 0, transform.rotation.rotY, 0);
	glRotatef(transform.rotation.angleZ, 0, 0, transform.rotation.rotZ);
	glScalef(transform.scale.x, transform.scale.y, transform.scale.z);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	gluCylinder(obj, baseRadius, topRadius, height, slices, stacks);
	glDisable(GL_COLOR_MATERIAL);
	glPopMatrix();

}
/// <summary>
/// this method gets the OBJ transform.
/// </summary>
/// <param name="parent"></param>
/// <param name="transform"></param>
/// <param name="material"></param>
/// <param name="axesEnabled"></param>
/// <param name="obj"></param>
/// <param name="texture"></param>
void getTransformOBJ(Transform parent, Transform transform,Material material, int axesEnabled, MeshObject* obj, GLint texture) {

	glPushMatrix();
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	//get the ambient, diffuse, specular values
	GLfloat ambientMaterial[4] = { material.ambient.r, material.ambient.g, material.ambient.b, material.ambient.a };
	GLfloat diffuseMaterial[4] = { material.diffuse.r, material.diffuse.g, material.diffuse.b, material.diffuse.a };
	GLfloat specularMaterial[4] = { material.specular.r, material.specular.g, material.specular.b, material.specular.a };
	//apply those values to the face front only.
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambientMaterial);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseMaterial);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specularMaterial);
	glMaterialf(GL_FRONT, GL_SHININESS, material.shininess);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_TEXTURE_2D);
	//get the parent position and rotations.
	glTranslatef(parent.position.x, parent.position.y, parent.position.z);
	glRotatef(parent.rotation.angleY, 0, parent.rotation.rotY, 0);
	glRotatef(parent.rotation.angleZ, 0, 0, parent.rotation.rotZ);
	glRotatef(parent.rotation.angleX, parent.rotation.rotX, 0, 0);
	//display the current origin axes of the object.
	if (axesEnabled) {
		drawTransformAxes(transform.position, transform.rotation, (Scalef) { 1, 1, 1 }, 0.5f);
	}
	//get the transform positions and rotations, colors.
	glColor3f(transform.color.r, transform.color.g, transform.color.b);
	glTranslatef(transform.position.x, transform.position.y, transform.position.z);
	glRotatef(transform.rotation.angleX, transform.rotation.rotX, 0, 0);
	glRotatef(transform.rotation.angleY, 0, transform.rotation.rotY, 0);
	glRotatef(transform.rotation.angleZ, 0, 0, transform.rotation.rotZ);
	glScalef(transform.scale.x, transform.scale.y, transform.scale.z);
	glBindTexture(GL_TEXTURE_2D, texture);
	renderMeshObject(obj);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDisable(GL_COLOR_MATERIAL);
	glPopMatrix();

}
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
void drawSpotlight(GLenum lightName, Material material, Transform parent, Transform transform,GLfloat spotExponent, GLfloat spotCutoff, int axesEnabled) {
	glPushMatrix();
	GLfloat spotDirection[] = { 0.0, -1.0, 0.0 };  // Spotlight direction (pointing downwards)

	// Apply parent's position and rotation
	glTranslatef(parent.position.x, parent.position.y, parent.position.z);
	glRotatef(parent.rotation.angleY, 0, parent.rotation.rotY, 0);
	glRotatef(parent.rotation.angleZ, 0, 0, parent.rotation.rotZ);
	glRotatef(parent.rotation.angleX, parent.rotation.rotX, 0, 0);

	if (axesEnabled) {
		drawTransformAxes(transform.position, transform.rotation, (Scalef) { 1, 1, 1 }, 0.5f);
	}
	//get the transform positions and rotations, colors.
	glRotatef(transform.rotation.angleX, transform.rotation.rotX, 0, 0);
	glRotatef(transform.rotation.angleY, 0, transform.rotation.rotY, 0);
	glRotatef(transform.rotation.angleZ, 0, 0, transform.rotation.rotZ);
	glScalef(transform.scale.x, transform.scale.y, transform.scale.z);

	// Set spotlight material properties
	GLfloat ambientMaterial[] = { material.ambient.r, material.ambient.g, material.ambient.b, material.ambient.a };
	GLfloat diffuseMaterial[] = { material.diffuse.r, material.diffuse.g, material.diffuse.b, material.diffuse.a };
	GLfloat specularMaterial[] = { material.specular.r, material.specular.g, material.specular.b, material.specular.a };

	glLightfv(lightName, GL_AMBIENT, ambientMaterial);
	glLightfv(lightName, GL_DIFFUSE, diffuseMaterial);
	glLightfv(lightName, GL_SPECULAR, specularMaterial);

	// Enable lighting and spotlight
	glEnable(GL_LIGHTING);
	glEnable(lightName);
	glEnable(GL_NORMALIZE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);

	// Set spotlight properties
	glLightfv(lightName, GL_SPOT_DIRECTION, spotDirection);
	glTranslatef(transform.position.x, transform.position.y, transform.position.z);
	GLfloat lightPosition[] = { transform.position.x, transform.position.y, transform.position.z, 1.0 };
	glLightfv(lightName, GL_POSITION, lightPosition);

	// Rotate the spotlight direction
	GLfloat lightDir[] = { 0.0, -1.0, 0.0 }; // Initialize the spotlight direction

	glMultMatrixf(lightDir); // Apply the rotation to the spotlight direction vector

	glLightf(lightName, GL_SPOT_EXPONENT, spotExponent);
	glLightf(lightName, GL_SPOT_CUTOFF, spotCutoff);

	// Restore previous matrix state
	glPopMatrix();
}



