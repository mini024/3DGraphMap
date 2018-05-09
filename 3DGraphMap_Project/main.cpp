//
//  main.cpp
//  3DGrpahMap_Project
//
//  Created by Jessica M Cavazos Erhard on 4/11/18.
//  Copyright © 2018
//  Jessica M Cavazos Erhard.
//  José Antonio Ruiz del Moral Cervantes
//  All rights reserved.
//
//

#include <stdio.h>
#include <stdlib.h>
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#include <math.h>
#include <sys/time.h>
#include <vector>
#include "Planet.h"

//#include "imgui.h"
//#include "imgui_impl_glut.h"

using namespace std;

//void DoGUI();

// GLobal Variables
float diffuseColor[] = {1.0f, 0.0f, 0.0f, 1.0f};
float lightDirection[] = {2.0f, 2.0f, 2.0f, 1.0f};
float eyePos[] = {-11.0f, 8.0f, -7.0f};
float target[] = {0.0f, 0.0f, 0.0f};
bool rotateModel = false;
float attenC = 1.0f;
float attenL = 0.0f;
float attenQ = 0.0f;

bool planeVisible = true;
float planeScale[] = {10.0f, 1.0f, 10.0f};
float planeTranslate[] = {0.0f, 0.0f, 0.0f};

bool sphereVisible = true;
float sphereScale[] = {1.0f, 1.0f, 1.0f};
float sphereTranslate[] = {0.0f, 3.0f, 0.0f};

// angle of rotation for the camera direction
float angle = 0.0f;
// actual vector representing the camera's direction
float lx=0.0f,lz=-1.0f,ly=1.0f;
// XZY position of the camera
float x=0.0f, z=5.0f, y= 0.0f;
// the key states. These variables will be zero
//when no key is being presses
float deltaAngle = 0.0f;
float deltaMove = 0;
float deltaMoveY = 0;

// variable to store planet (global)
Planet earth;

void changeSize(int w, int h) {
    
    // Prevent a divide by zero, when window is too short
    // (you cant make a window of zero width).
    if (h == 0)
        h = 1;
    float ratio =  w * 1.0 / h;
    
    // Use the Projection Matrix
    glMatrixMode(GL_PROJECTION);
    
    // Reset Matrix
    glLoadIdentity();
    
    // Set the viewport to be the entire window
    glViewport(0, 0, w, h);
    
    // Set the correct perspective.
    gluPerspective(45.0f, ratio, 0.1f, 100.0f);
    
    // Get Back to the Modelview
    glMatrixMode(GL_MODELVIEW);
}

void drawCube(float x, float y, float z)
{
    const float sizex = 0.2f;
    const float sizey = 0.2f;
    const float sizez = 15.5f;
    
    glTranslatef(-x, -y, -z);
    
    glBegin(GL_QUADS);
    
    glColor3f(1.0, 1.0, 0.0);
    
    // FRONT
    glVertex3f(-sizex, -sizey, sizez);
    glVertex3f(sizex, -sizey, sizez);
    glVertex3f(sizex, sizey, sizez);
    glVertex3f(-sizex, sizey, sizez);
    
    // BACK
    glVertex3f(-sizex, -sizey, -sizez);
    glVertex3f(-sizex, sizey, -sizez);
    glVertex3f(sizex, sizey, -sizez);
    glVertex3f(sizex, -sizey, -sizez);
    
    glColor3f(0.0, 1.0, 0.0);
    
    // LEFT
    glVertex3f(-sizex, -sizey, sizez);
    glVertex3f(-sizex, sizey, sizez);
    glVertex3f(-sizex, sizey, -sizez);
    glVertex3f(-sizex, -sizey, -sizez);
    
    // RIGHT
    glVertex3f(sizex, -sizey, -sizez);
    glVertex3f(sizex, sizey, -sizez);
    glVertex3f(sizex, sizey, sizez);
    glVertex3f(sizex, -sizey, sizez);
    
    glColor3f(0.0, 0.0, 1.0);
    
    // TOP
    glVertex3f(-sizex, sizey, sizez);
    glVertex3f(sizex, sizey, sizez);
    glVertex3f(sizex, sizey, -sizez);
    glVertex3f(-sizex, sizey, -sizez);
    
    // BOTTOM
    glVertex3f(-sizex, -sizey, sizez);
    glVertex3f(-sizex, -sizey, -sizez);
    glVertex3f(sizex, -sizey, -sizez);
    glVertex3f(sizex, -sizey, sizez);
    
    glEnd();
    
    glTranslatef(x, y, z);
}

void getPointFromLatandLong(float latitude,float longitude ,char city){
    
    float X,Y,Z;
    latitude = latitude * M_PI / 180;
    longitude = longitude * M_PI / 180;
    
    int f  = 0;// flattening
    int alt = 1;
    float ls = atan((1 - f)*2 * tan(latitude));// lambda
    
    X = 10.0 * cos(ls) * cos(longitude) + alt * cos(latitude) * cos(longitude);
    Y = 10.0 * cos(ls) * sin(longitude) + alt *  cos(latitude) * sin(longitude);
    Z = 10.0 * sin(ls) + alt * sin(latitude);
    
    switch (city) {
        case '1':
            drawCube(X, Y-13.2, Z); //dubai
            break;
        case '2':
            drawCube(X, Y-12.2, Z); //moscow
            break;
        case '3':
            drawCube(X-1.3, Y-13.2, Z); //seoul
            break;
        case '4':
            drawCube(X+1.1, Y-14.6, Z); //singapore
            break;

        default:
            break;
    }
    
    //drawCube(X+1.1, Y-14.6, Z);
    
}

void computePos(float deltaMove) {
    
    x += deltaMove * lx * 0.1f;
    z += deltaMove * lz * 0.1f;
}

void computePosY(float deltaMove) {
    y += deltaMove * ly * 0.1f;
    z += deltaMove * lz * 0.1f;
}

void computeDir(float deltaAngle) {
    angle += deltaAngle;
    lx = sin(angle);
    lz = -cos(angle);
}

//void DoGUI(){
//    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoResize |
//    ImGuiWindowFlags_NoCollapse |
//    ImGuiWindowFlags_NoMove;
//    glDisable(GL_LIGHTING);
//    ImGui_ImplGLUT_NewFrame((int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y, 1.0f / 60.0f);
//
//    static bool showLightConfig = true;
//    static bool showAppConfig = true;
//    static bool firstRun = true;
//
//    if (firstRun) {
//        ImGui::SetNextWindowPos(ImVec2(10, 10));
//        ImGui::SetNextWindowSize(ImVec2(330,120));
//    }
//    ImGui::Begin("Camera", &showLightConfig, window_flags);
//    ImGui::DragFloat3("Eye Position", eyePos, 0.25f);
//    ImGui::DragFloat3("Target", target, 0.25f);
//    ImGui::Checkbox("Rotate View", &rotateModel);
//    ImGui::End();
//
//    if (firstRun) {
//        ImGui::SetNextWindowPos(ImVec2(670,10));
//        ImGui::SetNextWindowSize(ImVec2(120,80));
//    }
//    ImGui::Begin("Application", &showAppConfig, window_flags);
//
//    if (ImGui::Button("Quit")) {
//        exit(0);
//    }
//    if (ImGui::Button("Reset")) {
//        exit(0);
//    }
//    ImGui::End();
//
//
//    glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
//    ImGui::Render();
//    glEnable(GL_LIGHTING);
//    firstRun = false;
//}

void renderScene(void) {
    
    if (deltaMove)
        computePos(deltaMove);
    if (deltaMoveY)
        computePosY(deltaMoveY);
    if (deltaAngle)
        computePos(deltaMove);
        computeDir(deltaAngle);
    
    // Clear Color and Depth Buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Reset transformations
    glLoadIdentity();
    // Set the camera
    gluLookAt(x, 1.0f, z,
              x+lx, 10.0f,  z+lz,
              0.0f, 1.0f,  0.0f);
    glRotatef(90, 1.0, 0.0, 0.0);
    
    // position update
    earth.x0=  0.0;
    earth.y0=  0.0;
    earth.z0=  -30.0;
    
    // add this to render loop
    earth.draw(); // draws the planet
    //earth.t+=lz + 1; // just rotate planet by 2.5 deg each frame...
    
    getPointFromLatandLong(37.566667,126.966667,'3'); //Seoul
    getPointFromLatandLong(55.75,37.616667,'2'); //Moscow
    getPointFromLatandLong(25.263056,55.297222,'1'); //Dubai
    getPointFromLatandLong(1.283333,103.833333,'4'); //Singapore
    
     //   getPointFromLatandLong(10.480556,-66.903611); //Caracas
    //    getPointFromLatandLong(-15.793889,-47.882778); //Brasilia
    //    getPointFromLatandLong(38.833333,-104.816667); //Colorado spgs
    //    getPointFromLatandLong(19.433333,-99.133333); //CDMX (jala como colorado)

    
    
    // DoGUI();
    glutSwapBuffers();
}


void pressKey(int key, int xx, int yy) {
    
    switch (key) {
        case GLUT_KEY_LEFT : deltaAngle = -0.01f; break;
        case GLUT_KEY_RIGHT : deltaAngle = 0.01f; break;
        case GLUT_KEY_UP : deltaMove = 0.5f; break;
        case GLUT_KEY_DOWN : deltaMove = -0.5f; break;
        case 'a': deltaMoveY = -0.5f; break;
    }
}

void releaseKey(int key, int x, int y) {
    
    switch (key) {
        case GLUT_KEY_LEFT :
        case GLUT_KEY_RIGHT : deltaAngle = 0.0f;break;
        case GLUT_KEY_UP :
        case GLUT_KEY_DOWN : deltaMove = 0;break;
        case 'a':deltaMoveY = 0;break;
    }
}

//Global Variables
static int glutWindowId = 0;

int main(int argc, char **argv) {
    // init GLUT and create window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100,100);
    glutInitWindowSize(800,600);
    glutCreateWindow("Lighthouse3D - GLUT Tutorial");
    
    glutWindowId = glutGetWindow();
    
    // register callbacks
    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);
    glutIdleFunc(renderScene);
    
    glutSpecialFunc(pressKey);
    
    // here are the new entries
    glutIgnoreKeyRepeat(1);
    glutSpecialUpFunc(releaseKey);
    
    // OpenGL init
    glEnable(GL_DEPTH_TEST);
    
    // init after OpenGL initialisation
    earth.init(10.0,"/Users/alfonso/Google Drive/ITC 8vo Semestre/Graficas Computacionales/3DGraphMap/3DGraphMap_Project/world_map.bmp");
    
    // enter GLUT event processing cycle
    glutMainLoop();
    
    return 1;
}


