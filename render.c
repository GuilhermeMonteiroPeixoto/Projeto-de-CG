#include <math.h>
#include <GL/glut.h>
#include "render.h"
#include "logic.h"

#define M_PI 3.14159265358979323846

static void setPerspective(void){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fovDeg, (float)winW/(float)winH, 0.1, 1000.0);
    glMatrixMode(GL_MODELVIEW);
}

void initGL(void){
    glClearColor(0.04f,0.10f,0.04f,1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    setPerspective();
}

void updateProjection(void){
    setPerspective();
}

static void box_at(float sx,float sy,float sz,float tx,float ty,float tz){
    glPushMatrix();
        glTranslatef(tx,ty,tz);
        glScalef(sx,sy,sz);
        glutSolidCube(1.0);
    glPopMatrix();
}

static void drawGround(void){
    glColor3f(0.18f, 0.19f, 0.20f);
    glBegin(GL_QUADS);
        glVertex3f(-400.0f, -0.02f, -400.0f);
        glVertex3f( 400.0f, -0.02f, -400.0f);
        glVertex3f( 400.0f, -0.02f,  400.0f);
        glVertex3f(-400.0f, -0.02f,  400.0f);
    glEnd();
}

static void drawGrassStripes(void){
    glBegin(GL_QUADS);
    for (int i = 0; i < 10; ++i) {
        float x0 = -68.0f * 0.5f + i * (68.0f / 10.0f);
        glColor3f(0.25f, 0.36f - 0.06f * (i & 1), 0.10f);
        glVertex3f(x0, 0, -105.0f * 0.5f);
        glVertex3f(x0 + 68.0f/10.0f, 0, -105.0f * 0.5f);
        glVertex3f(x0 + 68.0f/10.0f, 0,  105.0f * 0.5f);
        glVertex3f(x0, 0,  105.0f * 0.5f);
    }
    glEnd();
}

static void drawRectLinesXZ(float xmin, float xmax, float zmin, float zmax, float y){
    glBegin(GL_LINE_LOOP);
        glVertex3f(xmin,y,zmin);
        glVertex3f(xmax,y,zmin);
        glVertex3f(xmax,y,zmax);
        glVertex3f(xmin,y,zmax);
    glEnd();
}

static void lineXZ(float x0, float z0, float x1, float z1, float y){
    glBegin(GL_LINES);
        glVertex3f(x0,y,z0);
        glVertex3f(x1,y,z1);
    glEnd();
}

static void drawArcXZ(float cx, float cz, float r, float a0_deg, float a1_deg, float y){
    int segs = 128;
    float a0 = a0_deg*(float)M_PI/180.0f;
    float a1 = a1_deg*(float)M_PI/180.0f;
    glBegin(GL_LINE_STRIP);
        for(int i=0;i<=segs;i++){
            float t=(float)i/segs, a=a0+t*(a1-a0);
            glVertex3f(cx + r*sinf(a), y, cz + r*cosf(a));
        }
    glEnd();
}

static void drawDiskXZ(float cx,float cz,float r,float y){
    int segs=48;
    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(cx,y,cz);
        for(int i=0;i<=segs;i++){
            float a = (2.0f*(float)M_PI*i)/segs;
            glVertex3f(cx + r*sinf(a), y, cz + r*cosf(a));
        }
    glEnd();
}

static void drawFieldLines(void){
    const float L=105.0f; 
    const float W=68.0f;
    const float y=0.01f;
    glLineWidth(2.5f);
    glColor3f(1,1,1);

    drawRectLinesXZ(-W*0.5f, W*0.5f, -L*0.5f, L*0.5f, y);
    lineXZ(-W*0.5f,0, W*0.5f,0, y);

    drawArcXZ(0,0, 9.15f, 0,360, y);
    drawDiskXZ(0,0, 0.20f, y);

    float zS=-L*0.5f;
    float zN=+L*0.5f;
    drawRectLinesXZ(-20.16f, 20.16f, zS, zS+16.50f, y);
    drawRectLinesXZ(-9.16f,  9.16f,  zS, zS+5.50f,  y);
    drawDiskXZ(0.0f, zS+11.0f, 0.10f, y);

    drawRectLinesXZ(-20.16f, 20.16f, zN-16.50f, zN, y);
    drawRectLinesXZ(-9.16f,  9.16f,  zN-5.50f,  zN, y);
    drawDiskXZ(0.0f, zN-11.0f, 0.10f, y);
}

static void drawGoalAt(float zGoal){
    glColor3f(1,1,1);
    box_at(0.12f, 2.44f, 0.12f, -7.32f*0.5f, 2.44f*0.5f, zGoal);
    box_at(0.12f, 2.44f, 0.12f, 7.32f*0.5f, 2.44f*0.5f, zGoal);
    box_at(7.32f, 0.12f, 0.12f, 0.0f, 2.44f, zGoal);
}

static void drawSoccerBall(void){
    glPushMatrix();
        glTranslatef(ballX, 0.25f, ballZ);
        glColor3f(1,1,1);
        glutSolidSphere(0.25f, 32, 24);
    glPopMatrix();
}

static void drawPlayerModel(float yawDeg, const float shirt[3]){
    glRotatef(yawDeg, 0,1,0);

    glColor3f(shirt[0], shirt[1], shirt[2]);
    box_at(0.40f, 0.80f, 0.22f, 0.0f, 0.80f*0.5f, 0.0f);

    glColor3f(1.00f, 0.86f, 0.72f);
    glPushMatrix();
        glTranslatef(0.0f, 0.80f + 0.12f*1.2f, 0.0f);
        glutSolidSphere(0.12f, 18, 18);
    glPopMatrix();
}

static void drawPlayer(const Player* p){
    glPushMatrix();
        glTranslatef(p->x, 0.0f, p->z);
        drawPlayerModel(p->yawDeg, p->shirt);
    glPopMatrix();
}

static void drawTallyRow(int n, float faceX, float baseY, float centerZ, float halfH, float sep, float r, float g, float b){
    if (n <= 0){
        return;
    }

    int groups5 = n / 5;
    int rem = n % 5;
    int cols = groups5 * 4 + rem;

    if (cols <= 0){
        return;
    }

    float total=(cols-1)*sep;
    float start=centerZ-total*0.5f;
    glColor3f(r,g,b);
    glLineWidth(2.0f);

    int i=0, col=0;
    while(i<n){
        int gcount;
        if (n - i >= 5){
            gcount = 5;
        } else{
            gcount = (n - i);
        }

        int verts;
        if (gcount == 5){
            verts = 4;
        } else{
            verts = gcount;
        }

        for(int k=0;k<verts;k++){
            float z = start + col * sep;
            col = col + 1;
            glBegin(GL_LINES);
                glVertex3f(faceX, baseY - halfH, z);
                glVertex3f(faceX, baseY + halfH, z);
            glEnd();
        }
        if(gcount==5){
            float z0=start+(col-4)*sep, z3=start+(col-1)*sep;
            glBegin(GL_LINES);
                glVertex3f(faceX, baseY - halfH*0.9f, z0);
                glVertex3f(faceX, baseY + halfH*0.9f, z3);
            glEnd();
        }
        i+=gcount;
    }
}

static void drawScoreboard(void){
    float panelX = (68.0f*0.5f) + 10.0f;
    float panelY = 10.0f;
    float panelZ = 0.0f;
    glColor3f(0.06f,0.06f,0.08f);
    box_at(0.30f, 4.5f, 10.0f, panelX, panelY, panelZ);

    float fx = panelX - 0.30f*0.5f - 0.01f;
    float hy = 4.5f*0.5f - 0.15f;
    float hz = 10.0f*0.5f - 0.15f;
    glColor3f(1,1,1);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex3f(fx, panelY - hy, panelZ - hz);
        glVertex3f(fx, panelY - hy, panelZ + hz);
        glVertex3f(fx, panelY + hy, panelZ + hz);
        glVertex3f(fx, panelY + hy, panelZ - hz);
    glEnd();

    float sepZ=0.40f;
    float halfH=0.80f;
    drawTallyRow(scoreBlue, fx, panelY + hy*0.55f, panelZ, halfH, sepZ, 0.30f, 0.80f, 1.00f);
    drawTallyRow(scoreRed,  fx, panelY - hy*0.55f, panelZ, halfH, sepZ, 1.00f, 0.40f, 0.40f);
}

static void drawGrandstand(void){
    const float L = 105.0f;
    const float W = 68.0f;

    const float marginFromField = 3.0f;
    const float baseX = -(W*0.5f) - marginFromField;
    const float zLen = L - 10.0f;
    const float z0 = -zLen*0.5f, z1 = +zLen*0.5f;

    const int   tiers = 8;
    const float stepDepthX = 1.4f;
    const float stepHeightY = 0.6f;
    const float slabThickY = 0.25f;

    glColor3f(0.55f,0.55f,0.58f);
    box_at(0.6f, 1.0f, zLen, baseX-0.3f, 0.5f, 0.0f);

    for(int t=0;t<tiers;t++){
        float tx = baseX - t*stepDepthX - 0.5f*stepDepthX;
        float ty = (t+1)*stepHeightY;
        box_at(stepDepthX, slabThickY, zLen, tx, ty, 0.0f);
    }

    const float personX = 0.30f;
    const float personY = 0.90f;
    const float personZ = 0.30f;
    const float spacingZ = 1.0f;

    for(int t=0;t<tiers;t++){
        float tx = baseX - t*stepDepthX - 0.5f*stepDepthX;
        float ty = (t+1)*stepHeightY + slabThickY*0.5f + personY*0.5f;
        for(float z=z0+1.0f; z<=z1-1.0f; z+=spacingZ){
            float tt = glutGet(GLUT_ELAPSED_TIME)/1000.0f;
            glColor3f(fabs((tx+z)/(100)), 0.0, 0.0);
            box_at(personX, personY, personZ, tx, ty+fabs(sin(z+tt*5))*0.2, z);
            glPushMatrix();
                glTranslatef(tx, ty+fabs(sin(z+tt*5))*0.2+(0.80f + 0.12f*1.2f), z);
                glColor3f(1.00f, 0.86f, 0.72f);
                glutSolidSphere(0.2f, 18, 18);
            glPopMatrix();
        }
    }
}

static void drawEndStand(float zSign){
    const float L = 105.0f;
    const float W = 68.0f;

    const float marginFromGoal = 3.0f;
    const float baseZ = zSign * (L*0.5f + marginFromGoal);

    const float widthX = W - 10.0f;
    const float x0 = -widthX*0.5f, x1 = +widthX*0.5f;

    const int   tiers = 8;
    const float stepDepthZ = 1.4f;
    const float stepHeightY = 0.6f;
    const float slabThickY = 0.25f;

    glColor3f(0.55f,0.55f,0.58f);
    box_at(widthX, 1.0f, 0.6f, 0.0f, 0.5f, baseZ + zSign*0.3f);

    for(int ti=0; ti<tiers; ++ti){
        float tz = baseZ + zSign * (ti*stepDepthZ + 0.5f*stepDepthZ);
        float ty = (ti+1)*stepHeightY;
        box_at(widthX, slabThickY, stepDepthZ, 0.0f, ty, tz);

        const float personX = 0.30f;
        const float personY = 0.90f;
        const float personZ = 0.30f;
        const float spacingX = 1.0f;
        const float headR = 0.20f;

        float bodyY = ty + slabThickY*0.5f + personY*0.5f;

        for(float x = x0 + 1.0f; x <= x1 - 1.0f; x += spacingX){
            glColor3f(0.95f, 0.10f, 0.10f);
            box_at(personX, personY, personZ, x, bodyY, tz);

            glPushMatrix();
                glTranslatef(x, bodyY + personY*0.5f + headR, tz);
                glColor3f(1.00f, 0.86f, 0.72f);
                glutSolidSphere(headR, 18, 18);
            glPopMatrix();
        }
    }
}

void reshape(int w,int h){
    if (h == 0){
        h = 1;
    }
    winW=w;
    winH=h;
    glViewport(0,0,w,h);
    setPerspective();
}

void display(void){
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    applyCamera();

    drawGround();
    drawGrassStripes();
    drawFieldLines();
    drawGoalAt(-105.0f*0.5f);
    drawGoalAt(105.0f*0.5f);

    drawGrandstand();
    drawEndStand(1.0f);
    drawEndStand(-1.0f);

    extern void drawScoreboard(void);
    extern void drawSoccerBall(void);

    drawScoreboard();
    drawSoccerBall();

    for (int i = 0; i < NUM_PLAYERS; i++) {
        drawPlayer(&players[i]);
    }

    glutSwapBuffers();
}

