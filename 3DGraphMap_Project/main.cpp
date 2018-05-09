//
//  main.cpp
//  3DGrpahMap_Project
//
//  Created by Jessica M Cavazos Erhard on 4/11/18.
//  Copyright © 2018
//  Jessica M Cavazos Erhard.
//  José Antonio Ruiz del Moral Cervantes
//  Luis Alfonso Arriaga Quezada
//  All rights reserved.
//
//

#include <stdio.h>
#include <stdlib.h>
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#include <math.h>
#include <sys/time.h>
#include "vector"
#include "Planet.h"

#include "imgui.h"
#include "imgui_impl_glut.h"

#include "GraphicBar.h"

#define WINFOW_TITLE    "3D BarCharts Over Image"
#define WINDOW_WIDTH    800
#define WINDOW_HEIGHT   600
#define UPDATE_TIME     17 /* ((1 / 60) * 1000) rounded up */
//#define X .525731112119133606
//#define Z .850650808352039932
#define MAX_POINTS  50

using namespace std;

//void DoGUI();

// GLobal Variables
float diffuseColor[] = {1.0f, 0.0f, 0.0f, 1.0f};
float lightDirection[] = {2.0f, 2.0f, 2.0f, 1.0f};
float eyePos[] = {0.0f, 8.0f, 5.0f};
float target[] = {0.0f, 1.0f, -1.0f};

//Main Function
void Initialize();
void Shutdown();
void DoGUI();
void LoadTexture();
void renderScene(); //display();
void updateTimer(int windowId);
void drawCube(float x, float y, float z, float h);
void reshape(int width, int height); //windowSize();
void computePos(float deltaMove);
void computePosY(float deltaMove);
void computeDir(float deltaAngle);
void createGraphicBars();
void renderEarth();
void getPointFromLatandLong(float latitude,float longitude ,char city);
double GetMilliseconds();
void Normalize(GLfloat *a);
void releaseKey(int key, int x, int y);
void pressKey(int key, int xx, int yy);

//Interaction
void keyDown(unsigned char key, int x, int y);
void keyUp(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void mouseDrag(int x, int y);
void mouseMove(int x, int y);

// Camera
void glPerspective(float fov, float aspectRatio, float znear, float zfar);
void reshapeOrtho(int w, int h);
void reshapePerspective(int w, int h);
void (*activeReshape)(int,int) = reshapePerspective;

//Static Global Variables
static double prevTime = 0.0;
static float rotation = 270.0f;
static int glutWindowId = 0;

//Global Variables
GraphicBar graphics[MAX_POINTS] = {};
Planet earth; // variable to store planet (global)
//float diffuseColor[] = {1.0f, 0.0f, 0.0f, 1.0f};
//float lightDirection[] = {2.0f, 2.0f, 2.0f, 1.0f};
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

// the key states. These variables will be zero
//when no key is being presses
float deltaAngle = 0.0f;
float deltaMove = 0;
float deltaMoveY = 0;

int main(int argc, char **argv) {
    // init GLUT and create window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100,100);
    glutInitWindowSize(WINDOW_WIDTH,WINDOW_HEIGHT);
    glutCreateWindow(WINFOW_TITLE);
    reshape(WINDOW_WIDTH, WINDOW_HEIGHT);
    
    glutWindowId = glutGetWindow();

    // register callbacks
    glutDisplayFunc(renderScene);
    glutReshapeFunc(reshape);
    glutIdleFunc(renderScene);
    
    glutSpecialFunc(pressKey);
    glutIgnoreKeyRepeat(1);
    glutSpecialUpFunc(releaseKey);
    
//    glutKeyboardFunc(keyDown);
//    glutKeyboardUpFunc(keyUp);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseDrag);
    
    // here are the new entries
    glutIgnoreKeyRepeat(false);
    glutTimerFunc(UPDATE_TIME, updateTimer, glutGetWindow());
    
    Initialize();
    atexit(Shutdown);
    prevTime = GetMilliseconds();

    // init after OpenGL initialisation
    earth.init(10.0,"/Users/alfonso/Google Drive/ITC 8vo Semestre/Graficas Computacionales/3DGraphMap/3DGraphMap_Project/world_map.bmp");
    
    glutMainLoop();
    
    return 0;
}

void Shutdown() {
    ImGui_ImplGLUT_Shutdown();
}

void Initialize() {
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    
    glDisable(GL_COLOR_MATERIAL);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);
    
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    
    glPolygonMode(GL_FRONT, GL_FILL);
    glPolygonMode(GL_BACK, GL_FILL);
    
    //glEnable(GL_LIGHTING);
    //glEnable(GL_LIGHT0);
    
    const float noColor[] = {0.0f, 0.0f, 0.0f, 1.0f};
    
    //glLightfv(GL_LIGHT0, GL_AMBIENT, noColor);
    //glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseColor);
    //glLightfv(GL_LIGHT0, GL_SPECULAR, diffuseColor);
    //glLightfv(GL_LIGHT0, GL_POSITION, lightDirection);
    
    const float ambientColor[] = {0.2f, 0.2f, 0.2f, 1.0f};
    const float globalDiffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};
    const float materialShine = 0.0f;
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientColor);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, globalDiffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, noColor);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noColor);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, materialShine);
    
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    
    //glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, attenC);
   // glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, attenL);
   // glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, attenQ);
    
    ImGui_ImplGLUT_Init();
    
    // position update
    earth.x0=  0.0;
    earth.y0=  0.0;
    earth.z0=  -30.0;
    
}

void renderScene(void) {
    // Move view
    if (deltaMove)
        computePos(deltaMove);
    if (deltaMoveY)
        computePosY(deltaMoveY);
    if (deltaAngle)
        computePos(deltaMove);
    computeDir(deltaAngle);
    
    // Clear Color and Depth Buffers
    glClearColor(0.07f, 0.63f, 0.88f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    glPushMatrix();
    
    // Reset transformations
    glLoadIdentity();
    // Set the camera
    gluLookAt(eyePos[0], 1.0f, eyePos[2],
              eyePos[0]+target[0], -10.0f,  eyePos[2]+target[2],
              0.0f, 1.0f,  0.0f);
    glRotatef(90, 1.0, 0.0, 0.0);
    
    // EARTH - position update
    earth.x0=  0.0;
    earth.y0=  0.0;
    earth.z0=  -30.0;
    
    // add this to render loop
    earth.draw(); // draws the planet
    earth.t +=target[0];
    
//    getPointFromLatandLong(37.566667,126.966667,'3'); //Seoul
//    getPointFromLatandLong(55.75,37.616667,'2'); //Moscow
    getPointFromLatandLong(25.263056,55.297222,'1'); //Dubai
//    getPointFromLatandLong(1.283333,103.833333,'4'); //Singapore
    
    //glEnable(GL_LIGHTING);
    if(sphereVisible){
        
        for (int i=0; i<10; i++){
            graphics[i].SetHeight(i+2.0f);
        }
        
        graphics[0].SetPos(-3.0f,-6.0f);
        graphics[1].SetPos(-3.0f,-3.0f);
        graphics[2].SetPos(-3.0f,-1.0f);
        graphics[3].SetPos(-3.0f,2.0f);
        graphics[4].SetPos(-3.0f,4.0f);
        graphics[5].SetPos(-3.0f,6.0f);
        
        graphics[6].SetPos(-2.0f,-3.0f);
        graphics[7].SetPos(-2.0f,-2.0f);
        graphics[8].SetPos(-2.0f,-1.0f);
        graphics[9].SetPos(-2.0f,0.0f);
        graphics[10].SetPos(-2.0f,1.0f);
        graphics[11].SetPos(-2.0f,2.0f);
        
        graphics[12].SetPos(-1.0f,-3.0f);
        graphics[13].SetPos(-1.0f,-2.0f);
        graphics[14].SetPos(-1.0f,-1.0f);
        graphics[15].SetPos(-1.0f,0.0f);
        graphics[16].SetPos(-1.0f,1.0f);
        graphics[17].SetPos(-1.0f,2.0f);
        
        graphics[18].SetPos(0.0f,-3.0f);
        graphics[19].SetPos(0.0f,-2.0f);
        graphics[20].SetPos(0.0f,-1.0f);
        graphics[21].SetPos(0.0f,0.0f);
        graphics[22].SetPos(0.0f,1.0f);
        graphics[23].SetPos(0.0f,2.0f);
        
        graphics[24].SetPos(1.0f,-3.0f);
        graphics[25].SetPos(1.0f,-2.0f);
        graphics[26].SetPos(1.0f,-1.0f);
        graphics[27].SetPos(1.0f,0.0f);
        graphics[28].SetPos(1.0f,1.0f);
        graphics[29].SetPos(1.0f,2.0f);
        
        graphics[30].SetPos(2.0f,-3.0f);
        graphics[31].SetPos(2.0f,-2.0f);
        graphics[32].SetPos(2.0f,-1.0f);
        graphics[33].SetPos(2.0f,0.0f);
        graphics[34].SetPos(2.0f,1.0f);
        graphics[35].SetPos(2.0f,2.0f);
        
//        for (int i=0; i<10; i++){
//            glPushMatrix();
//            glTranslatef(i*10.0, 0,0);
//            drawCube(graphics[i].GetPosX(), graphics[i].GetPosY(), 0.0f, graphics[i].GetHeight());
//            glPopMatrix();
//        }
    }
    
    //glDisable(GL_LIGHTING);
    
    glBegin(GL_LINES);
    glColor3f(1.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    //glVertex3f(lightDirection[0], lightDirection[1], lightDirection[2]);
    glEnd();
    
    //glDisable(GL_DEPTH_TEST);
    
    glBegin(GL_LINES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, 0.0f);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 1.0f);
    glEnd();
    
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_LIGHTING);
    
    if (deltaMove)
        computePos(deltaMove);
    if (deltaMoveY)
        computePosY(deltaMoveY);
    if (deltaAngle)
        computeDir(deltaAngle);
    
    DoGUI();
    glPopMatrix();
    glutSwapBuffers();
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
            drawCube(X-2, Y-9, Z, 15.5); //dubai
            break;
        case '2':
            drawCube(X, Y-12.2, Z, 15.5); //moscow
            break;
        case '3':
            drawCube(X-1.3, Y-13.2, Z, 15.5); //seoul
            break;
        case '4':
            drawCube(X+1.1, Y-14.6, Z, 15.5); //singapore
            break;
            
        default:
            break;
    }
    
    //drawCube(X+1.1, Y-14.6, Z, 2);
    
}

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

void drawCube(float x, float y, float z, float h)
{
    const float sizex = 0.2f;
    const float sizey = 0.2f;
    const float sizez = h;
    
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
    eyePos[0] += deltaMove * target[0] * 0.1f;
    eyePos[2] += deltaMove * target[2] * 0.1f;
}

void computePosY(float deltaMove) {
    eyePos[1] += deltaMove * target[1] * 0.1f;
    eyePos[2] += deltaMove * target[2] * 0.1f;
}

void computeDir(float deltaAngle) {
    angle += deltaAngle;
    target[0] = sin(angle);
    target[2] = -cos(angle);
}

void DoGUI(){
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoResize |
                                    ImGuiWindowFlags_NoCollapse |
                                    ImGuiWindowFlags_NoMove;
    
    ImGuiWindowFlags window_data_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    
    //glDisable(GL_LIGHTING);
    
    ImGui_ImplGLUT_NewFrame((int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y, 1.0f / 60.0f);
    
    static bool showCameraConfig = true;
    static bool firstRun = true;
    static bool showDataConfig = true;
    
    if (firstRun) {
        ImGui::SetNextWindowPos(ImVec2(10, 10));
        ImGui::SetNextWindowSize(ImVec2(330,120));
    }
    
    ImGui::Begin("Camera", &showCameraConfig, window_flags);
    ImGui::DragFloat3("Eye Position", eyePos, 0.25f);
    ImGui::DragFloat3("Target", target, 0.25f);
    ImGui::Checkbox("Rotate View", &rotateModel);
    ImGui::End();
    
    if (firstRun) {
        ImGui::SetNextWindowPos(ImVec2(WINDOW_WIDTH-510,5));
        ImGui::SetNextWindowSize(ImVec2(500,400));
    }
    //static float color[4] = { 0.4f,0.7f,0.0f,0.5f };
    
    ImGui::Begin("Dataset", &showDataConfig, window_data_flags);
    ImGui::Columns(4, "mycolumns"); // 4-ways, with border
    ImGui::Separator();
    ImGui::Text("Latitude"); ImGui::NextColumn();
    ImGui::Text("Longitude"); ImGui::NextColumn();
    ImGui::Text("Height"); ImGui::NextColumn();
    ImGui::Text("Color"); ImGui::NextColumn();
    ImGui::Separator();
    
    for (int i = 0; i < 20; i++)
    {
        float latitude = graphics[i].GetPosX();
        float longitude = graphics[i].GetPosY();
        float size = graphics[i].GetHeight();
        
        float color[4] = {graphics[i].GetColorR(), graphics[i].GetColorG(), graphics[i].GetColorB(), graphics[i].GetColorA()};
        
        ImGui::DragFloat("", &latitude); ImGui::NextColumn();
        ImGui::DragFloat("", &longitude); ImGui::NextColumn();
        ImGui::DragFloat("", &size); ImGui::NextColumn();
        ImGui::ColorEdit3("", color); ImGui::NextColumn();
    }
    ImGui::Separator();
    ImGui::End();
   
    glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
    ImGui::Render();
    //glEnable(GL_LIGHTING);
    firstRun = false;
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


void reshape(int width, int height) {
    ImGui::GetIO().DisplaySize.x = width;
    ImGui::GetIO().DisplaySize.y = height;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, width, height);
    activeReshape(width, height);
    glMatrixMode(GL_MODELVIEW);
}

void updateTimer(int windowId) {
    double curTime = GetMilliseconds();
    double deltaTime = (curTime - prevTime) * 0.001;
    
    if (rotateModel) {
        rotation += 60.0f * deltaTime;
        while (rotation > 360.0f) {
            rotation -= 360.0f;
        }
    }
    prevTime = curTime;
    glutTimerFunc(UPDATE_TIME, updateTimer, windowId);
    glutPostRedisplay();
}

void keyDown(unsigned char key, int x, int y) {
}

void keyUp(unsigned char key, int x, int y) {
    ImGuiIO& io = ImGui::GetIO();
    unsigned char keys[] = { key, '\0'};
    io.AddInputCharactersUTF8((const char*)keys);
    //io.AddInputCharacter(key);
    io.MousePos = ImVec2((float)x, (float)y);
}

void mouseDrag(int x, int y) {
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2((float)x, (float)y);
}

void mouseMove(int x, int y) {
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2((float)x, (float)y);
}

void mouse(int button, int state, int x, int y) {
    ImGuiIO& io = ImGui::GetIO();
    
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            io.MouseDown[0] = true;
        } else /* GLUT_UP */ {
            io.MouseDown[0] = false;
        }
    } else if (button == GLUT_RIGHT_BUTTON) {
        if (state == GLUT_DOWN) {
            io.MouseDown[1] = true;
        } else /* GLUT_UP */ {
            io.MouseDown[1] = false;
        }
    }
}

void reshapeOrtho(int w, int h) {
    glOrtho(0.0f, 1.0f, 1.0f, 0.0f, -100.0f, 100.0f);
}

void reshapePerspective(int w, int h) {
    glPerspective(60.0f, (float)w / (float)h, 0.01f, 1000.0f);
}

void glPerspective(float fov, float aspectRatio, float znear, float zfar) {
    float ymax = znear * tanf(fov * M_PI / 360.0f);
    float xmax = ymax * aspectRatio;
    glFrustum(-xmax, xmax, -ymax, ymax, znear, zfar);
}

double GetMilliseconds() {
#if !WIN32
    static timeval s_tTimeVal;
    gettimeofday(&s_tTimeVal, NULL);
    double time = s_tTimeVal.tv_sec * 1000.0; // sec to ms
    time += s_tTimeVal.tv_usec / 1000.0; // us to ms
#else
    double time = (double)GetTickCount() * 0.001;
#endif
    return time;
}

void Normalize(GLfloat *a) {
    GLfloat d=sqrt(a[0]*a[0]+a[1]*a[1]+a[2]*a[2]);
    a[0]/=d; a[1]/=d; a[2]/=d;
}
