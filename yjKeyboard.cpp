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

void myTranslate(float tx, float ty) {
    float T[3][3] = {
        {1, 0, tx},
        {0, 1, ty},
        {0, 0, 1}
    };
    multMatrix(T, transformMatrix, transformMatrix);
}

void myScale(float sx, float sy) {
    float S[3][3] = {
        {sx, 0, 0},
        {0, sy, 0},
        {0, 0, 1}
    };
    multMatrix(S, transformMatrix, transformMatrix);
}

void myRotate(float theta) {
    float theta_rad = theta * M_PI / 180.0f;
    float cos_t = cosf(theta_rad);
    float sin_t = sinf(theta_rad);
    float R[3][3] = {
        {cos_t, -sin_t, 0},
        {sin_t, cos_t, 0},
        {0, 0, 1}
    };
    multMatrix(R, transformMatrix, transformMatrix);
}

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

void keyboard(unsigned char key, int x, int y)
{
    //x값 입력 후 enter, y값 입력 후 enter
    if (key == '\r' || key == '\n') {
        float value = std::stof(inputBuffer);
        inputBuffer = "";

        switch (inputState) {
        case TRANSLATE_X: tx = value; inputState = TRANSLATE_Y; std::cout << "y값 입력: "; break;
        case TRANSLATE_Y: ty = value; myTranslate(tx, ty); inputState = NONE; glutPostRedisplay(); break;
        case SCALE_X: sx = value;inputState = SCALE_Y; std::cout << "y값 입력: "; break;
        case SCALE_Y: sy = value; myScale(sx, sy); inputState = NONE; glutPostRedisplay(); break;
        case ROTATE_ANGLE: theta = value; myRotate(theta); inputState = NONE; glutPostRedisplay(); break;
        default: break;
        }
        return;
    }

    if (isdigit(key) || key == '.' || key == '-' || key == '+') {
        inputBuffer += key;
        std::cout << key << std::flush;
        return;
    }

    switch (key) {
    case 'T': case 't': inputState = TRANSLATE_X; std::cout << "x값 입력: "; break;
    case 'S': case 's': inputState = SCALE_X; std::cout << "x값 입력"; break;
    case 'R': case 'r': inputState = ROTATE_ANGLE; std::cout << "회전 각도 입력: "; break;
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
    glutKeyboardFunc(keyboard);
    yjinit();
    glutDisplayFunc(display);
    glutMainLoop();

    return 0;

}
