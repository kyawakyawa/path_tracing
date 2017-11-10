#pragma once


#include <GL/glut.h>
#include "Scene.h"

Scene *glScene;
void idle(){
	static int N = 0;
    int n = 20;
    glScene->compute(n,N);
    N += n;
	printf("%d\n",N);
	glutPostRedisplay();
}

void display(void){
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_POINTS);
    for(int i = 0;i < glScene->HEIGHT;i++){
        for(int j = 0;j < glScene->WIDTH;j++){
            R r = glScene->img[i * glScene->WIDTH + j].red;
            R g = glScene->img[i * glScene->WIDTH + j].green;
            R b = glScene->img[i * glScene->WIDTH + j].blue;
            r = std::max(0.0,r);r = std::min(1.0,r);
            g = std::max(0.0,g);g = std::min(1.0,g);
            b = std::max(0.0,b);b = std::min(1.0,b);
            r = std::pow(r,1.0 / 2.2);
            g = std::pow(g,1.0 / 2.2);
            b = std::pow(b,1.0 / 2.2);
            glColor3d(r,g,b);
            glVertex2d(-1.0 + (float)j / (float)glScene->WIDTH * 2.0,1.0 - (float)i / (float)glScene->HEIGHT * 2.0);
        }
    }
    glEnd();
    glutSwapBuffers();
}

void resize(int w, int h)
{
    GLdouble size;

    size = (GLdouble)glScene->WIDTH;
    /* ウィンドウ全体をビューポートにする */
    glViewport(0, 0, w, h);

    /* 変換行列の初期化 */
    glLoadIdentity();

    /* スクリーン上の表示領域をビューポートの大きさに比例させる */
    glOrtho(-w / size, w / size, -h / size, h / size, -1.0, 1.0);
}
void init(void)
{
    glClearColor(0.0,0.0,0.0,0.0);
}


void drawgl(int argc, char **argv,Scene scene) {
	glScene = new Scene(scene);

    glutInitWindowSize(glScene->WIDTH, glScene->HEIGHT);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA);
    glutCreateWindow(argv[0]);
    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutIdleFunc(idle);
    init();
    glutMainLoop();
}
