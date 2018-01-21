//
//  Framework for a raytracer
//  File: plane.h
//
//  Created for the Computer Science course "Introduction Computer Graphics"
//  taught at the University of Groningen by Tobias Isenberg.
//
//  Authors:
//    Maarten Everts
//    Jasper van de Gronde
//
//  Students:
//    Vincent Fabioux
//    Olivier Léobal
//
//
//  This framework is inspired by and uses code of the raytracer framework of 
//  Bert Freudenberg that can be found at
//  http://isgwww.cs.uni-magdeburg.de/graphik/lehre/cg2/projekt/rtprojekt.html 
//

#ifndef PLANE_H_FABIOUX_LEOBAL
#define PLANE_H_FABIOUX_LEOBAL

#include "object.h"

class Plane : public Object
{
public:
    Plane(Point p, Vector normal) : p(p), N(normal) {};

    virtual Hit intersect(const Ray &ray);
    virtual bool hasWithin(Point p);
    
    const Point p;
    const Vector N;
};

#endif
