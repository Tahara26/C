/*
 * File:   CMath.c
 * Author: JustinT
 *
 * Created on January 16, 2018, 12:40 PM
 */


#include "xc.h"
#include "CMath.h"
#include "BOARD.h"
#define PI 3.14159265359

float myabs(float x)
{
    if (x<0){
        x = x*-1;
    }
    return x;
}

float enorm(float px, float py, float qx, float qy) {
    float dx, dy, g, e, s, t, r;
    dx = myabs(qx-px);
    dy = myabs(qy-py);
    if (dx>dy) {
        g = dx;
        e = dy;
    }
    else {
        g = dy;
        e = dx;
    }
    
    int i;
    for (i=0; i<2; i++) {
        t = (e/g);
        r = (t*t);
        s = (r/(4.0+r));
        g = g + 2 * s * g;
        e = e * s;
    }
    return g;
}

float arctangent2(float y, float x) {
    float z, r, phi;
    z = myabs(y);
    if (x>0) {
        r = ((x-z)/(x+z));
        phi = 0.1963*(r*r*r) - 0.9817*r + (PI/4);
    }
    else {
        r = ((x+z)/(x-z));
        phi = 0.1963*(r*r*r) - 0.9817*r + (3/4)*PI;
    }
    
    if (y<0) {
        phi = -1* phi;
    }
    return phi;
}

