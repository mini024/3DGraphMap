//
//  GraphicBar.hpp
//  3DGraphMap_Project
//
//  Created by Antonio Ruiz del Moral on 5/8/18.
//  Copyright © 2018 Jessica M Cavazos Erhard. All rights reserved.
//

#ifndef GraphicBar_h
#define GraphicBar_h

#include <stdio.h>

#endif /* GraphicBar_h */


#pragma once
//#include "Vector2f.h"
//#include "Vector3f.h"

class GraphicBar
{
    
private:
    float xPos;
    float yPos;
    float height;
    float color[4];
    
public:
    GraphicBar(void);
    GraphicBar(float x, float y, float h);
    ~GraphicBar(void);
    
    void SetPos(float x, float y);
    void SetHeight(float height);
    float GetPosX();
    float GetPosY();
    float GetHeight();
    float GetColorR();
    float GetColorG();
    float GetColorB();
    float GetColorA();
};
