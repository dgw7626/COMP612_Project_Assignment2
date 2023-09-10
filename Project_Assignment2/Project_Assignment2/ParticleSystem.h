#pragma once
#include "Transform.h"


typedef struct {
	Vector3f position; // particle positions
	Vector3f velocity; // particle velocity changes
	int lifeOfTime; // particle life of time

} ParticleSystem_t;
/// <summary>
/// this method initializes the particle system.
/// </summary>
/// <param name="numParticles"></param>
/// <param name="maxLife"></param>
/// <param name="particleSystem"></param>
/// <param name="initialPos"></param>
/// <param name="range"></param>
void initParticleSystem(int numParticles, int maxLife, ParticleSystem_t particleSystem[], Vector3f initialPos[4], int range);
/// <summary>
/// this method updates the current position of y each partcile per frame.
/// </summary>
/// <param name="numParticles"></param>
/// <param name="gravity"></param>
/// <param name="maxLife"></param>
/// <param name="initialPos"></param>
/// <param name="particleSystem"></param>
/// <param name="range"></param>
void updateParticleSystem(int numParticles, double gravity, int maxLife, Vector3f initialPos[4], ParticleSystem_t particleSystem[], int range);
/// <summary>
/// this method redraws the particle each frame.
/// </summary>
/// <param name="numParticles"></param>
/// <param name="initialPos"></param>
/// <param name="particleSystem"></param>
/// <param name="color"></param>
/// <param name="pos"></param>
void drawParticleSystem(int numParticles, Vector3f initialPos[4], ParticleSystem_t particleSystem[], Color3f color, Vector3f pos);