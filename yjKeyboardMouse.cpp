#include <Windows.h>
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <iostream>
#include <cmath>
#include <string>

using namespace std;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

enum InputState { NONE, TRANSLATE_X, TRANSLATE_Y, SCALE_X, SCALE_Y, ROTATE_ANGLE };
InputState inputState = NONE;

float tx = 0.0f, ty = 0.0f;
float sx = 1.0f, sy = 1.0f;
float theta = 0.0f;
std::string inputBuffer = "";

bool mouseDragging = false;
int mouseStartX = 0, mouseStartY = 0;
float start_tx = 0.0f, start_ty = 0.0f;
float start_sx = 1.0f, start_sy = 1.0f;
float start_theta = 0.0f;

void multMatrix(float A[3][3], float B[3][3], float C[3][3]) {
    float temp[3][3] = { 0 };
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            temp[i][j] = 0;
            for (int k = 0; k < 3; k++) {
                temp[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            C[i][j] = temp[i][j];
}


float transformMatrix[3][3] = {
    {1, 0, 0},
    {0, 1, 0},
    {0, 0, 1}
};

void transformPoint(float x, float y, float& out_x, float& out_y) {
    float homogeneous[3] = { x, y, 1.0f };
    float result[3] = { 0 };

    for (int i = 0; i < 3; i++) {
        result[i] = 0;
        for (int j = 0; j < 3; j++) {
            result[i] += transformMatrix[i][j] * homogeneous[j];
        }
    }
    out_x = result[0];
    out_y = result[1];
}

void yjinit(void)
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, 720.0, 0.0, 480.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void yjRocket()
{
    float x1, y1, x2, y2, x3, y3, x4, y4;

    //로켓 몸통
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_QUADS);
    transformPoint(340, 200, x1, y1); glVertex2f(x1, y1);
    transformPoint(380, 200, x2, y2); glVertex2f(x2, y2);
    transformPoint(380, 300, x3, y3); glVertex2f(x3, y3);
    transformPoint(340, 300, x4, y4); glVertex2f(x4, y4);
    glEnd();

    //로켓 앞부분
    glColor3f(1.0f, 1.0f, 0.0f);
    glBegin(GL_TRIANGLES);
    transformPoint(340, 300, x1, y1); glVertex2f(x1, y1);
    transformPoint(380, 300, x2, y2); glVertex2f(x2, y2);
    transformPoint(360, 340, x3, y3); glVertex2f(x3, y3);
    glEnd();

    //왼쪽 날개
    glColor3f(1.0f, 1.0f, 0.0f);
    glBegin(GL_QUADS);
    transformPoint(340, 200, x1, y1); glVertex2f(x1, y1);
    transformPoint(320, 180, x2, y2); glVertex2f(x2, y2);
    transformPoint(320, 210, x3, y3); glVertex2f(x3, y3);
    transformPoint(340, 230, x4, y4); glVertex2f(x4, y4);
    glEnd();

    // 오른쪽 날개
    glBegin(GL_QUADS);
    transformPoint(380, 200, x1, y1); glVertex2f(x1, y1);
    transformPoint(400, 180, x2, y2); glVertex2f(x2, y2);
    transformPoint(400, 210, x3, y3); glVertex2f(x3, y3);
    transformPoint(380, 230, x4, y4); glVertex2f(x4, y4);
    glEnd();
}
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            mouseDragging = true;
            mouseStartX = x;
            mouseStartY = y;
            // 현재 변환값 저장
            start_tx = tx;
            start_ty = ty;
            start_sx = sx;
            start_sy = sy;
            start_theta = theta;
        }
        else if (state == GLUT_UP) {
            mouseDragging = false;
            inputState = NONE;
            glutPostRedisplay();
        }
    }
}

void motion(int x, int y) {
    if (!mouseDragging) return;

    int dx = x - mouseStartX;
    int dy = y - mouseStartY;

    switch (inputState) {
    case TRANSLATE_X:
    case TRANSLATE_Y:
        tx = start_tx + dx;
        ty = start_ty - dy;
        transformMatrix[0][2] = tx;
        transformMatrix[1][2] = ty;
        glutPostRedisplay();
        break;

    case SCALE_X:
    case SCALE_Y: {
        float newWidth = 40.0f + dx;
        float newHeight = 100.0f - dy;

        sx = newWidth / 40.0f;
        sy = newHeight / 100.0f;

        transformMatrix[0][0] = sx;
        transformMatrix[1][1] = sy;
        glutPostRedisplay();
        break;
    }

    case ROTATE_ANGLE: {
        theta = start_theta + dx;
        float rad = theta * M_PI / 180.0f;
        float cos_t = cosf(rad);
        float sin_t = sinf(rad);
        transformMatrix[0][0] = cos_t;
        transformMatrix[0][1] = -sin_t;
        transformMatrix[1][0] = sin_t;
        transformMatrix[1][1] = cos_t;
        glutPostRedisplay();
        break;
    }
    default:
        break;
    }
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'T': case 't':
        inputState = TRANSLATE_X;
        std::cout << "Translation mode" << std::endl;
        break;
    case 'S': case 's':
        inputState = SCALE_X;
        std::cout << "Scale mode" << std::endl;
        break;
    case 'R': case 'r':
        inputState = ROTATE_ANGLE;
        std::cout << "Rotate mode" << std::endl;
        break;
    }
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    yjRocket();
    glFlush();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitWindowSize(720, 480);
    glutInitWindowPosition(10, 10);
    glutCreateWindow("yjRocket");
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyboard);
    yjinit();
    glutDisplayFunc(display);
    glutMainLoop();

    return 0;

}