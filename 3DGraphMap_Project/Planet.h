//
//  Planet.h
//  3DGraphMap_Project
//
//  Created by Jessica M Cavazos Erhard on 5/4/18.
//  Copyright © 2018 Jessica M Cavazos Erhard. All rights reserved.
//

#ifndef Planet_h
#define Planet_h

#include <stdio.h>
#include <stdlib.h>
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#include <math.h>
#include <sys/time.h>
#include <iostream>
#include <string>

//#include "bitmap_image.hpp"

using namespace std;

//---------------------------------------------------------------------------
const int nb=15;        // slices
const int na=nb<<1;     // points per equator
class Planet
{
public:
    bool _init;             // has been initiated ?
    GLfloat x0,y0,z0;       // center of planet [GCS]
    GLfloat pos[na][nb][3]; // vertex
    GLfloat nor[na][nb][3]; // normal
    GLfloat txr[na][nb][2]; // texcoord
    GLuint  txrid;          // texture id
    GLfloat t;              // dayly rotation angle [deg]
    Planet() { _init=false; txrid=0; x0=0.0; y0=0.0; z0=0.0; t=0.0; }
    ~Planet() { if (_init) glDeleteTextures(1,&txrid); }
    void init(GLfloat r,const char * texture);        // call after OpenGL is already working !!!
    void draw();
};

void Planet::init(GLfloat r,const char * texture)
{
    if (!_init) { _init=true; glGenTextures(1,&txrid); }
    
    GLfloat x,y,z,a,b,da,db;
    GLfloat tx0,tdx,ty0,tdy;// just correction if CLAMP_TO_EDGE is not available
    int ia,ib;
    
    // a,b to texture coordinate system
    tx0=0.0;
    ty0=0.5;
    tdx=0.5/M_PI;
    tdy=1.0/M_PI;
    
    // load texture to GPU memory
    if (texture!="")
    {
        char q;
        unsigned int *pp;
        int xs,ys,x,y,adr,*txr;
        union { unsigned int c32; char db[4]; } c;
        
        FILE * file;
        
        file = fopen( texture, "rb" );
        
        if ( file == NULL ) {
            printf("HEllo");
            return;
        }
        
        // seek through the bmp header, up to the width/height:
        fseek(file, 18, SEEK_CUR);
        
        xs = 1620;
        ys = 810;
        
        txr = new int[xs*ys*100];
        fseek(file, 24, SEEK_CUR);
        
        fread( txr, xs * ys * 100, 1, file );
        fclose(file);
        
        for(int i = 0; i < xs * ys; ++i)
        {
            int index = i*100;
            unsigned char B,R;
            B = txr[index];
            R = txr[index+2];

            txr[index] = R;
            txr[index+2] = B;
        }
        
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D,txrid);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_MODULATE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, xs, ys, 0, GL_RGBA, GL_UNSIGNED_BYTE, txr);
        glDisable(GL_TEXTURE_2D);
        //delete bmp;
        delete[] txr;
        
        // texture coordinates by 1 pixel from each edge (GL_CLAMP_TO_EDGE)
        tx0+=1.0/GLfloat(xs);
        ty0+=1.0/GLfloat(ys);
        tdx*=GLfloat(xs-2)/GLfloat(xs);
        tdy*=GLfloat(ys-2)/GLfloat(ys);
    }
    
    // correct texture coordinate system (invert x)
    tx0=1.0-tx0; tdx=-tdx;
    
    da=(2.0*M_PI)/GLfloat(na-1);
    db=     M_PI /GLfloat(nb-1);
    for (ib=0,b=-0.5*M_PI;ib<nb;ib++,b+=db) {
        for (ia=0,a= 0.0     ;ia<na;ia++,a+=da)
        {
            x=cos(b)*cos(a);
            y=cos(b)*sin(a);
            z=sin(b);
            nor[ia][ib][0]=x;
            nor[ia][ib][1]=y;
            nor[ia][ib][2]=z;
            pos[ia][ib][0]=r*x;
            pos[ia][ib][1]=r*y;
            pos[ia][ib][2]=r*z;
            txr[ia][ib][0]=tx0+(a*tdx);
            txr[ia][ib][1]=ty0+(b*tdy);
        }
    }
}
void Planet::draw()
{
    if (!_init) return;
    int ia,ib0,ib1;
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(x0,y0,z0);
    glRotatef(90.0,1.0,0.0,0.0); // rotate planets z axis (North) to OpenGL y axis (Up)
    glRotatef(-t,0.0,0.0,1.0); // rotate planets z axis (North) to OpenGL y axis (Up)

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,txrid);
    glColor3f(1.0,1.0,1.0);
    
    for (ib0=0,ib1=1;ib1<nb;ib0=ib1,ib1++)
    {
        glBegin(GL_QUAD_STRIP);
        for (ia=0;ia<na;ia++)
        {
            glNormal3fv  (nor[ia][ib0]);
            glTexCoord2fv(txr[ia][ib0]);
            glVertex3fv  (pos[ia][ib0]);
            
            glNormal3fv  (nor[ia][ib1]);
            glTexCoord2fv(txr[ia][ib1]);
            glVertex3fv  (pos[ia][ib1]);
        }
        glEnd();
    }
    glDisable(GL_TEXTURE_2D);
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

}
//---------------------------------------------------------------------------

#endif /* Planet_h */
