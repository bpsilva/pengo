/**
Implementa a movimenta��o simples de uma camera em primeira pessoa para um personagem que anda
sobre um plano.
*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cmath>
#include <iostream>
#include <gl/glut.h>

//openal (sound lib)
#include <al/alut.h>

//bitmap class to load bitmaps for textures
#include "bitmap.h"
#include "Rock.h"

#pragma comment(lib, "OpenAL32.lib")
#pragma comment(lib, "alut.lib")

//#include <mmsystem.h>
//#pragma comment(lib, "winmm.lib")

#define PI 3.14159265

// sound stuff
#define NUM_BUFFERS 1
#define NUM_SOURCES 1
#define NUM_ENVIRONMENTS 1

void mainInit();
void initSound();
void initTexture();
void createGLUI();
void renderScene();
void mainCreateMenu();
void onMouseButton(int button, int state, int x, int y);
void onMouseMove(int x, int y);
void onKeyDown(unsigned char key, int x, int y);
void onKeyUp(unsigned char key, int x, int y);
void onGLUIEvent(int id);
void onWindowReshape(int x, int y);
void mainIdle();
int main(int argc, char **argv);
void setWindow();
void setViewport(GLint left, GLint right, GLint bottom, GLint top);
void updateState();
void renderFloor();
void updateCam();

/**
Screen dimensions
*/
int windowWidth = 600;
int windowHeight = 480;

/**
Screen position
*/
int windowXPos = 100;
int windowYPos = 150;

int mainWindowId = 0;

double xOffset = -1.9;
double yOffset = -1.3;
int mouseLastX = 0;
int mouseLastY = 0;

float roty = 0.0f;
float rotx = 90.0f;

bool rightPressed = false;
bool leftPressed = false;
bool upPressed = false;
bool downPressed = false;

bool spacePressed = false;

float speedX = 0.0f;
float speedY = 0.0f;
float speedZ = 0.0f;

float posX = 0.0f;
float posY = 0.4f;
float posZ = 15.0f;

/*
variavel auxiliar pra dar varia��o na altura do ponto de vista ao andar.
*/
float headPosAux = 0.0f;

float maxSpeed = 0.25f;

float planeSize = 4.0f;

// more sound stuff (position, speed and orientation of the listener)
ALfloat listenerPos[]={0.0,0.0,4.0};
ALfloat listenerVel[]={0.0,0.0,0.0};
ALfloat listenerOri[]={0.0,0.0,1.0,
						0.0,1.0,0.0};

// now the position and speed of the sound source
ALfloat source0Pos[]={ -2.0, 0.0, 0.0};
ALfloat source0Vel[]={ 0.0, 0.0, 0.0};

// buffers for openal stuff
ALuint  buffer[NUM_BUFFERS];
ALuint  source[NUM_SOURCES];
ALuint  environment[NUM_ENVIRONMENTS];
ALsizei size,freq;
ALenum  format;
ALvoid  *data;



// parte de c�digo extra�do de "texture.c" por Michael Sweet (OpenGL SuperBible)
// texture buffers and stuff
int i;                       /* Looping var */
BITMAPINFO	*info;           /* Bitmap information */
GLubyte	    *bits;           /* Bitmap RGB pixels */
GLubyte     *ptr;            /* Pointer into bit buffer */
GLubyte	    *rgba;           /* RGBA pixel buffer */
GLubyte	    *rgbaptr;        /* Pointer into RGBA buffer */
GLubyte     temp;            /* Swapping variable */
GLenum      type;            /* Texture type */
GLuint      texture;         /* Texture object */


Rock *rock;

void setWindow() {
	//roty = 0.0f;
    //rotx = 90.0f;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,(GLfloat)windowWidth/(GLfloat)windowHeight,0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(posX,posY + 0.025 * std::abs(sin(headPosAux*PI/180)),posZ,
		posX + sin(roty*PI/180),posY + 0.025 * std::abs(sin(headPosAux*PI/180)) + cos(rotx*PI/180),posZ -cos(roty*PI/180),
		0.0,1.0,0.0);
}

/**
Atualiza a posi��o e orienta��o da camera
*/
void updateCam() {
	gluLookAt(posX,posY + 0.025 * std::abs(sin(headPosAux*PI/180)),posZ,
		posX + sin(roty*PI/180),posY + 0.025 * std::abs(sin(headPosAux*PI/180)) + cos(rotx*PI/180),posZ -cos(roty*PI/180),
		0.0,1.0,0.0);

	listenerPos[0] = posX;
	listenerPos[1] = posY;
	listenerPos[2] = posZ;

	source0Pos[0] = posX;
	source0Pos[1] = posY;
	source0Pos[2] = posZ;
}

void setViewport(GLint left, GLint right, GLint bottom, GLint top) {
	glViewport(left, bottom, right - left, top - bottom);
}


/**
Initialize
*/
void mainInit() {
	glClearColor(1.0,1.0,1.0,0.0);
	glColor3f(0.0f,0.0f,0.0f);
	setWindow();
	setViewport(0, windowWidth, 0, windowHeight);

	initSound();

	initTexture();


	printf("w - andar \n");
	printf("a - esquerda \n");
	printf("d - direita \n");

}

/**
 * Initialize openal and check for errors
 *
 * There is a class named Sounds that should handle sounds.
 */
void initSound() {

	printf("Initializing OpenAl \n");

	// This sould call Sounds::init

	printf("Sound ok! \n\n");
}

/**
Initialize the texture
*/
void initTexture(void)
{
	printf ("\nLoading texture..\n");
    // Load a texture object (256x256 true color)
    bits = LoadDIBitmap(".\\res\\snow.bmp", &info);
    if (bits == (GLubyte *)0) {
		printf ("Error loading texture!\n\n");
		return;
	}


    // Figure out the type of texture
    if (info->bmiHeader.biHeight == 1)
      type = GL_TEXTURE_1D;
    else
      type = GL_TEXTURE_2D;

    // Create and bind a texture object
    glGenTextures(1, &texture);
	glBindTexture(type, texture);

    // Set texture parameters
	glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Create an RGBA image
    rgba = (GLubyte *)malloc(info->bmiHeader.biWidth * info->bmiHeader.biHeight * 4);

    i = info->bmiHeader.biWidth * info->bmiHeader.biHeight;
    for( rgbaptr = rgba, ptr = bits;  i > 0; i--, rgbaptr += 4, ptr += 3)
    {
            rgbaptr[0] = ptr[2];     // windows BMP = BGR
            rgbaptr[1] = ptr[1];
            rgbaptr[2] = ptr[0];
            rgbaptr[3] = (ptr[0] + ptr[1] + ptr[2]) / 3;
    }

    glTexImage2D(type, 0, 4, info->bmiHeader.biWidth, info->bmiHeader.biHeight,
                  0, GL_RGBA, GL_UNSIGNED_BYTE, rgba );

    printf("Textura %d\n", texture);
	printf("Textures ok.\n\n", texture);
}


void renderFloor()
{
	glColor4f(0.8f,0.8f,0.8f,1.0f);
	glBegin(GL_LINES);
	for (int i = 0; i <= 10; i++) {
		glVertex3f(-planeSize, 0.0f, -planeSize + i*(2*planeSize)/10.0f);
		glVertex3f(planeSize, 0.0f, -planeSize + i*(2*planeSize)/10.0f);
	}
	for (int i = 0; i <= 10; i++) {
		glVertex3f(-planeSize + i*(2*planeSize)/10.0f, 0.0f, -planeSize);
		glVertex3f(-planeSize + i*(2*planeSize)/10.0f, 0.0f, planeSize);
	}
	glEnd();


}

void renderFloor_old() {

	// i want some nice, smooth, antialiased lines
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);

	// set things up to render the floor with the texture
	glShadeModel(GL_FLAT);
	glEnable (type);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor4f(0.4f,0.4f,0.4f,1.0f);

	glPushMatrix();

	float textureScaleX = 10.0;
	float textureScaleY = 10.0;

	glBegin(GL_QUADS);
		glTexCoord2f(textureScaleX, 0.0f);   // coords for the texture
		glVertex3f(-planeSize, 0.0f, planeSize);

		glTexCoord2f(0.0f, 0.0f);  // coords for the texture
		glVertex3f(planeSize, 0.0f, planeSize);

		glTexCoord2f(0.0f, textureScaleY);  // coords for the texture
		glVertex3f(planeSize, 0.0f, -planeSize);

		glTexCoord2f(textureScaleX, textureScaleY);  // coords for the texture
		glVertex3f(-planeSize, 0.0f, -planeSize);
	glEnd();

	glDisable(type);

	glColor4f(0.8f,0.8f,0.8f,1.0f);
	glBegin(GL_LINES);
	for (int i = 0; i <= 10; i++) {
		glVertex3f(-planeSize, 0.0f, -planeSize + i*(2*planeSize)/10.0f);
		glVertex3f(planeSize, 0.0f, -planeSize + i*(2*planeSize)/10.0f);
	}
	for (int i = 0; i <= 10; i++) {
		glVertex3f(-planeSize + i*(2*planeSize)/10.0f, 0.0f, -planeSize);
		glVertex3f(-planeSize + i*(2*planeSize)/10.0f, 0.0f, planeSize);
	}
	glEnd();

	glPopMatrix();
}


void updateState() {

	if (leftPressed) {
		roty -= 4.0f;
	}

	if (rightPressed) {
		roty += 4.0f;
	}

	if (upPressed) {

		speedX = 0.025 * sin(roty*PI/180);
		speedZ = -0.025 * cos(roty*PI/180);

		// efeito de "sobe e desce" ao andar
		headPosAux += 7.0f;
		if (headPosAux > 180.0f) {
			headPosAux = 0.0f;
		}

		posX += speedX;
		posZ += speedZ;

	} else {
		// parou de andar, para com o efeito de "sobe e desce"
		headPosAux = fmod(headPosAux, 90) - 1 * headPosAux / 90;
		headPosAux -= 4.0f;
		if (headPosAux < 0.0f) {
			headPosAux = 0.0f;
		}
	}


}

/**
Render scene
*/
void renderScene() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor4f(0.4f,0.4f,0.4f,1.0f);

	updateState();

	/*
	 * Anything that needs to be rendered have to be inside this
	 */
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	updateCam();
	renderFloor();
	rock->Draw();

	/*
	 * End Rendering
	 */


	glFlush();
	glutPostRedisplay();
	Sleep(30);
}

/**
Handles events from the mouse right button menu
*/
void mainHandleMouseRightButtonMenuEvent(int option) {
	switch (option) {
		case 1 :
			exit(0);
			break;
		default:
			break;
	}
}

/**
Create mouse button menu
*/
void mainCreateMenu() {
	glutCreateMenu(mainHandleMouseRightButtonMenuEvent);
	glutAddMenuEntry("Quit", 1);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

/**
Mouse button event handler
*/
void onMouseButton(int button, int state, int x, int y) {
	//printf("onMouseButton button: %d \n", button);
	glutPostRedisplay();
}

/**
Mouse move while button pressed event handler
*/
void onMouseMove(int x, int y) {

	mouseLastX = x;
	mouseLastY = y;

	glutPostRedisplay();
}

/**
Mouse move with no button pressed event handler
*/
void onMousePassiveMove(int x, int y) {


	mouseLastX = x;
	mouseLastY = y;
}

/**
Key press event handler
*/
void onKeyDown(unsigned char key, int x, int y) {
	switch (key) {
		case 32: //space
			spacePressed = true;
			break;
		case 119: //w
			if (!upPressed) {
				alSourcePlay(source[0]);
				//PlaySound((LPCSTR) "..\\..\\Footsteps.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
			}
			upPressed = true;
			break;
		case 115: //s
			downPressed = true;
			break;
		case 97: //a
			leftPressed = true;
			break;
		case 100: //d
			rightPressed = true;
			break;
		default:
			break;
	}

	//glutPostRedisplay();
}

/**
Key release event handler
*/
void onKeyUp(unsigned char key, int x, int y) {
	switch (key) {
		case 32: //space
			spacePressed = false;
			break;
		case 119: //w
			if (upPressed) {
				alSourceStop(source[0]);
				//PlaySound(NULL, 0, 0);
			}
			upPressed = false;
			break;
		case 115: //s
			downPressed = false;
			break;
		case 97: //a
			leftPressed = false;
			break;
		case 100: //d
			rightPressed = false;
			break;
		case 27:
			exit(0);
			break;
		default:
			break;
	}

	//glutPostRedisplay();
}

void onWindowReshape(int x, int y) {
	windowWidth = x;
	windowHeight = y;
	setWindow();
	setViewport(0, windowWidth, 0, windowHeight);
}

/**
Glut idle funtion
*/
void mainIdle() {
	/**
	Set the active window before send an glutPostRedisplay call
	so it wont be accidently sent to the glui window
	*/
	glutSetWindow(mainWindowId);
	glutPostRedisplay();
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(windowWidth,windowHeight);
	glutInitWindowPosition(windowXPos,windowYPos);

	/**
	Store main window id so that glui can send it redisplay events
	*/
	mainWindowId = glutCreateWindow("FPS");

	glutDisplayFunc(renderScene);

	glutReshapeFunc(onWindowReshape);

	/**
	Register mouse events handlers
	*/
	glutMouseFunc(onMouseButton);
	glutMotionFunc(onMouseMove);
	glutPassiveMotionFunc(onMousePassiveMove);

	/**
	Register keyboard events handlers
	*/
	glutKeyboardFunc(onKeyDown);
	glutKeyboardUpFunc(onKeyUp);

	rock = new Rock();
	if(rock->LoadObject() == false)
    {
        printf("Couldn't load the rock!");
    }

	mainInit();

	/**
	Create GLUT mouse button menus
	*/
	//mainCreateMenu();

	glutMainLoop();

    return 0;
}
