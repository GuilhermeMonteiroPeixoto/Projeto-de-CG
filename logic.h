#define LOGIC_H

typedef struct {
    float x, z;
    float yawDeg;
    float shirt[3];
} Player;

#define NUM_PLAYERS 6

extern int winW, winH;
extern float fovDeg, camDist, camYaw, camPitch;
extern float ballX, ballZ;
extern int scoreBlue, scoreRed;
extern Player players[NUM_PLAYERS];

void initLogic(void);
void resetKickoff(void);
void idle(void);
void keyboard(unsigned char key,int x,int y);
void special(int key,int x,int y);
void specialUp(int key,int x,int y);
void applyCamera(void);