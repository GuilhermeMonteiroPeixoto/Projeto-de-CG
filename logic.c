#include <math.h>
#include <stdlib.h>
#include <GL/glut.h>
#include "logic.h"
#include "render.h"

#define M_PI 3.14159265358979323846

int winW = 1280;
int winH = 720;
float fovDeg = 60.0f;
float camDist = 140.0f;
float camYaw = 30.0f;
float camPitch = 25.0f;
float ballX = 0.0f;
float ballZ = 0.0f;
int scoreBlue = 0;
int scoreRed = 0;

static int key_left = 0;
static int key_right = 0;
static int key_up = 0;
static int key_down = 0;
static int lastTimeMs = 0;

Player players[NUM_PLAYERS] = {
    {20.0f, 0.0f, 20.0f, {0.10f,0.30f,0.95f}},
    {20.0f, 30.0f, 80.0f, {0.90f,0.10f,0.10f}},
    {0.0f, 25.0f, 0.0f, {0.10f,0.30f,0.95f}},
    {-15.0f, -10.0f, 180.0f, {0.90f,0.10f,0.10f}},
    {-25.0f, 15.0f, -30.0f, {0.10f,0.30f,0.95f}},
    {10.0f, -25.0f, 150.0f, {0.90f,0.10f,0.10f}},
};

void applyCamera(void){
    const float yawR = camYaw*(float)M_PI/180.0f;
    const float pitchR = camPitch*(float)M_PI/180.0f;
    const float cx = camDist*cosf(pitchR)*sinf(yawR);
    const float cy = camDist*sinf(pitchR);
    const float cz = camDist*cosf(pitchR)*cosf(yawR);
    gluLookAt(cx, cy, cz,  ballX, 0.0f, ballZ,  0.0f, 1.0f, 0.0f);
}

static void updateBall(float dt) {
    float dx = (float)(key_right - key_left);
    float dz = (float)(key_down - key_up);
    float len = sqrtf(dx * dx + dz * dz);

    if (len > 0) {
        dx /= len;
        dz /= len;
        ballX += dx * 15.0f * dt;
        ballZ += dz * 15.0f * dt;
    }

    float halfW = 68.0f * 0.5f - 0.25f;
    float halfL = 105.0f * 0.5f - 0.25f;
    int dentroX = (fabsf(ballX) <= 7.32f * 0.5f);

    if (dentroX && ballZ >= (105.0f * 0.5f - 0.25f)) {
        scoreBlue += 1;
        resetKickoff();
        return;
    }

    if (dentroX && ballZ <= -(105.0f * 0.5f - 0.25f)) {
        scoreRed += 1;
        resetKickoff();
        return;
    }

    if (ballX < -halfW) {
        ballX = -halfW;
    }

    if (ballX > halfW) {
        ballX = halfW;
    }

    if (ballZ < -halfL) {
        ballZ = -halfL;
    }

    if (ballZ > halfL) {
        ballZ = halfL;
    }
}

static void updatePlayers(float dt) {
    float halfL = 105.0f * 0.5f - 0.5f;
    float v;

    if (ballZ >= 0.0f) {
        v = 0.8f;
    } else {
        v = -0.8f;
    }

    for (int i = 0; i < NUM_PLAYERS; i++) {
        players[i].z += v * dt;

        if (players[i].z > halfL) {
            players[i].z = halfL;
        }

        if (players[i].z < -halfL) {
            players[i].z = -halfL;
        }
    }
}

void resetKickoff(void){
    camYaw = 30.0f;
    camPitch = 25.0f;
    camDist = 140.0f;
    fovDeg = 60.0f;
    updateProjection();
    ballX = 0.0f;
    ballZ = 0.0f;
    key_left = key_right = key_up = key_down = 0;

    players[0].x=+20.0f;
    players[0].z = 0.0f;
    players[0].yawDeg = 20.0f;
    players[1].x=+20.0f;
    players[1].z = 30.0f;
    players[1].yawDeg = 80.0f;
}

void initLogic(void){
    lastTimeMs = glutGet(GLUT_ELAPSED_TIME);
    resetKickoff();
}

void idle(void){
    int now = glutGet(GLUT_ELAPSED_TIME);
    float dt = (now - lastTimeMs)*0.001f;
    
    if (dt < 0){
    dt = 0;
				}

				if (dt > 0.05f){
								dt = 0.05f;
				}

    lastTimeMs = now;
    updateBall(dt);
    updatePlayers(dt);
    glutPostRedisplay();
}

void special(int key, int x, int y) {
    (void)x;
    (void)y;

    if (key == GLUT_KEY_LEFT) {
        key_left = 1;
    }

    if (key == GLUT_KEY_RIGHT) {
        key_right = 1;
    }

    if (key == GLUT_KEY_UP) {
        key_up = 1;
    }

    if (key == GLUT_KEY_DOWN) {
        key_down = 1;
    }

    if (key == GLUT_KEY_PAGE_UP) {
        camDist -= 10.0f;

        if (camDist < 40.0f) {
            camDist = 40.0f;
        }
    }

    if (key == GLUT_KEY_PAGE_DOWN) {
        camDist += 10.0f;

        if (camDist > 300.0f) {
            camDist = 300.0f;
        }
    }
}

void specialUp(int key, int x, int y) {
    (void)x;
    (void)y;

    if (key == GLUT_KEY_LEFT) {
        key_left = 0;
    }

    if (key == GLUT_KEY_RIGHT) {
        key_right = 0;
    }

    if (key == GLUT_KEY_UP) {
        key_up = 0;
    }

    if (key == GLUT_KEY_DOWN) {
        key_down = 0;
    }
}

void keyboard(unsigned char key, int x, int y) {
    (void)x;
    (void)y;

    if (key == '+') {
        fovDeg -= 10.0f;

        if (fovDeg < 30.0f) {
            fovDeg = 30.0f;
        }

        updateProjection();
    }

    if (key == '-') {
        fovDeg += 10.0f;

        if (fovDeg > 100.0f) {
            fovDeg = 100.0f;
        }

        updateProjection();
    }

    if (key == 'j' || key == 'J') {
        camYaw -= 10.0f;
    }

    if (key == 'l' || key == 'L') {
        camYaw += 10.0f;
    }

    if (key == 'i' || key == 'I') {
        camPitch += 10.0f;

        if (camPitch > 89.0f) {
            camPitch = 89.0f;
        }
    }

    if (key == 'k' || key == 'K') {
        camPitch -= 10.0f;

        if (camPitch < -20.0f) {
            camPitch = -20.0f;
        }
    }

    if (key == 'r' || key == 'R') {
        resetKickoff();
    }

    if (key == 27) {
        extern void stopAudio(void);
        stopAudio();
        exit(0);
    }

    glutPostRedisplay();
}

