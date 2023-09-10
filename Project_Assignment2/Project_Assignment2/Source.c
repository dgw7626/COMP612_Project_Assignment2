/******************************************************************************
 *
 * Computer Graphics Programming 2020 Project Template v1.0 (11/04/2021)
 *
 * Based on: Animation Controller v1.0 (11/04/2021)
 *
 * This template provides a basic FPS-limited render loop for an animated scene,
 * plus keyboard handling for smooth game-like control of an object such as a
 * character or vehicle.
 *
 * A simple static lighting setup is provided via initLights(), which is not
 * included in the animationalcontrol.c template. There are no other changes.
 *
 ******************************************************************************/
#include "ObjLoader.h"
#include "DefaultAssets.h"
#include "ParticleSystem.h"
#include "RandomUtility.h"
#include <Windows.h>
#include <freeglut.h>
#include <math.h>
#include <stdio.h>
 /******************************************************************************
  * Animation & Timing Setup
  ******************************************************************************/

  // Target frame rate (number of Frames Per Second).
#define TARGET_FPS 60				

// Ideal time each frame should be displayed for (in milliseconds).
const unsigned int FRAME_TIME = 1000 / TARGET_FPS;

// Frame time in fractional seconds.
// Note: This is calculated to accurately reflect the truncated integer value of
// FRAME_TIME, which is used for timing, rather than the more accurate fractional
// value we'd get if we simply calculated "FRAME_TIME_SEC = 1.0f / TARGET_FPS".
const float FRAME_TIME_SEC = (1000 / TARGET_FPS) / 1000.0f;

// Time we started preparing the current frame (in milliseconds since GLUT was initialized).
unsigned int frameStartTime = 0;

/******************************************************************************
 * Some Simple Definitions of Motion
 ******************************************************************************/

#define MOTION_NONE 0				// No motion.
#define MOTION_CLOCKWISE -1			// Clockwise rotation.
#define MOTION_ANTICLOCKWISE 1		// Anticlockwise rotation.
#define MOTION_BACKWARD -1			// Backward motion.
#define MOTION_FORWARD 1			// Forward motion.
#define MOTION_LEFT -1				// Leftward motion.
#define MOTION_RIGHT 1				// Rightward motion.
#define MOTION_DOWN -1				// Downward motion.
#define MOTION_UP 1					// Upward motion.

 // Represents the motion of an object on four axes (Yaw, Surge, Sway, and Heave).
 // 
 // You can use any numeric values, as specified in the comments for each axis. However,
 // the MOTION_ definitions offer an easy way to define a "unit" movement without using
 // magic numbers (e.g. instead of setting Surge = 1, you can set Surge = MOTION_FORWARD).
 //
typedef struct {
	int Yaw;		// Turn about the Z axis	[<0 = Clockwise, 0 = Stop, >0 = Anticlockwise]
	int Surge;		// Move forward or back		[<0 = Backward,	0 = Stop, >0 = Forward]
	int Sway;		// Move sideways (strafe)	[<0 = Left, 0 = Stop, >0 = Right]
	int Heave;		// Move vertically			[<0 = Down, 0 = Stop, >0 = Up]
} motionstate4_t;

/******************************************************************************
 * Keyboard Input Handling Setup
 ******************************************************************************/

 // Represents the state of a single keyboard key.Represents the state of a single keyboard key.
typedef enum {
	KEYSTATE_UP = 0,	// Key is not pressed.
	KEYSTATE_DOWN		// Key is pressed down.
} keystate_t;

// Represents the states of a set of keys used to control an object's motion.
typedef struct {
	keystate_t MoveForward;
	keystate_t MoveBackward;
	keystate_t MoveLeft;
	keystate_t MoveRight;
	keystate_t MoveUp;
	keystate_t MoveDown;
	keystate_t TurnLeft;
	keystate_t TurnRight;
} motionkeys_t;

// Current state of all keys used to control our "player-controlled" object's motion.
motionkeys_t motionKeyStates = {
	KEYSTATE_UP, KEYSTATE_UP, KEYSTATE_UP, KEYSTATE_UP,
	KEYSTATE_UP, KEYSTATE_UP, KEYSTATE_UP, KEYSTATE_UP };

// How our "player-controlled" object should currently be moving, solely based on keyboard input.
//
// Note: this may not represent the actual motion of our object, which could be subject to
// other controls (e.g. mouse input) or other simulated forces (e.g. gravity).
motionstate4_t keyboardMotion = { MOTION_NONE, MOTION_NONE, MOTION_NONE, MOTION_NONE };

// Define all character keys used for input (add any new key definitions here).
// Note: USE ONLY LOWERCASE CHARACTERS HERE. The keyboard handler provided converts all
// characters typed by the user to lowercase, so the SHIFT key is ignored.

#define KEY_MOVE_FORWARD	'w'
#define KEY_MOVE_BACKWARD	's'
#define KEY_MOVE_LEFT		'a'
#define KEY_MOVE_RIGHT		'd'
#define KEY_RENDER_FILL		'l'
#define KEY_EXIT			27 // Escape key.

// Define all GLUT special keys used for input (add any new key definitions here).

#define SP_KEY_MOVE_UP		GLUT_KEY_UP
#define SP_KEY_MOVE_DOWN	GLUT_KEY_DOWN
#define SP_KEY_TURN_LEFT	GLUT_KEY_LEFT
#define SP_KEY_TURN_RIGHT	GLUT_KEY_RIGHT

/******************************************************************************
 * GLUT Callback Prototypes
 ******************************************************************************/

void display(void);
void reshape(int width, int height);
void keyPressed(unsigned char key, int x, int y);
void specialKeyPressed(int key, int x, int y);
void keyReleased(unsigned char key, int x, int y);
void specialKeyReleased(int key, int x, int y);
void idle(void);


/******************************************************************************
 * World Scale (unit 1 = 1 Meter)
 ******************************************************************************/
#define UNIT 1

 /******************************************************************************
  * Animation-Specific Setup (Add your own definitions, constants, and globals here)
  ******************************************************************************/

#define KEY_RENDER_AXIS 'p'
#define PI 3.14159265359
typedef struct {
	int x;
	int y;
}WindowSize;

WindowSize Screen = { 800,600 };
/******************************************************************************
 * Particle System.
 ******************************************************************************/
//Snow Particle System
#define P_SNOW_NUM_PARTICLES 400
#define P_SNOW_RANGE 600
#define P_SNOW_LIFE 100
#define P_SNOW_GRAVITY -0.1

Vector3f startSnowPosition = { 0,300,0 };
Vector3f initialSnowPositions[4];
ParticleSystem_t snowParticleSystem[P_SNOW_NUM_PARTICLES];
// Fire Particle System
#define P_FIRE_NUM_PARTICLES 100
#define P_FIRE_RANGE 5
#define P_FIRE_LIFE 20
#define P_FIRE_GRAVITY 0.1

Vector3f initialFirePositions[4];
Vector3f startFirePosition = { 20,0,380 };
ParticleSystem_t fireParticleSystem[P_FIRE_NUM_PARTICLES];
/******************************************************************************
 * Render objects as filled polygons (GL_TRUE) 1 or wireframes (GL_FALSE) 0. Default filled.
 ******************************************************************************/
unsigned int renderFillEnabled = GL_TRUE;
unsigned int axesEnabled = GL_FALSE;
/******************************************************************************
 * Helicopter Preference settings Angle, Position, Initial turning angles.
 ******************************************************************************/
Vector3f camera = { 0,1,10 };
Vector3f heliPosition = { 0,1,-380 };
GLdouble heliYawAngleY = 0.0f;
GLdouble heliPitchAngleX = 0.0f;
GLdouble heliRollAngleZ = 0.0f;
GLfloat limitedTurningPoint = 15.0f;
//Helicopter rotor rotation
GLfloat rotorRotation = 0.0f;
GLfloat startSpeed = 8.8f;
GLfloat speed = 0.0f;
//light box rotation
GLfloat lightSourceRotation = 0.0f;
GLfloat lightStartSpeed = 0.5f;
GLfloat lightSpeed = 0.0f;
//const values for helicopter predefined angles.
const GLdouble DEG_TO_RAD = PI / 180.0f;
const GLdouble MAX_PITCH_ANGLE = 15.0f;
const GLdouble HELI_SPEED = 28.0f;
const GLdouble RPM = 90.0f;
const GLdouble MAX_ANGLE = 360.0f;
/******************************************************************************
 * Random Ground, Wind Positions and Rotations.
 ******************************************************************************/
Vector3f randomPostions[10];
Vector3f randomWindPositions[10];
Quaternionf randomRotations[10];
/******************************************************************************
 * Default Material and Transform settings
 ******************************************************************************/
Transform defaultTransform = { 0,0,0,0,0,0,0,0,0,1,1,1,0,0,0 };
Material ambientMaterial = { 1,1,1,1,0.1f,0.2f,0.2f,0.2f,1,1,1,1,100.0f };
Material diffuseMaterial = { 0,0,0,0.2f,0.1f,0.5f,0.8f,0.2f,0,0,0,0.2f,10.0f };
Material lightMaterial = {1,1,1,1,0.5f,0.5f,0.5f,1,0.2f,0.2f,0.2f,1,100.0f};
/******************************************************************************
 * Freeglut quadric objects
 ******************************************************************************/
GLUquadricObj* sphereObj;
GLUquadricObj* cylinderObj;
GLUquadricObj* diskObj;
/******************************************************************************
 * Mesh Objects for OBJ
 ******************************************************************************/
MeshObject* cubeMeshObj;
MeshObject* sphereMeshObj;
MeshObject* monkeyMeshObj;
MeshObject* coneMeshObj;
MeshObject* cylinderMeshObj;
MeshObject* groundMeshObj;
/******************************************************************************
 * OBJ texture map lists.
 ******************************************************************************/
GLuint nightSkyBoxTexture;
GLuint brickTexture;
GLuint monkeyTexture;
GLuint marbleTexture;
GLuint woodTexture;
GLuint dirtTexture;
GLuint waterTexture;
GLuint heliPadTexture;


/******************************************************************************
 * Animation-Specific Function Prototypes (add your own here)
 ******************************************************************************/

void main(int argc, char** argv);
void init(void);
void think(void);
void initLights(void);
/// <summary>
/// this method draws the ground plane with the animated texture map.
/// </summary>
/// <param name="material"></param>
/// <param name="color"></param>
/// <param name="position"></param>
/// <param name="numQuads"></param>
/// <param name="textureID"></param>
/// <param name="rotationSpeed"></param>
void drawGround(Material material, Color3f color, Vector3f position, int numQuads, GLuint textureID, GLfloat rotationSpeed);
/// <summary>
/// this method instantiates the helipad ground object.
/// </summary>
/// <param name="pos"></param>
/// <param name="scale"></param>
void instantiateHeliPad(Vector3f pos, Scalef scale);
/// <summary>
/// this method instantiates the skybox object.
/// </summary>
/// <param name="pos"></param>
/// <param name="scale"></param>
void instantiateSkyBox(Vector3f pos, Scalef scale);
/// <summary>
/// this method instantiates the old windmill object.
/// </summary>
/// <param name="pos"></param>
/// <param name="transformRotation"></param>
/// <param name="rot"></param>
void instantiateOldWindMill(Vector3f pos, Quaternionf transformRotation, GLfloat rot);
/// <summary>
/// this method instantiates the animated windmill object.
/// </summary>
/// <param name="pos"></param>
/// <param name="transformRotation"></param>
/// <param name="rot"></param>
/// <param name="lightName"></param>
void instantiateWindMill(Vector3f pos, Quaternionf transformRotation, GLfloat rot, GLenum lightName);
/// <summary>
/// this method instantiates the land ground object.
/// </summary>
/// <param name="pos"></param>
void instantiateGround(Vector3f pos);
/// <summary>
/// this method instantiates the light house object.
/// </summary>
/// <param name="pos"></param>
/// <param name="rot"></param>
void instantiateLightHouse(Vector3f pos, GLfloat rot);
/// <summary>
/// this method instantiates the helicopter object instance with rotation parameter.
/// </summary>
/// <param name="pos"></param>
/// <param name="rot"></param>
/// <param name="quaternionf"></param>
void instantiateHelicopter(Vector3f pos, GLfloat rot, Quaternionf quaternionf);
/// <summary>
/// this mehtod is for helicopter instance to create main body part of the helicopter.
/// </summary>
/// <param name="pos"></param>
/// <param name="rotorRotation"></param>
/// <param name="quaternionf"></param>
void instantiateMainBody(Vector3f pos, GLfloat rotorRotation, Quaternionf quaternionf);
/// <summary>
/// this method is for helicopter instance to create animated main rotor.
/// </summary>
/// <param name="parent"></param>
/// <param name="pos"></param>
/// <param name="rot"></param>
void instantiateMainRotor(Transform parent, Vector3f pos, GLfloat rot);
/// <summary>
/// this method is for helicopter instance to create tailboom.
/// </summary>
/// <param name="parent"></param>
/// <param name="pos"></param>
/// <param name="rotorRotation"></param>
void instantiateTailBoom(Transform parent, Vector3f pos, GLfloat rotorRotation);
/// <summary>
/// this method is for helicopter instance to create windskid.
/// </summary>
/// <param name="parent"></param>
/// <param name="pos"></param>
void instantiateWindSkid(Transform parent, Vector3f pos);
/// <summary>
/// this method clears out all MeshObject datas.
/// </summary>
/// <param name=""></param>
void cleanObjs(void);
/******************************************************************************
 * Entry Point (don't put anything except the main function here)
 ******************************************************************************/

void main(int argc, char** argv)
{
	// Initialize the OpenGL window.
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(Screen.x, Screen.y);
	glutCreateWindow("Project Assignment 2");

	// Set up the scene.
	init();

	// Disable key repeat (keyPressed or specialKeyPressed will only be called once when a key is first pressed).
	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);

	// Register GLUT callbacks.
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyPressed);
	glutSpecialFunc(specialKeyPressed);
	glutKeyboardUpFunc(keyReleased);
	glutSpecialUpFunc(specialKeyReleased);
	glutIdleFunc(idle);
	glutCloseFunc(cleanObjs);
	// Record when we started rendering the very first frame (which should happen after we call glutMainLoop).
	frameStartTime = (unsigned int)glutGet(GLUT_ELAPSED_TIME);

	// Enter the main drawing loop (this will never return).
	glutMainLoop();
}

/******************************************************************************
 * GLUT Callbacks (don't add any other functions here)
 ******************************************************************************/

 /*
	 Called when GLUT wants us to (re)draw the current animation frame.

	 Note: This function must not do anything to update the state of our simulated
	 world. Animation (moving or rotating things, responding to keyboard input,
	 etc.) should only be performed within the think() function provided below.
 */
void display(void)
{

	/*
		TEMPLATE: REPLACE THIS COMMENT WITH YOUR DRAWING CODE

		Separate reusable pieces of drawing code into functions, which you can add
		to the "Animation-Specific Functions" section below.

		Remember to add prototypes for any new functions to the "Animation-Specific
		Function Prototypes" section near the top of this template.
	*/
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//initialize the camear angle, camera distanec and camera height.
	GLfloat cameraAngle = (GLfloat)((heliYawAngleY - 180.0f) * (PI / 180.0f));
	GLfloat cameraDistance = 30.0f;
	GLfloat cameraHeight = 15.0f;
	// apply all values to x and y, z positons of the camera vector3 positions.
	camera.x = heliPosition.x + (GLfloat)(sin(cameraAngle) * cameraDistance);
	camera.y = heliPosition.y + cameraHeight;
	camera.z = heliPosition.z + (GLfloat)(cos(cameraAngle) * cameraDistance);
	//make the camera object look at the helicopter object.
	gluLookAt(camera.x, camera.y, camera.z, heliPosition.x, heliPosition.y, heliPosition.z, 0, 1, 0);
	//update the snow and fire particle systems in real time, snow will be colored as white and fire will be red.
	updateParticleSystem(P_SNOW_NUM_PARTICLES, P_SNOW_GRAVITY, P_SNOW_LIFE, initialSnowPositions, snowParticleSystem, P_SNOW_RANGE);
	drawParticleSystem(P_SNOW_NUM_PARTICLES, initialSnowPositions, snowParticleSystem, (Color3f) { 1, 1, 1 }, startSnowPosition);
	updateParticleSystem(P_FIRE_NUM_PARTICLES, P_FIRE_GRAVITY, P_FIRE_LIFE, initialFirePositions, fireParticleSystem, P_FIRE_RANGE);
	drawParticleSystem(P_FIRE_NUM_PARTICLES, initialFirePositions, fireParticleSystem, (Color3f) { 0.4f, 0, 0 }, startFirePosition);
	//instantiate all land ground object and generate random positions.
	for (int i = 0; i < 10; i++) {
		instantiateGround((Vector3f) { randomPostions[i].x, randomPostions[i].y, randomPostions[i].z });
	}
	//instantiate all old windmill object and generate random positions and rotations.
	for (int j = 0; j < 10; j++) {
		instantiateOldWindMill((Vector3f) { randomWindPositions[j].x, randomWindPositions[j].y, randomWindPositions[j].z }, (Quaternionf) { 0, randomRotations[j].rotY, 0, 0, randomRotations[j].angleY, 0 }, lightSourceRotation);
	}
	//instantiate the helicopter with realtime rotation value, and insert quaternion angle value heliptching angle to the x, heli yaw angle to the y, lastly helirollanlge to be z.
	instantiateHelicopter(heliPosition, rotorRotation*2, (Quaternionf) { 1, 1, 1, (GLfloat)heliPitchAngleX, (GLfloat)heliYawAngleY, (GLfloat)heliRollAngleZ });
	//instantiate helipad the respawn position.
	instantiateHeliPad((Vector3f) { 0, 0, -380 }, (Scalef) {(UNIT * 10),(UNIT * 0.2f),(UNIT * 10) });
	//instantiate light hosue with the realtime rotation value.
	instantiateLightHouse((Vector3f) { 0, 0, 380 }, lightSourceRotation);
	//instantiate windmill with the realtime rotation value, light source.
	instantiateWindMill((Vector3f) { 100, 0, 100 }, (Quaternionf) { 0, 1, 0, 0, -45, 0 }, lightSourceRotation,GL_LIGHT5);
	instantiateWindMill((Vector3f) { 200, 0, 200 }, (Quaternionf) { 0, 1, 0, 0, -50, 0 }, lightSourceRotation,GL_LIGHT6);
	instantiateWindMill((Vector3f) { 0, 0, 0 }, (Quaternionf) { 0, 1, 0, 0, -55, 0 }, lightSourceRotation, GL_LIGHT7);
	//instantiate the skybox and attach to the world map.
	instantiateSkyBox((Vector3f) { 0, 50, 0 }, (Scalef) { 500, 500, 500 });
	//draw sea ground plane with the realtime rotation value.
	drawGround(diffuseMaterial,(Color3f){0,0.6f,0.6f}, (Vector3f) { 0, 0, 0 }, 100, waterTexture, lightSourceRotation);
	glutSwapBuffers();
}

/*
	Called when the OpenGL window has been resized.
*/
void reshape(int width, int height)
{
	glViewport(0, 0, (GLint)width, (GLint)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//set z far to be 1200 so there is no clipping issue.
	gluPerspective(70, (GLfloat)width / (GLfloat)height, 1.0f, 1200.0f);
}

/*
	Called each time a character key (e.g. a letter, number, or symbol) is pressed.
*/
void keyPressed(unsigned char key, int x, int y)
{
	switch (tolower(key)) {

		/*
			Keyboard-Controlled Motion Handler - DON'T CHANGE THIS SECTION

			Whenever one of our movement keys is pressed, we do two things:
			(1) Update motionKeyStates to record that the key is held down. We use
				this later in the keyReleased callback.
			(2) Update the relevant axis in keyboardMotion to set the new direction
				we should be moving in. The most recent key always "wins" (e.g. if
				you're holding down KEY_MOVE_LEFT then also pressed KEY_MOVE_RIGHT,
				our object will immediately start moving right).
		*/
	case KEY_MOVE_FORWARD:
		motionKeyStates.MoveForward = KEYSTATE_DOWN;
		keyboardMotion.Surge = MOTION_FORWARD;
		break;
	case KEY_MOVE_BACKWARD:
		motionKeyStates.MoveBackward = KEYSTATE_DOWN;
		keyboardMotion.Surge = MOTION_BACKWARD;
		break;
	case KEY_MOVE_LEFT:
		motionKeyStates.MoveLeft = KEYSTATE_DOWN;
		keyboardMotion.Sway = MOTION_LEFT;
		break;
	case KEY_MOVE_RIGHT:
		motionKeyStates.MoveRight = KEYSTATE_DOWN;
		keyboardMotion.Sway = MOTION_RIGHT;
		break;

		/*
			Other Keyboard Functions (add any new character key controls here)

			Rather than using literals (e.g. "t" for spotlight), create a new KEY_
			definition in the "Keyboard Input Handling Setup" section of this file.
			For example, refer to the existing keys used here (KEY_MOVE_FORWARD,
			KEY_MOVE_LEFT, KEY_EXIT, etc).
		*/
	case KEY_RENDER_FILL:
		renderFillEnabled = !renderFillEnabled;
		break;
	case KEY_RENDER_AXIS:
		//shows the origin axes of the current object.
		axesEnabled = !axesEnabled;
		break;
	case KEY_EXIT:
		exit(0);
		break;
	}
}

/*
	Called each time a "special" key (e.g. an arrow key) is pressed.
*/
void specialKeyPressed(int key, int x, int y)
{
	switch (key) {

		/*
			Keyboard-Controlled Motion Handler - DON'T CHANGE THIS SECTION

			This works as per the motion keys in keyPressed.
		*/
	case SP_KEY_MOVE_UP:
		motionKeyStates.MoveUp = KEYSTATE_DOWN;
		keyboardMotion.Heave = MOTION_UP;
		break;
	case SP_KEY_MOVE_DOWN:
		motionKeyStates.MoveDown = KEYSTATE_DOWN;
		keyboardMotion.Heave = MOTION_DOWN;
		break;
	case SP_KEY_TURN_LEFT:
		motionKeyStates.TurnLeft = KEYSTATE_DOWN;
		keyboardMotion.Yaw = MOTION_ANTICLOCKWISE;
		break;
	case SP_KEY_TURN_RIGHT:
		motionKeyStates.TurnRight = KEYSTATE_DOWN;
		keyboardMotion.Yaw = MOTION_CLOCKWISE;
		break;

		/*
			Other Keyboard Functions (add any new special key controls here)

			Rather than directly using the GLUT constants (e.g. GLUT_KEY_F1), create
			a new SP_KEY_ definition in the "Keyboard Input Handling Setup" section of
			this file. For example, refer to the existing keys used here (SP_KEY_MOVE_UP,
			SP_KEY_TURN_LEFT, etc).
		*/
	}
}

/*
	Called each time a character key (e.g. a letter, number, or symbol) is released.
*/
void keyReleased(unsigned char key, int x, int y)
{
	switch (tolower(key)) {

		/*
			Keyboard-Controlled Motion Handler - DON'T CHANGE THIS SECTION

			Whenever one of our movement keys is released, we do two things:
			(1) Update motionKeyStates to record that the key is no longer held down;
				we need to know when we get to step (2) below.
			(2) Update the relevant axis in keyboardMotion to set the new direction
				we should be moving in. This gets a little complicated to ensure
				the controls work smoothly. When the user releases a key that moves
				in one direction (e.g. KEY_MOVE_RIGHT), we check if its "opposite"
				key (e.g. KEY_MOVE_LEFT) is pressed down. If it is, we begin moving
				in that direction instead. Otherwise, we just stop moving.
		*/
	case KEY_MOVE_FORWARD:
		motionKeyStates.MoveForward = KEYSTATE_UP;
		keyboardMotion.Surge = (motionKeyStates.MoveBackward == KEYSTATE_DOWN) ? MOTION_BACKWARD : MOTION_NONE;
		break;
	case KEY_MOVE_BACKWARD:
		motionKeyStates.MoveBackward = KEYSTATE_UP;
		keyboardMotion.Surge = (motionKeyStates.MoveForward == KEYSTATE_DOWN) ? MOTION_FORWARD : MOTION_NONE;
		break;
	case KEY_MOVE_LEFT:
		motionKeyStates.MoveLeft = KEYSTATE_UP;
		keyboardMotion.Sway = (motionKeyStates.MoveRight == KEYSTATE_DOWN) ? MOTION_RIGHT : MOTION_NONE;
		break;
	case KEY_MOVE_RIGHT:
		motionKeyStates.MoveRight = KEYSTATE_UP;
		keyboardMotion.Sway = (motionKeyStates.MoveLeft == KEYSTATE_DOWN) ? MOTION_LEFT : MOTION_NONE;
		break;

		/*
			Other Keyboard Functions (add any new character key controls here)

			Note: If you only care when your key is first pressed down, you don't have to
			add anything here. You only need to put something in keyReleased if you care
			what happens when the user lets go, like we do with our movement keys above.
			For example: if you wanted a spotlight to come on while you held down "t", you
			would need to set a flag to turn the spotlight on in keyPressed, and update the
			flag to turn it off in keyReleased.
		*/
	}
}

/*
	Called each time a "special" key (e.g. an arrow key) is released.
*/
void specialKeyReleased(int key, int x, int y)
{
	switch (key) {
		/*
			Keyboard-Controlled Motion Handler - DON'T CHANGE THIS SECTION

			This works as per the motion keys in keyReleased.
		*/
	case SP_KEY_MOVE_UP:
		motionKeyStates.MoveUp = KEYSTATE_UP;
		keyboardMotion.Heave = (motionKeyStates.MoveDown == KEYSTATE_DOWN) ? MOTION_DOWN : MOTION_NONE;
		break;
	case SP_KEY_MOVE_DOWN:
		motionKeyStates.MoveDown = KEYSTATE_UP;
		keyboardMotion.Heave = (motionKeyStates.MoveUp == KEYSTATE_DOWN) ? MOTION_UP : MOTION_NONE;
		break;
	case SP_KEY_TURN_LEFT:
		motionKeyStates.TurnLeft = KEYSTATE_UP;
		keyboardMotion.Yaw = (motionKeyStates.TurnRight == KEYSTATE_DOWN) ? MOTION_CLOCKWISE : MOTION_NONE;
		break;
	case SP_KEY_TURN_RIGHT:
		motionKeyStates.TurnRight = KEYSTATE_UP;
		keyboardMotion.Yaw = (motionKeyStates.TurnLeft == KEYSTATE_DOWN) ? MOTION_ANTICLOCKWISE : MOTION_NONE;
		break;

		/*
			Other Keyboard Functions (add any new special key controls here)

			As per keyReleased, you only need to handle the key here if you want something
			to happen when the user lets go. If you just want something to happen when the
			key is first pressed, add you code to specialKeyPressed instead.
		*/
	}
}

/*
	Called by GLUT when it's not rendering a frame.

	Note: We use this to handle animation and timing. You shouldn't need to modify
	this callback at all. Instead, place your animation logic (e.g. moving or rotating
	things) within the think() method provided with this template.
*/
void idle(void)
{
	// Wait until it's time to render the next frame.

	unsigned int frameTimeElapsed = (unsigned int)glutGet(GLUT_ELAPSED_TIME) - frameStartTime;
	if (frameTimeElapsed < FRAME_TIME)
	{
		// This frame took less time to render than the ideal FRAME_TIME: we'll suspend this thread for the remaining time,
		// so we're not taking up the CPU until we need to render another frame.
		unsigned int timeLeft = FRAME_TIME - frameTimeElapsed;
		Sleep(timeLeft);
	}

	// Begin processing the next frame.

	frameStartTime = glutGet(GLUT_ELAPSED_TIME); // Record when we started work on the new frame.

	think(); // Update our simulated world before the next call to display().

	glutPostRedisplay(); // Tell OpenGL there's a new frame ready to be drawn.
}

/******************************************************************************
 * Animation-Specific Functions (Add your own functions at the end of this section)
 ******************************************************************************/

 /*
	 Initialise OpenGL and set up our scene before we begin the render loop.
 */
void init(void)
{
	glClearColor(0, 0, 0, 0.0f);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	//Enable use of fog
	glEnable(GL_FOG);
	// define the color of the fog, white
	GLfloat fogColor[4] = { 1,1,1,1 };
	// set the color of the fog
	glFogfv(GL_FOG_COLOR, fogColor);
	//set the fog mode to be exponential
	glFogf(GL_FOG_MODE, GL_EXP);
	//set the fog density
	glFogf(GL_FOG_DENSITY, 0.0010f);
	glHint(GL_FOG_HINT, GL_NICEST);

	//initialize snow positions with x,y and z there are 4 Vectors.
	initialSnowPositions[0].x = -1;
	initialSnowPositions[0].y = startSnowPosition.y;
	initialSnowPositions[0].z = -1;
	initialSnowPositions[1].x = -1;
	initialSnowPositions[1].y = startSnowPosition.y;
	initialSnowPositions[1].z = 1;
	initialSnowPositions[2].x = 1;
	initialSnowPositions[2].y = startSnowPosition.y;
	initialSnowPositions[2].z = 1;
	initialSnowPositions[3].x = 1;
	initialSnowPositions[3].y = startSnowPosition.y;
	initialSnowPositions[3].z = -1;
	//initialize fire positions with x,y and z there are 4 Vectors.
	initialFirePositions[0].x =  -1;
	initialFirePositions[0].y =  0;
	initialFirePositions[0].z =  -1;
	initialFirePositions[1].x =  -1;
	initialFirePositions[1].y = 0;
	initialFirePositions[1].z =  1;
	initialFirePositions[2].x =  1;
	initialFirePositions[2].y = 0;
	initialFirePositions[2].z =  1;
	initialFirePositions[3].x =  1;
	initialFirePositions[3].y = 0;
	initialFirePositions[3].z =  -1;
	//initialize with random postions and rotations for the land ground and windmill.
	for (int i = 0; i < 10; i++) {
		randomPostions[i].x = (GLfloat)getRandomRange(300);
		randomPostions[i].y = 0;
		randomPostions[i].z = (GLfloat)getRandomRange(300);
		//for the random wind
		randomWindPositions[i].x = (GLfloat)getRandomRange(300);
		randomWindPositions[i].y = 0;
		randomWindPositions[i].z = (GLfloat)getRandomRange(300);
		//for the random rotations
		randomRotations[i].angleY = (GLfloat)getRandomRange(360);
		randomRotations[i].rotY = 1;
 	}
	//initialize the particle systems within certain values.
	initParticleSystem(P_SNOW_NUM_PARTICLES,P_SNOW_LIFE,snowParticleSystem, initialSnowPositions, P_SNOW_RANGE);
	initParticleSystem(P_FIRE_NUM_PARTICLES, P_FIRE_LIFE, fireParticleSystem, initialFirePositions, P_FIRE_RANGE);
	//initialize the direction light renderer.
	initLights();
	//initialize all the freeglut quadric objects.
	sphereObj = gluNewQuadric();
	cylinderObj = gluNewQuadric();
	diskObj = gluNewQuadric();
	//initialize all the OBJ types from the assets folder.
	sphereMeshObj = loadMeshObject("assets/sphere.obj");
	cubeMeshObj = loadMeshObject("assets/cube.obj");
	coneMeshObj = loadMeshObject("assets/cone.obj");
	cylinderMeshObj = loadMeshObject("assets/cylinder.obj");
	monkeyMeshObj = loadMeshObject("assets/monkey.obj");
	groundMeshObj = loadMeshObject("assets/ground.obj");
	//initialize all the PPM texutre maps and insert them as GLint data.
	waterTexture = loadPPM("textures/water.ppm");
	brickTexture = loadPPM("textures/brick.ppm");
	nightSkyBoxTexture = loadPPM("textures/night.ppm");
	marbleTexture = loadPPM("textures/marble.ppm");
	woodTexture = loadPPM("textures/wood.ppm");
	dirtTexture = loadPPM("textures/dirt.ppm");
	heliPadTexture = loadPPM("textures/helipad.ppm");
	// Anything that relies on lighting or specifies normals must be initialised after initLights
}

/*
	Advance our animation by FRAME_TIME milliseconds.

	Note: Our template's GLUT idle() callback calls this once before each new
	frame is drawn, EXCEPT the very first frame drawn after our application
	starts. Any setup required before the first frame is drawn should be placed
	in init().
*/
void think(void)
{
	/*
		TEMPLATE: REPLACE THIS COMMENT WITH YOUR ANIMATION/SIMULATION CODE

		In this function, we update all the variables that control the animated
		parts of our simulated world. For example: if you have a moving box, this is
		where you update its coordinates to make it move. If you have something that
		spins around, here's where you update its angle.

		NOTHING CAN BE DRAWN IN HERE: you can only update the variables that control
		how everything will be drawn later in display().

		How much do we move or rotate things? Because we use a fixed frame rate, we
		assume there's always FRAME_TIME milliseconds between drawing each frame. So,
		every time think() is called, we need to work out how far things should have
		moved, rotated, or otherwise changed in that period of time.

		Movement example:
		* Let's assume a distance of 1.0 GL units is 1 metre.
		* Let's assume we want something to move 2 metres per second on the x axis
		* Each frame, we'd need to update its position like this:
			x += 2 * (FRAME_TIME / 1000.0f)
		* Note that we have to convert FRAME_TIME to seconds. We can skip this by
		  using a constant defined earlier in this template:
			x += 2 * FRAME_TIME_SEC;

		Rotation example:
		* Let's assume we want something to do one complete 360-degree rotation every
		  second (i.e. 60 Revolutions Per Minute, or RPM).
		* Each frame, we'd need to update our object's angle like this (we'll use the
		  FRAME_TIME_SEC constant as per the example above):
			a += 360 * FRAME_TIME_SEC;

		This works for any type of "per second" change: just multiply the amount you'd
		want to move in a full second by FRAME_TIME_SEC, and add or subtract that
		from whatever variable you're updating.

		You can use this same approach to animate other things like color, opacity,
		brightness of lights, etc.
	*/

	/*
		Keyboard motion handler: complete this section to make your "player-controlled"
		object respond to keyboard input.
	*/
	if (keyboardMotion.Yaw != MOTION_NONE) {
		//heli y position will be added if the yaw angle is enabled.
		heliYawAngleY += RPM * FRAME_TIME_SEC * keyboardMotion.Yaw;
		//if heli angle is less than max angle.
		if (heliYawAngleY > MAX_ANGLE)
			//it will substract the max value.
			heliYawAngleY -= MAX_ANGLE;
			//other wise it will added
		else if (heliYawAngleY < 0.0f)
			heliYawAngleY += MAX_ANGLE;
	}
	if (keyboardMotion.Surge != MOTION_NONE) {
		/* TEMPLATE: Move your object backward if .Surge < 0, or forward if .Surge > 0 */
		//heli position x will be added with the yaw angle value if the helicopter surges.
		heliPosition.x += (GLfloat)(sinf((GLfloat)heliYawAngleY * (GLfloat)DEG_TO_RAD) * keyboardMotion.Surge * FRAME_TIME_SEC * HELI_SPEED);
		//heli position z will be added with yaw angle value if the helicopter surges.
		heliPosition.z += (GLfloat)(cosf((GLfloat)heliYawAngleY * (GLfloat)DEG_TO_RAD) * keyboardMotion.Surge * FRAME_TIME_SEC * HELI_SPEED);
		//heli position x will be added when the helicopeter surges.
		heliPitchAngleX += 1.0f * keyboardMotion.Surge;
		// if max ptictch reached it it will limit the angle positions
		if (heliPitchAngleX > MAX_PITCH_ANGLE) { heliPitchAngleX = MAX_PITCH_ANGLE; }
		if (heliPitchAngleX < -MAX_PITCH_ANGLE) { heliPitchAngleX = -MAX_PITCH_ANGLE; }
	}
	else {
		//other wise it will add the pitch x value or subtract the pitch x angle.
		if (heliPitchAngleX < 0 && heliPitchAngleX >= -limitedTurningPoint) {
			heliPitchAngleX += 0.5f;
		}

		if (heliPitchAngleX > 0 && heliPitchAngleX <= limitedTurningPoint) {
			heliPitchAngleX -= 0.5f;
		}
	}
	if (keyboardMotion.Sway != MOTION_NONE) {
		/* TEMPLATE: Move (strafe) your object left if .Sway < 0, or right if .Sway > 0 */
		/* TEMPLATE: Move (strafe) your object left if .Sway < 0, or right if .Sway > 0 */
		// need to flip inverse somewhere in here
		//check and add x position if the heliyawangle y is sway.
		heliPosition.x += (GLfloat)( - cos(heliYawAngleY * (PI / 180.0f)) * keyboardMotion.Sway * FRAME_TIME_SEC * 32.0f);
		//check and add z position if the heliyawangle y is sway.
		heliPosition.z += (GLfloat)(sin(heliYawAngleY * (PI / 180.0f)) * keyboardMotion.Sway * FRAME_TIME_SEC * 32.0f);
		//add helirollangle when the sway is enabled.
		heliRollAngleZ += 1.0f * keyboardMotion.Sway;
		//if the angle is limited set to the the limited turning point. will stay in that particular angle.
		if (heliRollAngleZ > limitedTurningPoint) { heliRollAngleZ = limitedTurningPoint; }
		if (heliRollAngleZ < -limitedTurningPoint) { heliRollAngleZ = -limitedTurningPoint; }

	}
	else {
		//if the angle is not limited start adding angle Z axis
		if (heliRollAngleZ < 0 && heliRollAngleZ >= -limitedTurningPoint) {
			heliRollAngleZ += 0.5f;
		}
		//if the angle is not limited start subtracting angle Z axis.
		if (heliRollAngleZ > 0 && heliRollAngleZ <= limitedTurningPoint) {
			heliRollAngleZ -= 0.5f;
		}
	}
	if (keyboardMotion.Heave != MOTION_NONE) {
		/* TEMPLATE: Move your object down if .Heave < 0, or up if .Heave > 0 */
		//add y positon if the heave is enabled.
		heliPosition.y += speed * FRAME_TIME_SEC * keyboardMotion.Heave;
		//set the ground detection to be -0.5f it will not go below that value.
		if (heliPosition.y - 0.5f < 0 && heliPosition.y != 0)
		{
			//if the position is 0.5f it will set speed to 0.
			heliPosition.y = 0.5f;
			if (speed >= 0) {
				speed = 0.0f;
			}
		}
	}

	//keep adding speed if the start speed is greater than the speed.
	if (speed <= startSpeed) {
		speed += 0.035f;
	}
	//rotor rotation will rotated in 180 degrees only
	if (rotorRotation >= 180)
	{
		rotorRotation = 0.0f;
	}
	//rotor rotation will be keep adding.
	rotorRotation += speed;
	//keep adding if the light start speed is greater than the light speed. 
	if (lightSpeed <= lightStartSpeed) {
		lightSpeed += 0.0015f;
	}
	//light rotation will be rotated in 360 degrees only
	if (lightSourceRotation >= 360) {
		lightSourceRotation = 0.0f;
	}
	//light source rotation will be keep adding.
	lightSourceRotation += lightSpeed;
}

/*
	Initialise OpenGL lighting before we begin the render loop.

	Note (advanced): If you're using dynamic lighting (e.g. lights that move around, turn on or
	off, or change colour) you may want to replace this with a drawLights function that gets called
	at the beginning of display() instead of init().
*/
void initLights(void)
{
	// Simple lighting setup
	GLfloat globalAmbient[] = { 0.25f, 0.25f, 0.25f, 0.2f };
	GLfloat lightPosition[] = { 5.0f, 5.0f, 5.0f, 1.0f };
	//this changes the light brightness.
	GLfloat ambientLight[] = { 0, 0, 0, 0.2f };
	GLfloat diffuseLight[] = { 0, 0, 0, 0.2f };
	GLfloat specularLight[] = { 0, 0, 0, 0.2f };

	// Configure global ambient lighting.
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

	// Configure Light 0.
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);

	// Enable lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	
	// Make GL normalize the normal vectors we supply.
	glEnable(GL_NORMALIZE);

	// Enable use of simple GL colours as materials.
	glEnable(GL_COLOR_MATERIAL);
}


void cleanObjs(void) {
	//clear all Mesh Object Datas.
	close(cubeMeshObj);
	close(sphereMeshObj);
	close(coneMeshObj);
	close(cylinderMeshObj);
	close(monkeyMeshObj);
}
/// <summary>
/// this method instantiates the helipad ground object.
/// </summary>
/// <param name="pos"></param>
/// <param name="scale"></param>
void instantiateHeliPad(Vector3f pos, Scalef scale) {
	//create the base transform object with parent position and unit scales.
	Transform base = setTransform(pos, (Quaternionf) { 0, 0, 0, 0, 0, 0 }, (Scalef) {(UNIT * 1), (UNIT * 0.2f), (UNIT * 1)}, (Color3f) { 1, 1, 1 });
	Transform helipad = setTransform((Vector3f) { 0, 0, 0 }, (Quaternionf) { 0, 0, 0, 0, 0, 0 }, scale, (Color3f) { 1, 1, 1 });
	//create cube OBJ.
	getTransformOBJ(base, helipad, diffuseMaterial, axesEnabled, cubeMeshObj, heliPadTexture);
}
/// <summary>
/// this method instantiates the skybox object.
/// </summary>
/// <param name="pos"></param>
/// <param name="scale"></param>
void instantiateSkyBox(Vector3f pos, Scalef scale) {
	//create the base transform object with parent position and unit scales.
	Transform base = setTransform(pos, (Quaternionf) { 0, 0, 0, 0, 0, 0 }, (Scalef) { UNIT, UNIT, UNIT }, (Color3f) { 1, 1, 1 });
	Transform skybox = setTransform((Vector3f) { 0, 0, 0 }, (Quaternionf) { 0, 0, 0, 0, 0, 0 }, scale, (Color3f) { 1, 1, 1 });
	//create cube OBJ.
	getTransformOBJ(base, skybox, diffuseMaterial, axesEnabled, cubeMeshObj, nightSkyBoxTexture);
}
/// <summary>
/// this method instantiates the land ground object.
/// </summary>
/// <param name="pos"></param>
void instantiateGround(Vector3f pos) {
	//create the base transform object with parent position and unit scales.
	Transform ground = setTransform(pos, (Quaternionf) { 0, 0, 0, 0, 0, 0 }, (Scalef) { (UNIT *3), (UNIT *3), (UNIT*3) }, (Color3f) { 1, 1, 1 });
	//create the transformm object positions and apply.
	Transform land1 = setTransform((Vector3f) { 0, 1, 0 }, (Quaternionf) { 0, 0, 0, 0, 0, 0 }, (Scalef) { 5, 5, 5 }, (Color3f) { 1, 1, 1 });
	Transform land2 = setTransform((Vector3f) { 10, 1, 10 }, (Quaternionf) { 0, 1, 0, 0, 45, 0 }, (Scalef) { 5, 5, 5 }, (Color3f) { 1, 1, 1 });
	Transform land3 = setTransform((Vector3f) { -10, 1, -10 }, (Quaternionf) { 0, 1, 0, 0, 90, 0 }, (Scalef) { 5, 5, 5 }, (Color3f) { 1, 1, 1 });
	//create cube Objects.
	getTransformOBJ(ground, land1, diffuseMaterial, axesEnabled, groundMeshObj, dirtTexture);
	getTransformOBJ(ground, land2, diffuseMaterial, axesEnabled, groundMeshObj, dirtTexture);
	getTransformOBJ(ground, land3, diffuseMaterial, axesEnabled, groundMeshObj, dirtTexture);
}
/// <summary>
/// this method instantiates the old windmill object.
/// </summary>
/// <param name="pos"></param>
/// <param name="transformRotation"></param>
/// <param name="rot"></param>
void instantiateOldWindMill(Vector3f pos, Quaternionf transformRotation, GLfloat rot) {
	//create the base transform object with parent position and unit scales.
	Transform base = setTransform(pos, transformRotation, (Scalef) { (UNIT * 3), (UNIT * 3), (UNIT * 3) }, (Color3f) { 1, 1, 1 });
	//create the transformm object positions and apply. the following OBJ and Freeglut objects will be created.
	Transform pillar = setTransform((Vector3f) { 0, 4, 0 }, (Quaternionf) { 0, 0, 0, 0, 0, 0 }, (Scalef) { 2, 4, 2 }, (Color3f) { 1, 1, 1 });
	getTransformOBJ(base, pillar, diffuseMaterial, axesEnabled, cylinderMeshObj, brickTexture);
	pillar = setTransform((Vector3f) { 0, 12, 0 }, (Quaternionf) { 0, 0, 0, 0, 0, 0 }, (Scalef) { 2, 4, 2 }, (Color3f) { 1, 1, 1 });
	getTransformOBJ(base, pillar, diffuseMaterial, axesEnabled, cylinderMeshObj, brickTexture);
	pillar = setTransform((Vector3f) { 0, 20, 0 }, (Quaternionf) { 0, 0, 0, 0, 0, 0 }, (Scalef) { 2, 4, 2 }, (Color3f) { 1, 1, 1 });
	getTransformOBJ(base, pillar, diffuseMaterial, axesEnabled, cylinderMeshObj, brickTexture);
	pillar = setTransform((Vector3f) { 0, 28, 0 }, (Quaternionf) { 0, 0, 0, 0, 0, 0 }, (Scalef) { 2, 4, 2 }, (Color3f) { 1, 1, 1 });
	getTransformOBJ(base, pillar, diffuseMaterial, axesEnabled, cylinderMeshObj, brickTexture);
	pillar = setTransform((Vector3f) { 0, 32, 0 }, (Quaternionf) { 1, 0, 0, 90, 0, 0 }, (Scalef) { 2, 3, 2 }, (Color3f) { 1, 1, 1 });
	getTransformOBJ(base, pillar, diffuseMaterial, axesEnabled, cylinderMeshObj, brickTexture);
	pillar = setTransform((Vector3f) { 0, 32, 3 }, (Quaternionf) { 1, 0, 0, 90, 0, 0 }, (Scalef) { 1, 3, 1 }, (Color3f) { 0, 0, 0 });
	getTransformOBJ(base, pillar, diffuseMaterial, axesEnabled, cylinderMeshObj, marbleTexture);
	Transform blade = setTransform((Vector3f) { 0, 32, 4 }, (Quaternionf) { 1, 1, 1, -90, (120 + rot), -90 }, (Scalef) { 0.10f, 0.25f, 0.25f }, (Color3f) { 1, 1, 1 });
	getTransformCylinder(base, blade, ambientMaterial, 5, 5, 100, 5, 5, axesEnabled, cylinderObj);
	blade = setTransform((Vector3f) { 0, 32, 4 }, (Quaternionf) { 1, 1, 1, -90, (240 + rot), -90 }, (Scalef) { 0.10f, 0.25f, 0.25f }, (Color3f) { 1, 1, 1 });
	getTransformCylinder(base, blade, ambientMaterial, 5, 5, 100, 5, 5, axesEnabled, cylinderObj);
	blade = setTransform((Vector3f) { 0, 32, 4 }, (Quaternionf) { 1, 1, 1, -90, (360 + rot), -90 }, (Scalef) { 0.10f, 0.25f, 0.25f }, (Color3f) { 1, 1, 1 });
	getTransformCylinder(base, blade, ambientMaterial, 5, 5, 100, 5, 5, axesEnabled, cylinderObj);

}
/// <summary>
/// this method instantiates the animated windmill object.
/// </summary>
/// <param name="pos"></param>
/// <param name="transformRotation"></param>
/// <param name="rot"></param>
/// <param name="lightName"></param>
void instantiateWindMill(Vector3f pos,Quaternionf transformRotation, GLfloat rot, GLenum lightName) {
	//create the base transform object with parent position and unit scales.
	Transform base = setTransform(pos, transformRotation, (Scalef) { (UNIT * 3), (UNIT * 3), (UNIT * 3) }, (Color3f) { 1, 1, 1 });
	//create the transformm object positions and apply. the following OBJ and Freeglut objects will be created.
	Transform pillar = setTransform((Vector3f) { 0, 4, 0 }, (Quaternionf) { 0, 0, 0, 0, 0, 0 }, (Scalef) { 2, 4, 2 }, (Color3f) { 1, 1, 1 });
	getTransformOBJ(base, pillar, diffuseMaterial, axesEnabled, cylinderMeshObj, marbleTexture);
	pillar = setTransform((Vector3f) { 0, 12, 0 }, (Quaternionf) { 0, 0, 0, 0, 0, 0 }, (Scalef) { 2, 4, 2 }, (Color3f) { 1, 1, 1 });
	getTransformOBJ(base, pillar, diffuseMaterial, axesEnabled, cylinderMeshObj, marbleTexture);
	pillar = setTransform((Vector3f) { 0, 20, 0 }, (Quaternionf) { 0, 0, 0, 0, 0, 0 }, (Scalef) { 2, 4, 2 }, (Color3f) { 1, 1, 1 });
	getTransformOBJ(base, pillar, diffuseMaterial, axesEnabled, cylinderMeshObj, marbleTexture);
	pillar = setTransform((Vector3f) { 0, 28, 0 }, (Quaternionf) { 0, 0, 0, 0, 0, 0 }, (Scalef) { 2, 4, 2 }, (Color3f) { 1, 1, 1 });
	getTransformOBJ(base, pillar, diffuseMaterial, axesEnabled, cylinderMeshObj, marbleTexture);
	pillar = setTransform((Vector3f) { 0, 32, 0 }, (Quaternionf) { 1, 0, 0, 90, 0, 0 }, (Scalef) { 2, 3, 2 }, (Color3f) { 1, 1, 1 });
	Transform lightPosition = setTransform((Vector3f) { 0, 32, 8 }, (Quaternionf) { 1, 0, 0, -45, 0, 0 }, (Scalef) { 2, 3, 2 }, (Color3f) { 1, 1, 1 });
	getTransformOBJ(base, pillar, diffuseMaterial, axesEnabled, cylinderMeshObj, marbleTexture);
	drawSpotlight(lightName, lightMaterial, base, lightPosition, 100, 30, axesEnabled);
	pillar = setTransform((Vector3f) { 0, 32, 3 }, (Quaternionf) { 1, 0, 0, 90, 0, 0 }, (Scalef) { 1, 3, 1 }, (Color3f) { 0, 0, 0 });
	getTransformOBJ(base, pillar, diffuseMaterial, axesEnabled, cylinderMeshObj, marbleTexture);
	Transform blade = setTransform((Vector3f) { 0, 32, 4 }, (Quaternionf) { 1, 1, 1, -90, (120 + rot), -90 }, (Scalef) { 0.10f, 0.25f, 0.25f }, (Color3f) { 1, 1, 1 });
	getTransformCylinder(base, blade, ambientMaterial, 5, 5, 100, 5, 5, axesEnabled, cylinderObj);
	blade = setTransform((Vector3f) { 0, 32, 4 }, (Quaternionf) { 1, 1, 1, -90, (240 + rot), -90 }, (Scalef) { 0.10f, 0.25f, 0.25f }, (Color3f) { 1, 1, 1 });
	getTransformCylinder(base, blade, ambientMaterial, 5, 5, 100, 5, 5, axesEnabled, cylinderObj);
	blade = setTransform((Vector3f) { 0, 32, 4 }, (Quaternionf) { 1, 1, 1, -90, (360 + rot), -90 }, (Scalef) { 0.10f, 0.25f, 0.25f }, (Color3f) { 1, 1, 1 });
	getTransformCylinder(base, blade, ambientMaterial, 5, 5, 100, 5, 5, axesEnabled, cylinderObj);
	
}
/// <summary>
/// this method instantiates the light house object.
/// </summary>
/// <param name="pos"></param>
/// <param name="rot"></param>
void instantiateLightHouse(Vector3f pos, GLfloat rot) {
	//create the base transform object with parent position and unit scales.
	Transform lightHouse = setTransform(pos, (Quaternionf) { 0, 0, 0, 0, 0, 0 }, (Scalef) { UNIT, UNIT, UNIT }, (Color3f) { 1, 1, 1 });
	//create the transformm object positions and apply. the following OBJ and Freeglut objects will be created.
	Transform lightHouseBody1 = setTransform((Vector3f) { 0, 4, 0 }, (Quaternionf) { 0, 0, 0, 0, 0, 0 }, (Scalef) { 5, 5, 5 }, (Color3f) { 1, 1, 1 });
	Transform lightHouseBody2 = setTransform((Vector3f) { 0, 14, 0 }, (Quaternionf) { 0, 0, 0, 0, 0, 0 }, (Scalef) { 5, 5, 5 }, (Color3f) { 1, 1, 1 });
	Transform lightHouseBody3 = setTransform((Vector3f) { 0, 24, 0 }, (Quaternionf) { 0, 0, 0, 0, 0, 0 }, (Scalef) { 5, 5, 5 }, (Color3f) { 1, 1, 1 });
	Transform lightHouseBody4 = setTransform((Vector3f) { 0, 34, 0 }, (Quaternionf) { 0, 0, 0, 0, 0, 0 }, (Scalef) { 5, 5, 5 }, (Color3f) { 1, 1, 1 });
	Transform lightHouseTop = setTransform((Vector3f) { 0, 54, 0 }, (Quaternionf) { 0, 0, 0, 0, 0, 0 }, (Scalef) { 5, 5, 5 }, (Color3f) { 1, 1, 1 });
	Transform lightContainer = setTransform((Vector3f) { lightHouse.position.x, 44, lightHouse.position.z }, (Quaternionf) { 0, 1, 0, 0, rot, 0 }, (Scalef) { UNIT, UNIT, UNIT }, (Color3f) { 1, 1, 1 });
	Transform lightSource = setTransform((Vector3f) { 0, 0, 0}, (Quaternionf) { 1, 1, 0, 45, rot, 0 }, (Scalef) { UNIT, UNIT, UNIT }, (Color3f) { 1, 1, 1 });
	getTransformOBJ(lightHouse, lightHouseBody1, diffuseMaterial, axesEnabled, cylinderMeshObj, brickTexture);
	getTransformOBJ(lightHouse, lightHouseBody2, diffuseMaterial, axesEnabled, cylinderMeshObj, brickTexture);
	getTransformOBJ(lightHouse, lightHouseBody3, diffuseMaterial, axesEnabled, cylinderMeshObj, brickTexture);
	getTransformOBJ(lightHouse, lightHouseBody4, diffuseMaterial, axesEnabled, cylinderMeshObj, brickTexture);
	getTransformOBJ(lightHouse, lightHouseTop, diffuseMaterial, axesEnabled, coneMeshObj, brickTexture);
	getTransformOBJ(defaultTransform, lightContainer, diffuseMaterial, axesEnabled, monkeyMeshObj, brickTexture);
	drawSpotlight(GL_LIGHT4, lightMaterial, lightContainer, lightSource, 100, 50, axesEnabled);
	instantiateGround(pos);
}
/// <summary>
/// this method instantiates the helicopter object instance with rotation parameter.
/// </summary>
/// <param name="pos"></param>
/// <param name="rot"></param>
/// <param name="quaternionf"></param>
void instantiateHelicopter(Vector3f pos, GLfloat rot, Quaternionf quaternionf) {
	//check if the helicopter object is face rendering or not rendering.
	if (sphereObj != NULL && cylinderObj != NULL) {
		renderFillEnabled ? gluQuadricDrawStyle(sphereObj, GLU_FILL) : gluQuadricDrawStyle(sphereObj, GLU_LINE);
		renderFillEnabled ? gluQuadricDrawStyle(cylinderObj, GLU_FILL) : gluQuadricDrawStyle(cylinderObj, GLU_LINE);
		glPushMatrix();
		//get the current rotation of the realtime value.
		instantiateMainBody(pos, rot - 180, quaternionf);
		glPopMatrix();
	}
}
/// <summary>
/// this mehtod is for helicopter instance to create main body part of the helicopter.
/// </summary>
/// <param name="pos"></param>
/// <param name="rotorRotation"></param>
/// <param name="quaternionf"></param>
void instantiateMainBody(Vector3f pos, GLfloat rotorRotation, Quaternionf quaternionf) {
	//create the base transform object with parent position and unit scales.
	Transform heliBody = setTransform((Vector3f) { pos.x + 0, pos.y + 2.0f, pos.z + 0 }, (Quaternionf) { 1, 1, 1, quaternionf.angleX, quaternionf.angleY, quaternionf.angleZ }, (Scalef) { 0.15f, 0.15f, 0.15f }, (Color3f) { 0, 0, 0 });
	//create the transformm object positions and apply. the following OBJ and Freeglut objects will be created.
	getTransformCube((Transform) { (Vector3f) { 0, 0, 0 }, (Quaternionf) { 0, 0, 0, 0, 0, 0 }, (Scalef) { UNIT, UNIT, UNIT } }, heliBody, diffuseMaterial,axesEnabled,renderFillEnabled);
	drawSpotlight(GL_LIGHT1, lightMaterial,heliBody, (Transform) { (Vector3f) { 0, -3, 0 }, (Quaternionf) { 1, 0, 0, -45, 0, 0 }, (Scalef) { UNIT, UNIT, UNIT }, (Color3f) { 1, 1, 1 } }, 100,30.0f, axesEnabled);
	instantiateMainRotor(heliBody, (Vector3f) { pos.x, pos.y + 5, pos.z + 0 }, rotorRotation);
	instantiateWindSkid(heliBody, (Vector3f) { 0, 0, 0 });
	instantiateTailBoom(heliBody, (Vector3f) { 0, 0, 0 }, rotorRotation);
}
/// <summary>
/// this method is for helicopter instance to create animated main rotor.
/// </summary>
/// <param name="parent"></param>
/// <param name="pos"></param>
/// <param name="rot"></param>
void instantiateMainRotor(Transform parent, Vector3f pos, GLfloat rotorRotation) {
	//create the transformm object positions and apply. the following OBJ and Freeglut objects will be created.
	Transform bladeDown = setTransform((Vector3f) { 0, 3, 0 }, (Quaternionf) { 0, 1, 0, 0, (0 + rotorRotation), 0 }, (Scalef) { 0.25f, 0.05f, 5 },(Color3f){0,0,0});
	getTransformCylinder(parent, bladeDown, diffuseMaterial, 1, 1, 2, 5, 5,axesEnabled,cylinderObj);
	Transform bladeTop = setTransform((Vector3f) { 0, 3, 0 }, (Quaternionf) { 0, 1, 0, 0, (180 + rotorRotation), 0 }, (Scalef) { 0.25f, 0.05f, 5 }, (Color3f) { 0, 0, 0 });
	getTransformCylinder(parent, bladeTop, diffuseMaterial, 1, 1, 2, 5, 5, axesEnabled, cylinderObj);
	Transform bladeRight = setTransform((Vector3f) { 0, 3, 0 }, (Quaternionf) { 0, 1, 0, 0, (90 + rotorRotation), 0 }, (Scalef) { 0.25f, 0.05f, 5 }, (Color3f) { 0, 0, 0 });
	getTransformCylinder(parent, bladeRight, diffuseMaterial, 1, 1, 2, 5, 5, axesEnabled, cylinderObj);
	Transform bladeLeft = setTransform((Vector3f) { 0, 3, 0 }, (Quaternionf) { 0, 1, 0, 0, (-90 + rotorRotation), 0 }, (Scalef) { 0.25f, 0.05f, 5 }, (Color3f) { 0, 0, 0 });
	getTransformCylinder(parent, bladeLeft, diffuseMaterial, 1, 1, 2, 5, 5, axesEnabled, cylinderObj);
	Transform bladeOrigin = setTransform((Vector3f) { 0, 3, 0 }, (Quaternionf) { 1, 0, 0, 90, 0, 0 }, (Scalef) { 0.1f, 0.1f, 1.5f }, (Color3f) { 1, 0.4f, 0 });
	getTransformCylinder(parent, bladeOrigin, diffuseMaterial, 1, 1, 2, 5, 5, axesEnabled, cylinderObj);
	Transform bladeHolder = setTransform(((Vector3f) { 0, 2.5f, 0 }), (Quaternionf) { 1, 0, 0, 90, 0, 0 }, (Scalef) { 0.25f, 0.25f, 0.25f }, (Color3f) { 1, 0.4f, 0 });
	getTransformCylinder(parent, bladeHolder, diffuseMaterial, 1, 1, 2, 10, 5, axesEnabled, cylinderObj);
	Transform bladeCap = setTransform(((Vector3f) { 0, 3.1f, 0 }), (Quaternionf) { 0, 0, 0, 0 ,0, 0 }, (Scalef) { 0.25f, 0.25f, 0.25f}, (Color3f) { 1, 0.4f, 0 });
	getTransformSphere(parent, bladeCap, ambientMaterial, 1, 3, 3, axesEnabled, sphereObj);
	Transform mainBody = setTransform((Vector3f) { 0, 0, 0 }, (Quaternionf) { 0, 0, 0, 0, 0, 0 }, (Scalef) { 1.5f, 2, 3.5f }, (Color3f) { 1, 0.4f, 0 });
	getTransformSphere(parent, mainBody, ambientMaterial, 1, 15, 15, axesEnabled, sphereObj);
	Transform tankLeft = setTransform((Vector3f) { 1, 1.5f, -2 }, (Quaternionf) { 0, 0, 0, 0, 0, 0 }, (Scalef) { 0.25f, 0.25f, 0.25f }, (Color3f) { 1, 1, 1 });
	getTransformCylinder(parent, tankLeft, diffuseMaterial, 1, 1, 10,5,5, axesEnabled, cylinderObj);
	Transform tankRight = setTransform((Vector3f) { -1, 1.5f, -2 }, (Quaternionf) { 0, 0, 0, 0, 0, 0 }, (Scalef) { 0.25f, 0.25f, 0.25f }, (Color3f) { 1, 1, 1 });
	getTransformCylinder(parent, tankRight, diffuseMaterial, 1, 1, 10, 5, 5, axesEnabled, cylinderObj);
}
/// <summary>
/// this method is for helicopter instance to create tailboom.
/// </summary>
/// <param name="parent"></param>
/// <param name="pos"></param>
/// <param name="rotorRotation"></param>
void instantiateTailBoom(Transform parent, Vector3f pos, GLfloat rotorRotation) {
	//create the transformm object positions and apply. the following OBJ and Freeglut objects will be created.
	Transform heliTail = setTransform((Vector3f) { pos.x + 0, pos.y + 1, -11 }, (Quaternionf) { 0, 1, 0, (0), 0, 0 }, (Scalef) { 0.15f, 0.25f, 5.5f }, (Color3f) { 1, 0.6f, 0 });
	getTransformCylinder(parent, heliTail, diffuseMaterial, 1, 1, 2, 5, 5, axesEnabled, cylinderObj);
	Transform heliTailTop = setTransform((Vector3f) { 0, 1, -11 }, (Quaternionf) { 1, 0, 0, -90, 0, 0 }, (Scalef) { 0.05f, 0.5f, 1.0f }, (Color3f) { 1, 0.6f, 0 });
	getTransformCylinder(parent, heliTailTop, diffuseMaterial, 1, 1, 2, 5, 5, axesEnabled, cylinderObj);
	Transform heliTailDown = setTransform((Vector3f) { 0, 1, -11 }, (Quaternionf) { 1, 0, 0, 135.0f, 0, 0 }, (Scalef) { 0.05f, 0.25f, 0.5f }, (Color3f) { 1, 0.6f, 0 });
	getTransformCylinder(parent, heliTailDown, diffuseMaterial, 1, 1, 2, 5, 5, axesEnabled, cylinderObj);
	Transform heliRudder = setTransform((Vector3f) { 0.25f, 1.25f, -10 }, (Quaternionf) { 0, 0, 0, 0, 0, 0 }, (Scalef) { 0.2f, 0.2f, 0.2f }, (Color3f) { 1, 0.6f, 0 });
	getTransformSphere(parent, heliRudder, ambientMaterial, 1, 5, 5, axesEnabled, sphereObj);
	Transform heliBladeTop = setTransform((Vector3f) { 0.25f, 1.25f,  -10 }, (Quaternionf) { 1, 1,1, rotorRotation, 0, 90 }, (Scalef) { 0.2f, 0.05f, 0.5f }, (Color3f) { 0, 0, 0 });
	getTransformCylinder(parent, heliBladeTop, diffuseMaterial, 1, 1, 2, 5, 5, axesEnabled, cylinderObj);
	Transform heliBladeDown = setTransform((Vector3f) { 0.25f, 1.25f,  -10 }, (Quaternionf) { 1, 1,1, rotorRotation, 180, 90 }, (Scalef) { 0.2f, 0.05f, 0.5f }, (Color3f) { 0, 0, 0 });
	getTransformCylinder(parent, heliBladeDown, diffuseMaterial, 1, 1, 2, 5, 5, axesEnabled, cylinderObj);
}
/// <summary>
/// this method is for helicopter instance to create windskid.
/// </summary>
/// <param name="parent"></param>
/// <param name="pos"></param>
void instantiateWindSkid(Transform parent, Vector3f pos) {
	//create the transformm object positions and apply. the following OBJ and Freeglut objects will be created.
	Transform topPillar = setTransform((Vector3f) { pos.x + 1.0f, pos.y - 1.0f, pos.z + 1.0f }, (Quaternionf) { 1, 0, 0, 90, 0, 0 }, (Scalef) { 0.10f, 0.15f, 0.15f }, (Color3f) { 0, 0, 0 });
	getTransformCylinder(parent, topPillar, ambientMaterial, 1, 1, 8, 5, 5, axesEnabled, cylinderObj);
	Transform topPillarRight = setTransform((Vector3f) { pos.x - 1.0f, pos.y - 1.0f, pos.z + 1.0f }, (Quaternionf) { 1, 0, 0, 90, 0, 0 }, (Scalef) { 0.10f, 0.15f, 0.15f }, (Color3f) { 0, 0, 0 });
	getTransformCylinder(parent, topPillarRight, ambientMaterial, 1, 1, 8, 5, 5, axesEnabled, cylinderObj);
	Transform downPillarLeft = setTransform((Vector3f) { pos.x + 1.0f, pos.y - 1.0f, pos.z - 1.0f }, (Quaternionf) { 1, 0, 0, 90, 0, 0 }, (Scalef) { 0.10f, 0.15f, 0.15f }, (Color3f) { 0, 0, 0 });
	getTransformCylinder(parent, downPillarLeft, ambientMaterial, 1, 1, 8, 5, 5, axesEnabled, cylinderObj);
	Transform downPillarRight = setTransform((Vector3f) { pos.x - 1.0f, pos.y - 1.0f, pos.z - 1.0f }, (Quaternionf) { 1, 0, 0, 90, 0, 0 }, (Scalef) { 0.10f, 0.15f, 0.15f }, (Color3f) { 0, 0, 0 });
	getTransformCylinder(parent, downPillarRight, ambientMaterial, 1, 1, 8, 5, 5, axesEnabled, cylinderObj);
	Transform skidLeft = setTransform((Vector3f) { pos.x + 1.0f, pos.y - 2.2f, pos.z - 2.2f }, (Quaternionf) { 1, 0, 0, 0, 0, 0 }, (Scalef) { 0.25f, 0.25f, 0.25f }, (Color3f) { 0, 0, 0 });
	getTransformCylinder(parent, skidLeft, ambientMaterial, 1, 1, 16, 5, 5, axesEnabled, cylinderObj);
	Transform skidRight = setTransform((Vector3f) { pos.x - 1.0f, pos.y - 2.2f, pos.z - 2.2f }, (Quaternionf) { 1, 0, 0, 0, 0, 0 }, (Scalef) { 0.25f, 0.25f, 0.25f }, (Color3f) { 0, 0, 0 });
	getTransformCylinder(parent, skidRight, ambientMaterial, 1, 1, 16, 5, 5, axesEnabled, cylinderObj); 
}
/// <summary>
/// this method draws the ground plane with the animated texture map.
/// </summary>
/// <param name="material"></param>
/// <param name="color"></param>
/// <param name="position"></param>
/// <param name="numQuads"></param>
/// <param name="textureID"></param>
/// <param name="rotationSpeed"></param>
void drawGround(Material material,Color3f color, Vector3f position, int numQuads, GLuint textureID, GLfloat rotationSpeed) {
	const float unitSize = UNIT * 10.0f; // Unit size in meters
	const float textureScale = 1.0f; // Scale factor for texture coordinates
	float startX = position.x - (numQuads / 2) * unitSize;
	float startZ = position.z - (numQuads / 2) * unitSize;
	glPushMatrix();
	glTranslatef(position.x, position.y, position.z);
	renderFillEnabled ? glPolygonMode(GL_FRONT_AND_BACK, GL_FILL) : glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glEnable(GL_TEXTURE_2D); // Enable texturing

	// Loop through the quad planes and draw them
	for (int i = 0; i < numQuads; i++) {
		for (int j = 0; j < numQuads; j++) {
			// Calculate the position and size of the current quad plane
			float quadX = startX + i * unitSize;
			float quadZ = startZ + j * unitSize;
			float quadSize = unitSize;

			GLfloat ambientMaterial[4] = { material.ambient.r, material.ambient.g, material.ambient.b, material.ambient.a };
			GLfloat diffuseMaterial[4] = { material.diffuse.r, material.diffuse.g, material.diffuse.b, material.diffuse.a };
			GLfloat specularMaterial[4] = { material.specular.r, material.specular.g, material.specular.b, material.specular.a };

			glMaterialfv(GL_FRONT, GL_AMBIENT, ambientMaterial);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseMaterial);
			glMaterialfv(GL_FRONT, GL_SPECULAR, specularMaterial);
			glMaterialf(GL_FRONT, GL_SHININESS, material.shininess);
			glEnable(GL_COLOR_MATERIAL);

			// Set the color to white (for texture)
			glColor3f(color.r, color.g, color.b);

			// Calculate the texture coordinates
			float texCoordX1 = (i + rotationSpeed / 360.0f) * textureScale;
			float texCoordX2 = texCoordX1 + textureScale;
			float texCoordY1 = (j + rotationSpeed / 360.0f) * textureScale;
			float texCoordY2 = texCoordY1 + textureScale;

			// Bind the texture for the current quad
			glBindTexture(GL_TEXTURE_2D, textureID);

			// Draw the quad plane with texture coordinates
			glBegin(GL_QUADS);
			glNormal3f(0.0f, 1.0f, 0.0f);
			glTexCoord2f(texCoordX1, texCoordY1);
			glVertex3f(quadX, position.y, quadZ);
			glTexCoord2f(texCoordX2, texCoordY1);
			glVertex3f(quadX + quadSize, position.y, quadZ);
			glTexCoord2f(texCoordX2, texCoordY2);
			glVertex3f(quadX + quadSize, position.y, quadZ + quadSize);
			glTexCoord2f(texCoordX1, texCoordY2);
			glVertex3f(quadX, position.y, quadZ + quadSize);
			glEnd();
		}
	}

	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_TEXTURE_2D); // Disable texturing

	glPopMatrix();
}
/******************************************************************************/