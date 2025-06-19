#include <Windows.h>
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <iostream>
#include <cmath>
#include <string>

enum ObjectID { NONE_ID = 0, CUBE_ID = 1, TEAPOT_ID = 2, UPPER_ARM_ID = 3, LOWER_ARM_ID = 4};
int selectedObject = NONE_ID;

enum InputMode { NONE = 0, TRANSLATE_X, SCALE_X, ROTATE_ANGLE};
int inputState = NONE;

struct ObjectTransform {
    float tx = 0, ty = 0;
    float sx = 1.0f, sy = 1.0f;
    float theta = 0.0f;
};

ObjectTransform cubeTransform, teapotTransform;

struct ArmTransform {
    float upper_arm_tx = 0.1f, upper_arm_ty = 0.1f;
    float upper_arm_sx = 1.0f, upper_arm_sy = 1.0f;
    float upper_arm_theta = 0.0f;

    float lower_arm_tx = 0.1f, lower_arm_ty = 0.1f;
    float lower_arm_sx = 1.0f, lower_arm_sy = 1.0f;
    float lower_arm_theta = -30.0f;
};

ArmTransform armTransform;

bool mouseDragging = false;
int mouseStartX = 0, mouseStartY = 0;
float start_tx, start_ty, start_sx, start_sy, start_theta;

void yjinit(void)
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)720 / 480, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void drawBox() {
    glPushMatrix();
    glScalef(0.5, 0.1, 0.1);
    glutWireCube(1.0);
    glPopMatrix();
}

void yjArm(int base_tx, int base_ty, int base_tz, double shoulderAngle, double elbowAngle) {
    glPushMatrix();
    //팔 전체의 기본 위치
    glTranslatef(base_tx, base_ty, base_tz);

    glTranslatef(armTransform.upper_arm_tx, armTransform.upper_arm_ty, 0);
    glRotatef(armTransform.upper_arm_theta + shoulderAngle, 0, 0, 1);
    glScalef(armTransform.upper_arm_sx, armTransform.upper_arm_sy, 1);
    glTranslatef(0.05, 0.05, 0);
    // 색상변경
    if (selectedObject == LOWER_ARM_ID) {
        glColor3f(1, 1, 0);  // 노란색
    }
    else {
        glColor3f(0, 1, 0);
    }
    glPushMatrix();
    
    glTranslatef(0.25, -0.05, 0); //lower arm의 끝점을 upper arm의 끝점으로 이동
    glRotatef(armTransform.lower_arm_theta + elbowAngle, 0, 0, 1);
    glTranslatef(0.05, 0.05, 0);
    drawBox();
    glPopMatrix();
    // 색상변경
    if (selectedObject == UPPER_ARM_ID) {
        glColor3f(1, 1, 0);  // 노란색
    }
    else {
        glColor3f(0, 1, 0);
    }
    drawBox();
    glPopMatrix();

}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            mouseDragging = true;
            mouseStartX = x;
            mouseStartY = y;

            if (selectedObject == CUBE_ID) {
                start_tx = cubeTransform.tx;
                start_ty = cubeTransform.ty;
                start_sx = cubeTransform.sx;
                start_sy = cubeTransform.sy;
                start_theta = cubeTransform.theta;
            }
            else if (selectedObject == TEAPOT_ID) {
                start_tx = teapotTransform.tx;
                start_ty = teapotTransform.ty;
                start_sx = teapotTransform.sx;
                start_sy = teapotTransform.sy;
                start_theta = teapotTransform.theta;
            }
            else if (selectedObject == UPPER_ARM_ID) {
                start_tx = armTransform.upper_arm_tx;
                start_ty = armTransform.upper_arm_ty;
                start_sx = armTransform.upper_arm_sx;
                start_sy = armTransform.upper_arm_sy;
                start_theta = armTransform.upper_arm_theta;
            }
            else if (selectedObject == LOWER_ARM_ID) {
                start_theta = armTransform.lower_arm_theta;
            }
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

    ObjectTransform* t = nullptr;
    if (selectedObject == CUBE_ID) t = &cubeTransform;
    else if (selectedObject == TEAPOT_ID) t = &teapotTransform;
  
    if (selectedObject == UPPER_ARM_ID) {
        switch (inputState) {
        case TRANSLATE_X:
            armTransform.upper_arm_tx = start_tx + dx * 0.01f;
            armTransform.upper_arm_ty = start_ty - dy * 0.01f;
            break;
        case SCALE_X:
            armTransform.upper_arm_sx = start_sx + dx * 0.01f;
            armTransform.upper_arm_sy = start_sy - dy * 0.01f;
            if (armTransform.upper_arm_sx < 0.1f) armTransform.upper_arm_sx = 0.1f;
            if (armTransform.upper_arm_sy < 0.1f) armTransform.upper_arm_sy = 0.1f;
            break;
        case ROTATE_ANGLE:
            armTransform.upper_arm_theta = start_theta + dx;
            break;
        }
        glutPostRedisplay();
        return;
    }

    // Lower arm control (rotation만 가능)
    if (selectedObject == LOWER_ARM_ID) {
        switch (inputState) {
        case ROTATE_ANGLE:
            armTransform.lower_arm_theta = start_theta + dx;
            break;
        }
        glutPostRedisplay();
        return;
    }

    if (!t) return;

    switch (inputState) {
    case TRANSLATE_X:
        t->tx = start_tx + dx * 0.5f;
        t->ty = start_ty - dy * 0.5f;
        break;

    case SCALE_X:
        t->sx = start_sx + dx * 0.01f;
        t->sy = start_sy - dy * 0.01f;
        break;

    case ROTATE_ANGLE:
        t->theta = start_theta + dx;
        break;
    }

    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case '1':
        selectedObject = CUBE_ID;
        std::cout << "Cube selected" << std::endl;
        glutPostRedisplay();
        break;
    case '2':
        selectedObject = TEAPOT_ID;
        std::cout << "Teapot selected" << std::endl;
        glutPostRedisplay();
        break;
    case '3':
        selectedObject = UPPER_ARM_ID;
        std::cout << "상단팔 선택 - 상위팔+하위팔 모두 제어" << std::endl;
        glutPostRedisplay();
        break;
    case '4':
        selectedObject = LOWER_ARM_ID;
        std::cout << "하단팔 선택 - 하위팔만 제어" << std::endl;
        glutPostRedisplay();
        break;
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

void display(){
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //setting up the view
    gluLookAt(
        0.0, 0.0, 5.0,
        0.0, 0.0, 0.0,
        0.0, 1.0, 0.0);

    yjArm(0.1, 0.8, 0, -90, 10);

    //cube
    glPushMatrix();
    glTranslatef(cubeTransform.tx, cubeTransform.ty, 0); //이동
    glRotatef(cubeTransform.theta, 0, 0, 1);             //회전
    glScalef(cubeTransform.sx, cubeTransform.sy, 1);     //스케일
    //색상 결정 : 선택된 물체는 노란색, 기본은 흰색
    if (selectedObject == CUBE_ID) {
        glColor3f(1, 1, 0);
    }    else {
        glColor3f(1, 1, 1);
    }
    glutWireCube(1.0);
    glPopMatrix();

    //Teapot
    glPushMatrix();
    glTranslatef(teapotTransform.tx, teapotTransform.ty, 0);
    glRotatef(teapotTransform.theta, 0, 0, 1);
    glScalef(teapotTransform.sx, teapotTransform.sy, 1);
    if (selectedObject == TEAPOT_ID) {
        glColor3f(1, 1, 0);
    }    else {
        glColor3f(1, 1, 1);
    }
    glutSolidTeapot(0.3);
    glPopMatrix();

    glFlush();
}

int main(int argc, char** argv){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB);
    glutInitWindowSize(720, 480);
    glutInitWindowPosition(10, 10);
    glutCreateWindow("yjArm");

    //초기 위치 설정
    cubeTransform.tx = 0.8f;
    cubeTransform.ty = 0.5f;
    teapotTransform.tx = -0.8f;
    teapotTransform.ty = 0.5f;

    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyboard);
    yjinit();
    glutDisplayFunc(display);
    glutMainLoop();

    return 0;
}