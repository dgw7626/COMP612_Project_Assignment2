# COMP612 Project : Helicopter simulator

<img src="https://github.com/dgw7626/COMP612_Project_Assignment2/blob/master/Project_Assignment2/Project_Assignment2/textures/helicopterSimulator.png" width="100%" />

This is my final project for Assignment 2 in COMP612. I used FreeGLUT and OpenGL to implement a rescue scene involving
a helicopter. The project is written in C, and most of the methods are implemented from scratch to utilize 3D math. 
The main features include a particle system and features like transform, vector3f, etc. I learned how to use OpenGL 
graphics and started to understand the architecture of the fixed rendering pipeline and other architectures.

## Features

### Finished (Helicopter Simulator Features)

- Controller for the Helicopter (WASD) for movements, (Arrow keys) for rotations.
- Visual Effect feature that implements particle system for the rain and fire.
- Animated tiled based sea ground plane.
- OBJ Exporter for the OpenGL
- Animated Helicopter, Windmill, Light House
- Smooth movement controller
- Realtime ambient lights and directional light.

### Bug Fixes

<details><summary>Expand to see bug fixes</summary>

- Fixed incorrect transform position of the object.
- Fixed the transitions between child object and parent object.
- Fixed the issue with disorinented rotation of the child object that has parent root.
- Particle system particle recycling issues with the geomeotry.
- Particle system unstable reliability due to mutliple 3D based mesh.
- Incorrect euler angle calculations for the helicopters oriented position.
- Fixed with window resize issue and rendering issues
- Camera distortion and flickering issues when coliding with skybox.
- Fixed with z plane so the double sides geometry will be invisible due to the matrix.
- Implementation of the delta time for the constant frame rates and animation cycles.
- Incorrect OBJ texure reader type.
- OpenGL does not support multi threading.
- Fixed the incorrect errors when you press "P".
- Fixed the input delay when the many entities appears.

</details>


# Package Structure
- **Project_Assignment2** — a folder that contains visual studio solution, transform.c, source.c, etc. 
- **Project_Executable_HighSpec** — a folder that contains executable application for the high spec models.
- **Project_Executable_LowSpec** —  a folder that contains executable application for the low spec models.
- **Logbook** — a logbook that records of project development progress.

