#include <GL/glut.h>
#include "render.h"
#include "logic.h"
#include "audio.h"

void initLighting()
{
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    GLfloat globalAmbient[] = { 0.10f, 0.10f, 0.10f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHT1);

    GLfloat diffuse[] =  {1.0f, 1.0f, 0.9f, 1.0f};
    GLfloat specular[] = {1.0f, 1.0f, 1.0f, 1.0f};

    glEnable(GL_LIGHT2);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT2, GL_SPECULAR, specular);
    glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 38.0f);
    glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 10.0f);

    glEnable(GL_LIGHT3);
    glLightfv(GL_LIGHT3, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT3, GL_SPECULAR, specular);
    glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, 38.0f);
    glLightf(GL_LIGHT3, GL_SPOT_EXPONENT, 10.0f);

    glEnable(GL_LIGHT4);
    glLightfv(GL_LIGHT4, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT4, GL_SPECULAR, specular);
    glLightf(GL_LIGHT4, GL_SPOT_CUTOFF, 38.0f);
    glLightf(GL_LIGHT4, GL_SPOT_EXPONENT, 10.0f);

    glEnable(GL_LIGHT5);
    glLightfv(GL_LIGHT5, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT5, GL_SPECULAR, specular);
    glLightf(GL_LIGHT5, GL_SPOT_CUTOFF, 38.0f);
    glLightf(GL_LIGHT5, GL_SPOT_EXPONENT, 10.0f);
}

GLuint texBrick;
GLuint texBuilding;

int main(int argc, char** argv){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
    glutInitWindowSize(1280, 720);
    glutInitWindowPosition(100, 80);
    glutCreateWindow("Campo de Futebol");

    initGL();
    initLighting();

    texBrick = loadTextureJPG("brick.jpg");
    texBuilding = loadTextureJPG("build.jpg");

    initLogic();
    initAudio();

    glutReshapeFunc(reshape);
    glutDisplayFunc(display);

    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutSpecialUpFunc(specialUp);

    glutIdleFunc(idle);
    glutIgnoreKeyRepeat(1);

    glutMainLoop();

    stopAudio();
    return 0;
}

