//
//  GraphicBar.cpp
//  3DGraphMap_Project
//
//  Created by Antonio Ruiz del Moral on 5/8/18.
//  Copyright © 2018 Jessica M Cavazos Erhard. All rights reserved.
//

#include "GraphicBar.h"
#include <stdio.h>
#include <stdlib.h>
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#include <math.h>
#include <sys/time.h>
#include <random>

#include "imgui.h"
#include "imgui_impl_glut.h"

GraphicBar::GraphicBar(void){
    xPos = 0.0f;
    yPos= 0.0f;
    height = 2.0f;
    color[0] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    color[1] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    color[2] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    color[3] = 0.0f;
}

GraphicBar::GraphicBar(float x, float y, float h){
    xPos = x;
    yPos= y;
    height = h;
    color[0] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    color[1] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    color[2] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    color[3] = 0.0f;
    
}

GraphicBar::~GraphicBar(void){
}

// x = latitude and y = longitude
void GraphicBar::SetPos(float latitude, float longitude){
    xPos = latitude;
    yPos = longitude;
}

void GraphicBar::SetHeight(float h){
    height = h ;
}

float GraphicBar::GetPosX(){
    
    return xPos;
}

float GraphicBar::GetPosY(){
    return yPos;
}

float GraphicBar::GetHeight(){
    return height;
}

float GraphicBar::GetColorR(){
    return color[0];
}
float GraphicBar::GetColorG(){
    return color[1];
}
float GraphicBar::GetColorB(){
    return color[2];
}
float GraphicBar::GetColorA(){
    return color[3];
}


