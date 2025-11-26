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
    // Converte os ângulos da câmera (yaw, pitch) para radianos
    const float yawR = camYaw*(float)M_PI/180.0f;
    const float pitchR = camPitch*(float)M_PI/180.0f;
    // Calcula a posição da câmera em órbita ao redor do campo, a uma distância camDist
    const float cx = camDist*cosf(pitchR)*sinf(yawR);
    const float cy = camDist*sinf(pitchR);
    const float cz = camDist*cosf(pitchR)*cosf(yawR);
    // a câmera sempre olha para a bola
    // mantém o horizonte nivelado
    gluLookAt(cx, cy, cz,  ballX, 0.0f, ballZ,  0.0f, 1.0f, 0.0f);
}

static void updateBall(float dt) {
    // atualiza a posição da bola (ballX, ballZ)
    // detecta gols
    // colisões com as bordas.

    // vetor de direção no plano XZ
    float dx = (float)(key_right - key_left);
    float dz = (float)(key_down - key_up);
    // comprimento
    float len = sqrtf(dx * dx + dz * dz);

    // Se o vetor de direção não for nulo, ele é normalizado
    if (len > 0) {
        dx /= len;
        dz /= len;
        // a posição da bola é atualizada
        ballX += dx * 15.0f * dt;
        ballZ += dz * 15.0f * dt;
    }

    // definição dos limites do campo
    float halfW = 68.0f * 0.5f - 0.25f;
    float halfL = 105.0f * 0.5f - 0.25f;

    int dentroX = (fabsf(ballX) <= 7.32f * 0.5f);
    // verifica se a bola ta dentro da largura do gol
    // se passou a linha de fundo superior
    if (dentroX && ballZ >= (105.0f * 0.5f - 0.25f)) {
        scoreBlue += 1;
        resetKickoff();
        return;
    }
    // se passou a linha de fundo inferior
    if (dentroX && ballZ <= -(105.0f * 0.5f - 0.25f)) {
        scoreRed += 1;
        resetKickoff();
        return;
    }

    // colisão com as bordas
    // Se a posição ultrapassar os limites
    // é corrigida para ficar na borda.
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

    const float speed = 1.0f;
    const float friction = 0.90f;

    float halfL = 105.0f * 0.5f - 0.5f;

    for (int i = 0; i < NUM_PLAYERS; i++) {

        Player* p = &players[i];
        float dx = ballX - p->x;
        float dz = ballZ - p->z;
        float len = sqrtf(dx*dx + dz*dz);
        float vx = 0.0f;
        float vz = 0.0f;

        if (len > 0.01f) {
            dx /= len;
            dz /= len;
            vx = dx * speed;
            vz = dz * speed;
        }

        p->x += vx * dt;
        p->z += vz * dt;

        if (len > 0.01f) {
            p->yawDeg = atan2f(dx, dz) * 180.0f / M_PI;
        }

        if (p->z > halfL)  p->z = halfL;
        if (p->z < -halfL) p->z = -halfL;

        float moveSpeed = sqrtf(vx*vx + vz*vz);

        if (moveSpeed > 0.1f) {
            p->animPhase += dt * (6.0f + moveSpeed * 0.2f);

            p->armSwing = sinf(p->animPhase) * 25.0f;
            p->legSwing = sinf(p->animPhase) * 35.0f;
        } else {
            p->armSwing *= friction;
            p->legSwing *= friction;
        }
    }
}


void resetKickoff(void){
    camYaw = 30.0f;
    camPitch = 25.0f;
    camDist = 140.0f;
    fovDeg = 60.0f;
    // recalcular a matriz de projeção
    updateProjection();
    ballX = 0.0f;
    ballZ = 0.0f;
    key_left = key_right = key_up = key_down = 0;

    players[0].x= 20.0f;
    players[0].z = 0.0f;
    players[0].yawDeg = 20.0f;
    players[1].x= 20.0f;
    players[1].z = 30.0f;
    players[1].yawDeg = 80.0f;
}

void initLogic(void){
    // pegando o tempo inicial
    lastTimeMs = glutGet(GLUT_ELAPSED_TIME);
    // Configura camera, bola, teclado e posições iniciais dos jogadores
    resetKickoff();
}

void idle(void){
    // Pega o tempo atual
    int now = glutGet(GLUT_ELAPSED_TIME);
    // para deixar a velocidade do jogo fica independente do FPS
    float dt = (now - lastTimeMs)*0.001f;
    
    if (dt < 0){
        dt = 0;
    }

    // Limita o dt
    // evita que a cena pule exageradamente.
    if (dt > 0.05f){
        dt = 0.05f;
    }

    lastTimeMs = now;
    updateBall(dt);
    updatePlayers(dt);
    // redesenhar a cena
    glutPostRedisplay();
}

void special(int key, int x, int y) {
    (void)x;
    (void)y;

    // setar flag
    // mover enquanto a tecla estiver pressionada
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
    // posição do mouse
    // não usadas
    (void)x;
    (void)y;

    // controlar campo de visão
    // zoom in
    if (key == '+') {
        fovDeg -= 10.0f;

        if (fovDeg < 30.0f) {
            fovDeg = 30.0f;
        }

        updateProjection();
    }
    // controlar campo de visão
    // zoom out
    if (key == '-') {
        fovDeg += 10.0f;

        if (fovDeg > 100.0f) {
            fovDeg = 100.0f;
        }

        updateProjection();
    }

    // Rotação da câmera
    // mexe no yaw
    if (key == 'j' || key == 'J') {
        camYaw -= 10.0f;
    }

    if (key == 'l' || key == 'L') {
        camYaw += 10.0f;
    }

    // Inclinação da câmera
    // eixo x
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

    // reseet
    if (key == 'r' || key == 'R') {
        resetKickoff();
    }

    // encerrar o programa
    if (key == 27) {
        extern void stopAudio(void);
        stopAudio();
        exit(0);
    }

    // para redesenhar a tela assim que possivel
    glutPostRedisplay();
}