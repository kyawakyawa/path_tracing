#pragma once


#include <GL/glut.h>
#include "../Scene/Scene.hpp"

Scene *glScene;
void idle(){
    glScene->camera->sensitization(&glScene->shapes,&glScene->lights,1,4);
	printf("%ld\n",glScene->camera->sample_num);
	glutPostRedisplay();
}

void display(void){
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_POINTS);
    for(int i = 0;i < glScene->camera->pixel_h;i++){
        for(int j = 0;j < glScene->camera->pixel_w;j++){
            R r = glScene->camera->img[i * glScene->camera->pixel_w + j].red;
            R g = glScene->camera->img[i * glScene->camera->pixel_w + j].green;
            R b = glScene->camera->img[i * glScene->camera->pixel_w + j].blue;
            r = std::max((R)0.0,r);r = std::min((R)1.0,r);
            g = std::max((R)0.0,g);g = std::min((R)1.0,g);
            b = std::max((R)0.0,b);b = std::min((R)1.0,b);
            r = std::pow(r,1.0 / 2.2);
            g = std::pow(g,1.0 / 2.2);
            b = std::pow(b,1.0 / 2.2);
            glColor3d(r,g,b);
            glVertex2d(-1.0 + (float)j / (float)glScene->camera->pixel_w * 2.0,1.0 - (float)i / (float)glScene->camera->pixel_w * 2.0);
        }
    }
    glEnd();
    glutSwapBuffers();
}

void resize(int w, int h)
{
    GLdouble size;

    size = (GLdouble)glScene->camera->pixel_w;
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

    glutInitWindowSize(glScene->camera->pixel_w, glScene->camera->pixel_h);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA);
    glutCreateWindow(argv[0]);
    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutIdleFunc(idle);
    init();
    glutMainLoop();
}
