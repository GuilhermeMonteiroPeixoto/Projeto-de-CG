#include <GL/glut.h>
#include "render.h"
#include "logic.h"
#include "audio.h"

int main(int argc, char** argv){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
    glutInitWindowSize(1280, 720);
    glutInitWindowPosition(100, 80);
    glutCreateWindow("Campo de Futebol");

    initGL();
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
