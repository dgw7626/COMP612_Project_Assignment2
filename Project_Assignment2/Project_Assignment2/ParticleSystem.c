#include "ParticleSystem.h"
#include <math.h>
/// <summary>
/// this method initializes the particle system.
/// </summary>
/// <param name="numParticles"></param>
/// <param name="maxLife"></param>
/// <param name="particleSystem"></param>
/// <param name="initialPos"></param>
/// <param name="range"></param>
void initParticleSystem(int numParticles, int maxLife, ParticleSystem_t particleSystem[], Vector3f initialPos[4], int range) {
    for (int i = 0; i < numParticles; i++) {
        particleSystem[i].position.x = (GLfloat)((((double)rand() / RAND_MAX) * (GLint)range * 2.0 - (GLint)range) + initialPos[i].x);
        particleSystem[i].position.y = (GLfloat)(((double)rand() / RAND_MAX) * initialPos[i].y);
        particleSystem[i].position.z = (GLfloat)((((double)rand() / RAND_MAX) * (GLint)range * 2.0 - (GLint)range) + initialPos[i].z);

        particleSystem[i].velocity.x = (GLfloat)(((double)rand() / (RAND_MAX - 0.5)) * 0.1);
        particleSystem[i].velocity.y = (GLfloat)(((double)rand() / (RAND_MAX * 0.05)) + 0.01);
        particleSystem[i].velocity.z = (GLfloat)(((double)rand() / (RAND_MAX - 0.5)) * 0.1);

        particleSystem[i].lifeOfTime = rand() % maxLife;
    }
}
/// <summary>
/// this method updates the current position of y each partcile per frame.
/// </summary>
/// <param name="numParticles"></param>
/// <param name="gravity"></param>
/// <param name="maxLife"></param>
/// <param name="initialPos"></param>
/// <param name="particleSystem"></param>
/// <param name="range"></param>
void updateParticleSystem(int numParticles, double gravity,int maxLife, Vector3f initialPos[4], ParticleSystem_t particleSystem[], int range) {
    for (int i = 0; i < numParticles; i++) {
        particleSystem[i].position.y += particleSystem[i].velocity.y;
        particleSystem[i].velocity.y += (GLfloat)gravity;
        particleSystem[i].lifeOfTime--;

        if (particleSystem[i].lifeOfTime < 0) {
            particleSystem[i].position.x = (GLfloat)(((GLfloat)rand() / (GLfloat)RAND_MAX * (GLint)range * 2.0f - (GLint)range));
            particleSystem[i].position.y = (GLfloat)((GLfloat)rand() / (GLfloat)RAND_MAX * initialPos[1].y);
            particleSystem[i].position.z = (GLfloat)(((GLfloat)rand() / (GLfloat)RAND_MAX * (GLint)range * 2.0f - (GLint)range));

            particleSystem[i].velocity.y = (GLfloat)((GLfloat)rand() / (GLfloat)RAND_MAX * 0.05 + 0.01);
            particleSystem[i].lifeOfTime = rand() % maxLife;
        }
    }
}
/// <summary>
/// this method redraws the particle each frame.
/// </summary>
/// <param name="numParticles"></param>
/// <param name="initialPos"></param>
/// <param name="particleSystem"></param>
/// <param name="color"></param>
/// <param name="pos"></param>
void drawParticleSystem(int numParticles, Vector3f initialPos[4], ParticleSystem_t particleSystem[], Color3f color,Vector3f pos) {
    glPushMatrix();
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_POINT_SMOOTH);
    glPointSize(1.0);
    glBegin(GL_POINTS);
    for (int i = 0; i <= numParticles; i++) {
        glColor3f(color.r, color.g, color.b);
        glVertex3f((particleSystem[i].position.x + pos.x), particleSystem[i].position.y, (particleSystem[i].position.z + pos.z));
    }
    glEnd();
    glDisable(GL_POINT_SMOOTH);
    glDisable(GL_COLOR_MATERIAL);
    glPopMatrix();
}
