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

#include "imgui.h"
#include "imgui_impl_glut.h"

void DoGUI();

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

// Image
GLuint texture;
GLUquadric *textureObj;

//VAO-Vertex Array Object
//VBO-Vertex Buffer Object
//EBO-Element Buffer Object
unsigned int VBO;
unsigned int VAO;
unsigned int EBO;

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
    const float sizex = 0.5f;
    const float sizey = 0.5f;
    const float sizez = 0.5f;
    
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

GLuint loadTexture( const char * filename)
{
    // Data read from the header of the BMP file
    unsigned char header[54]; // Each BMP file begins by a 54-bytes header
    unsigned int dataPos;     // Position in the file where the actual data begins
    unsigned int width, height;
    unsigned int imageSize;   // = width*height*3
    // Actual RGB data
    unsigned char * data;
    
    // Open the file
    FILE * file = fopen(filename,"rb");
    if (!file){printf("Image could not be opened\n"); return 0;}
    
    if ( fread(header, 1, 54, file)!=54 ){ // If not 54 bytes read : problem
        printf("Not a correct BMP file\n");
        return false;
    }
    
    if ( header[0]!='B' || header[1]!='M' ){
        printf("Not a correct BMP file\n");
        return 0;
    }
    
    // Read ints from the byte array
    dataPos    = *(int*)&(header[0x0A]);
    imageSize  = *(int*)&(header[0x22]);
    width      = *(int*)&(header[0x12]);
    height     = *(int*)&(header[0x16]);
    
    // Some BMP files are misformatted, guess missing information
    if (imageSize==0)    imageSize=width*height*3; // 3 : one byte for each Red, Green and Blue component
    if (dataPos==0)      dataPos=54; // The BMP header is done that way
    
    // Create a buffer
    data = new unsigned char [imageSize];
    
    // Read the actual data from the file into the buffer
    fread(data,1,imageSize,file);
    
    //Everything is in memory now, the file can be closed
    fclose(file);
    
    // Create one OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);
    
    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    // Give the image to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
    
    // When MAGnifying the image (no bigger mipmap available), use LINEAR filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // When MINifying the image, use a LINEAR blend of two mipmaps, each filtered LINEARLY too
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // Generate mipmaps, by the way.
    glGenerateMipmap(GL_TEXTURE_2D);
    
    return textureID;
}

void DoGUI(){
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoResize |
    ImGuiWindowFlags_NoCollapse |
    ImGuiWindowFlags_NoMove;
    glDisable(GL_LIGHTING);
    ImGui_ImplGLUT_NewFrame((int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y, 1.0f / 60.0f);
    
    static bool showLightConfig = true;
    static bool showAppConfig = true;
    static bool firstRun = true;
    
    if (firstRun) {
        ImGui::SetNextWindowPos(ImVec2(10, 10));
        ImGui::SetNextWindowSize(ImVec2(330,120));
    }
    ImGui::Begin("Camera", &showLightConfig, window_flags);
    ImGui::DragFloat3("Eye Position", eyePos, 0.25f);
    ImGui::DragFloat3("Target", target, 0.25f);
    ImGui::Checkbox("Rotate View", &rotateModel);
    ImGui::End();
    
    if (firstRun) {
        ImGui::SetNextWindowPos(ImVec2(670,10));
        ImGui::SetNextWindowSize(ImVec2(120,80));
    }
    ImGui::Begin("Application", &showAppConfig, window_flags);
    
    if (ImGui::Button("Quit")) {
        exit(0);
    }
    if (ImGui::Button("Reset")) {
        exit(0);
    }
    ImGui::End();
    
    
    glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
    ImGui::Render();
    glEnable(GL_LIGHTING);
    firstRun = false;
}

void renderScene(void) {
    
    if (deltaMove)
        computePos(deltaMove);
    if (deltaMoveY)
        computePosY(deltaMoveY);
    if (deltaAngle)
        computeDir(deltaAngle);
    
    // Clear Color and Depth Buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Reset transformations
    glLoadIdentity();
    // Set the camera
    gluLookAt(    x, 1.0f, z,
              x+lx, 10.0f,  z+lz,
              0.0f, 1.0f,  0.0f);
    glRotatef(90, 1.0, 0.0, 0.0);

    // init after OpenGL initialisation
    earth.init(10.0,"/Users/jessicamcavazoserhard/Documents/ITC/8 semestre/Graficas/OpenGL/3DGraphMap_Project/3DGraphMap_Project/world_map.bmp");

    // position update
    earth.x0=  0.0;
    earth.y0=  0.0;
    earth.z0= -30.0;

    // add this to render loop
    earth.draw(); // draws the planet
    earth.t+=0.5; // just rotate planet by 2.5 deg each frame...

    // Draw Cubes
    for(int i = -3; i < 3; i++)
        for(int j=-3; j < 3; j++) {
            glPushMatrix();
            glTranslatef(i*10.0,0,j * 10.0);
            drawCube(0.0,0.0,0.0);
            glPopMatrix();
        }
    
    //DoGUI();
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
    
    // enter GLUT event processing cycle
    glutMainLoop();
    
    return 1;
}


