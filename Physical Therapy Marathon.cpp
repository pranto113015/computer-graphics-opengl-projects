// Author
// Name: Pranto Kumar
 

#include <windows.h>
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>

#include "corpoHumano.h"
#include "cenario.h"
#include "textura.h"

/* Window size */
#define WINDOW_WIDTH 700
#define WINDOW_HEIGHT 700

#define MAX_ZOOM_OUT 180
#define MAX_ZOOM_IN 30
#define PI 3.141592

/* Camera parameters */
float cameraX, cameraY, cameraZ;
float aspectRatio       = 0.0;
float fov               = 45.0;
float cameraRadius      = 130.0;
float theta             = 0.35;
float alpha             = 0.0;

/* Menu options and animation */
int optUser = -1;
int optAnimation = -1;
int resetFlag = 0;
int animationFlag = 0;

/* Definition of light parameters */
float lightAmbient[]  = {0.2, 0.2, 0.2, 1.0};
float lightDiffuse[]  = {0.3, 0.3, 0.3, 1.0};
float lightSpecular[] = {0.3, 0.3, 0.3, 1.0};

/* Definition of rotation axes to be modified by the user */
typedef struct {

    float *axis[3];
    float orientation;
    char keyPressed;

} Rotation;

Rotation rotate;

/* Current joint of the human body */
Animation *currentJoint;

/* Repositions the light source */
void updateLightningPosition() {

    float light0_position[] = {cameraX, cameraY, cameraZ, 1.0};
    float light1_position[] = {-cameraX, -cameraY, -cameraZ, 1.0};

    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

    glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, lightSpecular);
    glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
}

void initLightning() {

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lightAmbient);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    updateLightningPosition();
}

void init() {

    glClearColor(0.0, 0.0, 0.0, 1.0);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_COLOR_MATERIAL);
    glShadeModel(GL_SMOOTH);

    initBodyQuadrics();
    initScenarioQuadrics();
    initLightning();

    loadTexture("floor.bmp", 0);
}

void updateCamera() {

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(fov, aspectRatio, 1, 1000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    cameraX = sin(alpha) * cos(theta) * cameraRadius;
    cameraY = sin(theta) * cameraRadius;
    cameraZ = cos(alpha) * cos(theta) * cameraRadius;

    gluLookAt(cameraX, cameraY, cameraZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    updateLightningPosition();
}

void reshape(int w, int h) {

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    aspectRatio = (float) w / (float) h;

    if (w >= h) glOrtho(-15.0 * aspectRatio, 15.0 * aspectRatio, -15.0, 15.0, -15.0, 15.0);
    else glOrtho(-15.0, 15.0, -15.0 / aspectRatio, 15.0 / aspectRatio, -15.0, 15.0);

    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	updateCamera();
}

void display() {

    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-15, 15, -15, 15, -15, 15);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    updateCamera();

    drawHuman();
    drawScenario(alpha, theta);

    glutSwapBuffers();
}

void changeBodyJoint(int id) {

    switch (id) {

/* Peach Junta */
        case 0:
            currentJoint = &neckJoint;
            break;

   /* Left shoulder joint */
        case 1:
            currentJoint = &leftShoulderJoint;
            break;

/* Right shoulder joint */
        case 2:
            currentJoint = &rightShoulderJoint;
            break;

      /* Left elbow joint */
        case 3:
            currentJoint = &leftElbowJoint;
            break;

/* Right elbow joint */
        case 4:
            currentJoint = &rightElbowJoint;
            break;

/* Left hip joint */
        case 5:
            currentJoint = &leftHipJoint;
            break;

  /* Right hip joint */
        case 6:
            currentJoint = &rightHipJoint;
            break;

/* Left knee joint */
        case 7:
            currentJoint = &leftKneeJoint;
            break;

/* Right knee joint */
        case 8:
            currentJoint = &rightKneeJoint;
            break;
    }
}

/* Changes the rotation axes of a given joint of the human body */
void changeJointRotation() {

    rotate.axis[0] = &currentJoint->rotation[0];
    rotate.axis[1] = &currentJoint->rotation[1];
    rotate.axis[2] = &currentJoint->rotation[2];
}

void resetMenu(int id) {

    resetFlag = id;
}

void userMenu(int id) {

    optUser = id;
    changeBodyJoint(id);
    changeJointRotation();
}

void animationMenu(int id) {

    animationFlag = 1;
    optAnimation = id;
}

/* Checks whether the joints of the human body exceed the specified limits of the animation */
int checkJointRotation() {

    char keys[] = {"xyz"};

    int i;
    for (i = 0; i < AXIS; i++) {

        if (rotate.keyPressed == keys[i]) {

            float offSet = currentJoint->rotation[i] + rotate.orientation;
            float minRot = currentJoint->minRotation[i];
            float maxRot = currentJoint->maxRotation[i];

            return (-minRot <= offSet) && (offSet <= maxRot);
        }
    }
}

void keyboard(unsigned char key, int x, int y) {

    switch (key) {

    /* ESC KEY */
        case 27:
            exit(0);
            break;

        case 'x':
        case 'X':
        case 'y':
        case 'Y':
        case 'z':
        case 'Z':

            if (optUser < 0 || (animationFlag || resetFlag || checkIfHasEquipment())) return;

            if (key == 'x' || key == 'X') rotate.keyPressed = 'x';
            else if (key == 'y' || key == 'Y') rotate.keyPressed = 'y';
            else rotate.keyPressed = 'z';

            if (key == 'x' || key == 'y' || key == 'z') {

                rotate.orientation = 2.0;

                if (checkJointRotation()) {

                    if (rotate.keyPressed == 'x') *rotate.axis[0] += 2.0;
                    else if (rotate.keyPressed == 'y') *rotate.axis[1] += 2.0;
                    else if (rotate.keyPressed == 'z') *rotate.axis[2] += 2.0;
                }
            }
            else {

                rotate.orientation = -2.0;

                if (checkJointRotation()) {

                    if (rotate.keyPressed == 'x') *rotate.axis[0] -= 2.0;
                    else if (rotate.keyPressed == 'y') *rotate.axis[1] -= 2.0;
                    else if (rotate.keyPressed == 'z') *rotate.axis[2] -= 2.0;
                }
            }
            break;

        case '+':

            if (cameraRadius > MAX_ZOOM_IN) cameraRadius -= 2.0;
            break;

        case '-':

            if (cameraRadius < MAX_ZOOM_OUT) cameraRadius += 2.0;
            break;
    }
    glutPostRedisplay();
}

void specialKeyboard(int key, int x, int y) {

    float newTheta = theta;

    switch (key) {

        case GLUT_KEY_LEFT:

            alpha -= 0.05;
            break;

        case GLUT_KEY_RIGHT:

            alpha += 0.05;
            break;

        case GLUT_KEY_UP:

            if (newTheta + 0.05 < 1.5) theta += 0.05;
            break;

        case GLUT_KEY_DOWN:

            if (newTheta - 0.05 > 0) theta -= 0.05;
            break;
    }

  /* Limit camera angles between 0 and 2PI */
    if (alpha > 2 * PI) alpha = alpha - 2 * PI;
    else if(alpha < 0.0) alpha = 2 * PI - alpha;

    glutPostRedisplay();
}

/* Checks if the rotation axis of the human body joints are in the initial position */
int checkInitialJointRotation() {

    float minRot = -0.1;
    float maxRot = 0.1;

    return (((*rotate.axis[0] - minRot) * (*rotate.axis[0]- maxRot) <= 0) &&
            ((*rotate.axis[1] - minRot) * (*rotate.axis[1] - maxRot) <= 0) &&
            ((*rotate.axis[2] - minRot) * (*rotate.axis[2] - maxRot) <= 0));
}

/* Resets all angles to their initial position */
int resetJointsAngle() {

    int numResetedJoints = 0;

    int i, j;
    for (i = 0; i < NUM_JOINTS; i++) {

        changeBodyJoint(i);
        changeJointRotation();

        if (checkInitialJointRotation()) {
            *rotate.axis[0] = *rotate.axis[1] = *rotate.axis[2] = 0.0;
            numResetedJoints += 1;
            continue;
        }

        for (j = 0; j < AXIS; j++) {

            if (*rotate.axis[j] < 0) *rotate.axis[j] += STEP;
            else if (*rotate.axis[j] > 0) *rotate.axis[j] -= STEP;
        }
    }

    if (numResetedJoints == NUM_JOINTS) {
        resetFlag = 0;
        optUser = -1;
    }
}

void idleFunc() {

    if ((resetFlag && !checkIfHasEquipment()) && (optAnimation < 0 || (checkIfAnimationEnded() && optUser >= 0))) resetJointsAngle();

    if (animationFlag) {

        if (checkIfAnimationEnded() && optUser >= 0) resetFlag = 1;
        else {

            updateAnimation(optAnimation, resetFlag, optUser);
            kinematics(optAnimation, resetFlag, optUser);

            if (checkIfAnimationEnded()) {
                animationFlag = 0;
                optAnimation = -1;
            }
        }
    }
    glutPostRedisplay();
}

int main(int argc, char *argv[]) {

    int subUserMenu, aquecimento, halteres, reset;

    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Maratona da Fisioterapia - Projeto Final - Computacao Grafica");

    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeyboard);
    glutReshapeFunc(reshape);
    glutIdleFunc(idleFunc);

    subUserMenu = glutCreateMenu(userMenu);



    aquecimento = glutCreateMenu(animationMenu);

        glutAddMenuEntry(" Quadriceps Stretch ", 0);
        glutAddMenuEntry(" Pescoco Alongamento ", 1);
        glutAddMenuEntry(" Flexao Lateral Tronco ", 2);
        glutAddMenuEntry(" Squat ", 3);
        glutAddMenuEntry(" Polichinelo ", 4);
        glutAddMenuEntry(" All ", 5);

    halteres = glutCreateMenu(animationMenu);

        glutAddMenuEntry(" Get Equipment ", 6);
        glutAddMenuEntry(" Alternating Thread ", 7);
        glutAddMenuEntry(" Simultaneous Thread ", 8);
        glutAddMenuEntry(" Front Lift ", 9);
        glutAddMenuEntry(" Side Lift", 10);
        glutAddMenuEntry(" I sink ", 11);
        glutAddMenuEntry(" All ", 12);
        glutAddMenuEntry(" Return Equipment ", 13);

    reset = glutCreateMenu(resetMenu);

    glutAddMenuEntry(" Reset ", 1);
    glutAddSubMenu(" Exercises - Warming up ", aquecimento);
    glutAddSubMenu(" Exercises - Dumbbells ", halteres);

	glutAttachMenu(GLUT_RIGHT_BUTTON);

    glutMainLoop();
    return 0;
 }
