#define RENDER_H

void initGL(void);
void updateProjection(void);
void reshape(int w,int h);
void display(void);
GLuint loadTextureJPG(const char* filename);
extern GLuint texBuilding;
extern GLuint texBrick;

