#include <math.h>
#include <GL/glut.h>
#include "render.h"
#include "logic.h"

#define M_PI 3.14159265358979323846

GLuint sky_rt, sky_lf, sky_up, sky_dn, sky_ft, sky_bk;
extern float camYaw, camPitch, camDist;


void makeShadowMatrix(GLfloat shadowMat[4][4], const GLfloat lightPos[4])
{
    GLfloat plane[4] = {0, 1, 0, 0};

    GLfloat dot = plane[0]*lightPos[0] + plane[1]*lightPos[1] +
                  plane[2]*lightPos[2] + plane[3]*lightPos[3];

    shadowMat[0][0] = dot - lightPos[0] * plane[0];
    shadowMat[1][0] = - lightPos[0] * plane[1];
    shadowMat[2][0] = - lightPos[0] * plane[2];
    shadowMat[3][0] = - lightPos[0] * plane[3];

    shadowMat[0][1] = - lightPos[1] * plane[0];
    shadowMat[1][1] = dot - lightPos[1] * plane[1];
    shadowMat[2][1] = - lightPos[1] * plane[2];
    shadowMat[3][1] = - lightPos[1] * plane[3];

    shadowMat[0][2] = - lightPos[2] * plane[0];
    shadowMat[1][2] = - lightPos[2] * plane[1];
    shadowMat[2][2] = dot - lightPos[2] * plane[2];
    shadowMat[3][2] = - lightPos[2] * plane[3];

    shadowMat[0][3] = - lightPos[3] * plane[0];
    shadowMat[1][3] = - lightPos[3] * plane[1];
    shadowMat[2][3] = - lightPos[3] * plane[2];
    shadowMat[3][3] = dot - lightPos[3] * plane[3];
}

void applyMaterial()
{
    GLfloat spec[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat shin[] = {32.0};

    glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
    glMaterialfv(GL_FRONT, GL_SHININESS, shin);
}

void loadSkyboxTextures()
{
    sky_rt = loadTextureJPG("jajlands1_rt.jpg");
    sky_lf = loadTextureJPG("jajlands1_lf.jpg");
    sky_up = loadTextureJPG("jajlands1_up.jpg");
    sky_dn = loadTextureJPG("jajlands1_dn.jpg");
    sky_ft = loadTextureJPG("jajlands1_ft.jpg");
    sky_bk = loadTextureJPG("jajlands1_bk.jpg");
}

static void setPerspective(void){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fovDeg, (float)winW/(float)winH, 0.1, 1000.0);
    glMatrixMode(GL_MODELVIEW);
}

void initGL(void){
    glClearColor(0.04f,0.10f,0.04f,1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    loadSkyboxTextures();
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
    glColor3f(0.75f, 0.75f, 0.75f);

    glBegin(GL_QUADS);
        glVertex3f(-200.0f, -0.02f, -200.0f);
        glVertex3f( 200.0f, -0.02f, -200.0f);
        glVertex3f( 200.0f, -0.02f,  200.0f);
        glVertex3f(-200.0f, -0.02f,  200.0f);
    glEnd();
}

static void drawGrassStripes(void){
    glBegin(GL_QUADS);

    for (int i = 0; i < 10; ++i) {
        glNormal3f(0,1,0);
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
        glNormal3f(0,1,0);
        glVertex3f(xmin,y,zmin);
        glVertex3f(xmax,y,zmin);
        glVertex3f(xmax,y,zmax);
        glVertex3f(xmin,y,zmax);
    glEnd();
}

static void lineXZ(float x0, float z0, float x1, float z1, float y){
    glBegin(GL_LINES);
        glNormal3f(0,1,0);
        glVertex3f(x0,y,z0);
        glVertex3f(x1,y,z1);
    glEnd();
}

static void drawArcXZ(float cx, float cz, float r, float a0_deg, float a1_deg, float y){
    int segs = 128;

    float a0 = a0_deg*(float)M_PI/180.0f;
    float a1 = a1_deg*(float)M_PI/180.0f;

    glBegin(GL_LINE_STRIP);
        glNormal3f(0,1,0);
        for(int i=0;i<=segs;i++){
            float t=(float)i/segs;
            float a=a0+t*(a1-a0);
            glVertex3f(cx + r*sinf(a), y, cz + r*cosf(a));
        }
    glEnd();
}

static void drawDiskXZ(float cx,float cz,float r,float y){
    int segs=48;

    glBegin(GL_TRIANGLE_FAN);
        glNormal3f(0,1,0);
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
    box_at(0.12f, 2.44f, 0.12f,  7.32f*0.5f, 2.44f*0.5f, zGoal);
    box_at(7.32f, 0.12f, 0.12f,  0.0f,        2.44f,      zGoal);
}

static void drawSoccerBall(void){
    glPushMatrix();
        glTranslatef(ballX, 0.25f, ballZ);
        glColor3f(1,1,1);
        glutSolidSphere(0.25f, 32, 24);
    glPopMatrix();
}

static void drawPlayerModel(const Player* p)
{
    glEnable(GL_NORMALIZE);
    glRotatef(p->yawDeg, 0, 1, 0);
    glColor3fv(p->shirt);
    box_at(0.40f, 0.80f, 0.25f, 0.0f, 1.0f, 0.0f);

    glPushMatrix();
        glTranslatef(0.0f, 1.40f + 0.22f, 0.0f);
        glColor3f(1.00f, 0.86f, 0.72f);
        glutSolidSphere(0.22f, 18, 18);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0.40f, 1.20f, 0.0f);
        glRotatef(p->armSwing, 1, 0, 0); 
        glColor3fv(p->shirt);
        box_at(0.15f, 0.50f, 0.15f, 0, -0.25f, 0);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(-0.40f, 1.20f, 0.0f);
        glRotatef(-p->armSwing, 1, 0, 0);
        glColor3fv(p->shirt);
        box_at(0.15f, 0.50f, 0.15f, 0, -0.25f, 0);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0.18f, 0.60f, 0.0f);
        glRotatef(-p->legSwing, 1, 0, 0);
        glColor3f(0.05f, 0.05f, 0.05f);
        box_at(0.18f, 0.60f, 0.18f, 0, -0.30f, 0);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(-0.18f, 0.60f, 0.0f);
        glRotatef(p->legSwing, 1, 0, 0);
        glColor3f(0.05f, 0.05f, 0.05f);
        box_at(0.18f, 0.60f, 0.18f, 0, -0.30f, 0);
    glPopMatrix();

    glDisable(GL_NORMALIZE);
}


static void drawPlayer(const Player* p){
    glPushMatrix();

        glPushAttrib(GL_CURRENT_BIT);

        glTranslatef(p->x, 0.0f, p->z);
        drawPlayerModel(p);

        glPopAttrib();

    glPopMatrix();
}


static void drawTallyRow(int n, float faceX, float baseY, float centerZ, float halfH, float sep, float r, float g, float b){
    if (n <= 0) return;

    int groups5 = n / 5;
    int rem = n % 5;
    int cols = groups5 * 4 + rem;

    if (cols <= 0) return;

    float total=(cols-1)*sep;
    float start=centerZ-total*0.5f;

    glColor3f(r,g,b);
    glLineWidth(2.0f);

    int i=0, col=0;

    while(i<n){
        int gcount = (n - i >= 5 ? 5 : n - i);

        int verts = (gcount == 5 ? 4 : gcount);

        for(int k=0;k<verts;k++){
            float z = start + col * sep;
            col++;

            glBegin(GL_LINES);
                glNormal3f(1,0,0);
                glVertex3f(faceX, baseY - halfH, z);
                glVertex3f(faceX, baseY + halfH, z);
            glEnd();
        }

        if(gcount==5){
            float z0=start+(col-4)*sep;
            float z3=start+(col-1)*sep;

            glBegin(GL_LINES);
                glNormal3f(1,0,0);
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

    float fx = panelX - 0.15f - 0.01f;
    float hy = 4.5f*0.5f - 0.15f;
    float hz = 10.0f*0.5f - 0.15f;

    glColor3f(1,1,1);
    glLineWidth(2.0f);

    glBegin(GL_LINE_LOOP);
        glNormal3f(1,0,0);
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

static void drawSkybox(float size)
{
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glEnable(GL_TEXTURE_2D);

    glPushMatrix();

    float yawR   = camYaw   * M_PI / 180.0f;
    float pitchR = camPitch * M_PI / 180.0f;

    float cx = camDist * cosf(pitchR) * sinf(yawR);
    float cy = camDist * sinf(pitchR);
    float cz = camDist * cosf(pitchR) * cosf(yawR);

    glTranslatef(cx, cy, cz);

    #define SKY_FACE(tex) \
        glBindTexture(GL_TEXTURE_2D, tex); \
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); \
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    SKY_FACE(sky_rt);
    glBegin(GL_QUADS);
        glTexCoord2f(0,1);
        glVertex3f( size, -size, -size);
        glTexCoord2f(0,0);
        glVertex3f( size, size, -size);
        glTexCoord2f(1,0);
        glVertex3f( size, size, size);
        glTexCoord2f(1,1);
        glVertex3f( size, -size, size);
    glEnd();

    SKY_FACE(sky_lf);
    glBegin(GL_QUADS);
        glTexCoord2f(0,1);
        glVertex3f(-size, -size, size);
        glTexCoord2f(0,0);
        glVertex3f(-size, size, size);
        glTexCoord2f(1,0);
        glVertex3f(-size, size, -size);
        glTexCoord2f(1,1);
        glVertex3f(-size, -size, -size);
    glEnd();

    SKY_FACE(sky_ft);
    glBegin(GL_QUADS);
        glTexCoord2f(0,1);
        glVertex3f( size, -size, size);
        glTexCoord2f(0,0);
        glVertex3f( size, size, size);
        glTexCoord2f(1,0);
        glVertex3f(-size, size, size);
        glTexCoord2f(1,1);
        glVertex3f(-size, -size, size);
    glEnd();

    SKY_FACE(sky_bk);
    glBegin(GL_QUADS);
        glTexCoord2f(0,1);
        glVertex3f(-size, -size, -size);
        glTexCoord2f(0,0);
        glVertex3f(-size, size, -size);
        glTexCoord2f(1,0);
        glVertex3f( size, size, -size);
        glTexCoord2f(1,1);
        glVertex3f( size, -size, -size);
    glEnd();

    SKY_FACE(sky_up);
    glBegin(GL_QUADS);
        glTexCoord2f(0,0);
        glVertex3f(-size, size, -size);
        glTexCoord2f(1,0);
        glVertex3f( size, size, -size);
        glTexCoord2f(1,1);
        glVertex3f( size, size, size);
        glTexCoord2f(0,1);
        glVertex3f(-size, size, size);
    glEnd();

    SKY_FACE(sky_dn);
    glBegin(GL_QUADS);
        glTexCoord2f(0,1);
        glVertex3f(-size, -size, -size);
        glTexCoord2f(1,1);
        glVertex3f( size, -size, -size);
        glTexCoord2f(1,0);
        glVertex3f( size, -size, size);
        glTexCoord2f(0,0);
        glVertex3f(-size, -size, size);
    glEnd();

    glPopMatrix();

    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}


static void drawStadiumFloor(void)
{
    float xMinWall = -49.0f;
    float xMaxWall =  45.0f;
    float zMinWall = -68.0f;
    float zMaxWall =  68.0f;

    float xMinField = -34.0f;
    float xMaxField =  34.0f;
    float zMinField = -52.5f;
    float zMaxField =  52.5f;

    glDisable(GL_TEXTURE_2D);
    glColor3f(0.75f, 0.70f, 0.65f);

    glBegin(GL_QUADS);

    glVertex3f(xMinWall, 0, zMinWall);
    glVertex3f(xMinField,0, zMinWall);
    glVertex3f(xMinField,0, zMaxWall);
    glVertex3f(xMinWall, 0, zMaxWall);

    glVertex3f(xMaxField,0, zMinWall);
    glVertex3f(xMaxWall,0, zMinWall);
    glVertex3f(xMaxWall,0, zMaxWall);
    glVertex3f(xMaxField,0, zMaxWall);

    glVertex3f(xMinField,0, zMaxField);
    glVertex3f(xMaxField,0, zMaxField);
    glVertex3f(xMaxField,0, zMaxWall);
    glVertex3f(xMinField,0, zMaxWall);

    glVertex3f(xMinField,0, zMinWall);
    glVertex3f(xMaxField,0, zMinWall);
    glVertex3f(xMaxField,0, zMinField);
    glVertex3f(xMinField,0, zMinField);

    glEnd();
}

static void drawSpotlight(void)
{
    GLUquadric* quad = gluNewQuadric();
    gluQuadricNormals(quad, GLU_SMOOTH);

    glColor3f(0.6f, 0.6f, 0.65f);
    glPushMatrix();
        glTranslatef(0, 0, 0);
        glRotatef(-90, 1, 0, 0);
        gluCylinder(quad, 0.6f, 0.6f, 20.0f, 16, 4);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0, 20.0f, 0);   
        glRotatef(35, 1, 0, 0);
        gluCylinder(quad, 0.3f, 0.3f, 4.0f, 12, 4);
    glPopMatrix();

    glPushMatrix();
        float ang = 35.0f * M_PI/180.0;
        glTranslatef(0, 20.0f - 4.0f * sin(ang), 4.0f * cos(ang));
        glRotatef(35, 1, 0, 0);

        glColor3f(0.3f, 0.3f, 0.33f);

        float w = 2.5f, h = 2.0f, d = 1.5f;

        glBegin(GL_QUADS);
            glVertex3f(-w, -h, 0);
            glVertex3f( w, -h, 0);
            glVertex3f( w,  h, 0);
            glVertex3f(-w,  h, 0);
        glEnd();
    glPopMatrix();

        glColor3f(1.0f, 1.0f, 0.85f);
        glBegin(GL_QUADS);
            glVertex3f(-w, -h, d + 0.01f);
            glVertex3f( w, -h, d + 0.01f);
            glVertex3f( w,  h, d + 0.01f);
            glVertex3f(-w,  h, d + 0.01f);
        glEnd();

    glPopMatrix();

    gluDeleteQuadric(quad);
}

static void drawAllSpotlights(void)
{
    glPushMatrix();
        glTranslatef(-40.0f, 0.0f, 50.0f);
        glRotatef(120,0,1,0);
        
        drawSpotlight();

        GLfloat pos1[] = { 0, 20, 4, 1 };
        GLfloat dir1[] = { 0, -1, 0 };

        glLightfv(GL_LIGHT2, GL_POSITION, pos1);
        glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, dir1);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(40.0f, 0.0f, -50.0f);
        glRotatef(-70,0,1,0);
        drawSpotlight();

        GLfloat pos2[] = { 0, 20, 4, 1 };
        GLfloat dir2[] = { 0, -1, 0 };

        glLightfv(GL_LIGHT3, GL_POSITION, pos2);
        glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, dir2);
    glPopMatrix();
}



static void drawStadiumWalls(void)
{
    float wallH = 6.0f;
    float xMin = -49.0f;
    float xMax =  45.0f;
    float zMin = -68.0f;
    float zMax =  68.0f;

    float repX = 6.0f;
    float repY = 2.0f;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texBrick);
    glColor3f(1,1,1);

    glBegin(GL_QUADS);

    glTexCoord2f(0,0);
    glVertex3f(xMin, 0, zMin);
    glTexCoord2f(repX,0);
    glVertex3f(xMin, 0, zMax);
    glTexCoord2f(repX,repY);
    glVertex3f(xMin, wallH, zMax);
    glTexCoord2f(0,repY);
    glVertex3f(xMin, wallH, zMin);

    glTexCoord2f(0,0);
    glVertex3f(xMin, 0, zMax);
    glTexCoord2f(repX,0);
    glVertex3f(xMin, 0, zMin);
    glTexCoord2f(repX,repY);
    glVertex3f(xMin, wallH, zMin);
    glTexCoord2f(0,repY);
    glVertex3f(xMin, wallH, zMax);

    glTexCoord2f(0,0);
    glVertex3f(xMax, 0, zMax);
    glTexCoord2f(repX,0);
    glVertex3f(xMax, 0, zMin);
    glTexCoord2f(repX,repY);
    glVertex3f(xMax, wallH, zMin);
    glTexCoord2f(0,repY);
    glVertex3f(xMax, wallH, zMax);

    glTexCoord2f(0,0);
    glVertex3f(xMax, 0, zMin);
    glTexCoord2f(repX,0);
    glVertex3f(xMax, 0, zMax);
    glTexCoord2f(repX,repY);
    glVertex3f(xMax, wallH, zMax);
    glTexCoord2f(0,repY);
    glVertex3f(xMax, wallH, zMin);

    glTexCoord2f(0,0);
    glVertex3f(xMin, 0, zMax);
    glTexCoord2f(repX,0);
    glVertex3f(xMax, 0, zMax);
    glTexCoord2f(repX,repY);
    glVertex3f(xMax, wallH, zMax);
    glTexCoord2f(0,repY);
    glVertex3f(xMin, wallH, zMax);

    glTexCoord2f(0,0);
    glVertex3f(xMax, 0, zMax);
    glTexCoord2f(repX,0);
    glVertex3f(xMin, 0, zMax);
    glTexCoord2f(repX,repY);
    glVertex3f(xMin, wallH, zMax);
    glTexCoord2f(0,repY);
    glVertex3f(xMax, wallH, zMax);

    glTexCoord2f(0,0);
    glVertex3f(xMax, 0, zMin);
    glTexCoord2f(repX,0);
    glVertex3f(xMin, 0, zMin);
    glTexCoord2f(repX,repY);
    glVertex3f(xMin, wallH, zMin);
    glTexCoord2f(0,repY);
    glVertex3f(xMax, wallH, zMin);

    glTexCoord2f(0,0);
    glVertex3f(xMin, 0, zMin);
    glTexCoord2f(repX,0);
    glVertex3f(xMax, 0, zMin);
    glTexCoord2f(repX,repY);
    glVertex3f(xMax, wallH, zMin);
    glTexCoord2f(0,repY);
    glVertex3f(xMin, wallH, zMin);

    glEnd();

    glDisable(GL_TEXTURE_2D);
}


static void drawBuilding(void)
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texBuilding);
    glColor3f(1,1,1);

    float size = 6.0f;
    float h = 15.0f;
    float x = -42.0f;
    float z =  60.0f;

    glPushMatrix();
        glTranslatef(x, h*0.5f, z);

        glBegin(GL_QUADS);

        glTexCoord2f(0,1);
        glVertex3f( size, -h*0.5f, -size);
        glTexCoord2f(1,1);
        glVertex3f( size, -h*0.5f, size);
        glTexCoord2f(1,0);
        glVertex3f( size,  h*0.5f, size);
        glTexCoord2f(0,0);
        glVertex3f( size,  h*0.5f, -size);

        glTexCoord2f(1,1);
        glVertex3f(-size, -h*0.5f, -size);
        glTexCoord2f(0,1);
        glVertex3f(-size, -h*0.5f, size);
        glTexCoord2f(0,0);
        glVertex3f(-size,  h*0.5f, size);
        glTexCoord2f(1,0);
        glVertex3f(-size,  h*0.5f, -size);

        glTexCoord2f(0,1);
        glVertex3f(-size, -h*0.5f, size);
        glTexCoord2f(1,1);
        glVertex3f(size, -h*0.5f, size);
        glTexCoord2f(1,0);
        glVertex3f(size,  h*0.5f, size);
        glTexCoord2f(0,0);
        glVertex3f(-size,  h*0.5f, size);

        glTexCoord2f(1,1);
        glVertex3f( size, -h*0.5f,-size);
        glTexCoord2f(0,1);
        glVertex3f(-size, -h*0.5f,-size);
        glTexCoord2f(0,0);
        glVertex3f(-size,  h*0.5f,-size);
        glTexCoord2f(1,0);
        glVertex3f( size,  h*0.5f,-size);
        
        glEnd();

        glDisable(GL_TEXTURE_2D);
        glColor3f(0.60f, 0.60f, 0.60f);

        glBegin(GL_QUADS);
            glVertex3f(-size,  h*0.5f, -size);
            glVertex3f( size,  h*0.5f, -size);
            glVertex3f( size,  h*0.5f,  size);
            glVertex3f(-size,  h*0.5f,  size);
        glEnd();

    glPopMatrix();
}

extern GLuint texBrick;

static void drawGrandstand(void){
    const float L  = 105.0f;
    const float W  = 68.0f;

    const float marginFromField = 3.0f;
    const float baseX = -(W * 0.5f) - marginFromField;
    const float zLen  = L - 10.0f;
    const float z0 = -zLen * 0.5f, z1 = +zLen * 0.5f;

    const int   tiers = 8;
    const float stepDepthX = 1.4f;
    const float stepHeightY = 0.6f;
    const float slabThickY = 0.25f;

    glColor3f(0.55f,0.55f,0.58f);
    box_at(0.6f, 1.0f, zLen, baseX-0.3f, 0.5f, 0.0f);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texBrick);
    glColor3f(1.0f, 1.0f, 1.0f);

    for(int t = 0; t < tiers; t++){
        float tx = baseX - t*stepDepthX - 0.5f*stepDepthX;
        float ty = (t+1)*stepHeightY;

        float x0 = tx - stepDepthX*0.5f;
        float x1 = tx + stepDepthX*0.5f;

        float y0 = ty;
        float y1 = ty + slabThickY;

        float zStart = -zLen*0.5f;
        float zEnd   = +zLen*0.5f;

        float repeatX = 1.5f;
        float repeatZ = zLen / 4.0f;

        glBegin(GL_QUADS);
            glTexCoord2f(0, 0);
            glVertex3f(x0, y0, zStart);
            glTexCoord2f(repeatX, 0);
            glVertex3f(x1, y0, zStart);
            glTexCoord2f(repeatX, repeatZ);
            glVertex3f(x1, y0, zEnd);
            glTexCoord2f(0, repeatZ);
            glVertex3f(x0, y0, zEnd);
        glEnd();

        const float personX = 0.30f;
        const float personY = 0.90f;
        const float personZ = 0.30f;
        const float spacingZ = 1.0f;
        const float headR = 0.20f;

        for(float z = z0+1.0f; z <= z1-1.0f; z += spacingZ){
            float tt = glutGet(GLUT_ELAPSED_TIME)/1000.0f;

            glDisable(GL_TEXTURE_2D);
            glColor3f(fabs((tx+z)/100), 0.0, 0.0);

            box_at(personX, personY, personZ,
                tx, ty + fabs(sin(z+tt*5))*0.2f, z);

            glPushMatrix();
                glTranslatef(tx,
                    ty + fabs(sin(z+tt*5))*0.2f + (0.80f + 0.12f*1.2f),
                    z);

                glColor3f(1.00f, 0.86f, 0.72f);
                glutSolidSphere(headR, 18, 18);
            glPopMatrix();

            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, texBrick);
        }
    }

    glDisable(GL_TEXTURE_2D);
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

    applyMaterial();

    applyCamera();
    drawSkybox(30.0f);
    GLfloat light1_pos[] = { -40, 20, 50, 1 };
    GLfloat light1_dir[] = { 40, -20, -50 };

    GLfloat light2_pos[] = { 40, 20, -50, 1 };
    GLfloat light2_dir[] = { -40, -20, 50 };

    glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light1_dir);

    glLightfv(GL_LIGHT2, GL_POSITION, light2_pos);
    glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, light2_dir);

    drawGround();
    drawBuilding();
    drawStadiumWalls(); 
    drawGrassStripes();
    drawStadiumFloor();
    drawFieldLines();
    drawAllSpotlights();
    
    GLfloat lightPos[4] = {40.0f, 25.0f, 60.0f, 1.0f};
    GLfloat shadowMat[4][4];
    makeShadowMatrix(shadowMat, lightPos);

    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);

    glColor4f(0.0f, 0.0f, 0.0f, 0.35f);

    glPushMatrix();
        glMultMatrixf(&shadowMat[0][0]);
        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        drawGoalAt(-105.0f*0.5f);
        drawGoalAt(105.0f*0.5f);
        drawSoccerBall();

        for (int i = 0; i < NUM_PLAYERS; i++)
            drawPlayer(&players[i]);
            
        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);

    glPopMatrix();

    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    
    drawGoalAt(-105.0f*0.5f);
    drawGoalAt(105.0f*0.5f);

    drawGrandstand();
    drawEndStand(1.0f);
    drawEndStand(-1.0f);

    drawScoreboard();
    drawSoccerBall();

    for (int i = 0; i < NUM_PLAYERS; i++) {
        drawPlayer(&players[i]);
    }

    glutSwapBuffers();
}

