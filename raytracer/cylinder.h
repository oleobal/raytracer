//
//  Framework for a raytracer
//  File: cylinder.h
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

#ifndef CYLINDER_H_FABIOUX_LEOBAL
#define CYLINDER_H_FABIOUX_LEOBAL

#include "object.h"

class Cylinder : public Object
{
public:
    Cylinder(Point p0, Point p1, double r) : p0(p0), p1(p1), r(r) {};

    virtual Hit intersect(const Ray &ray);

    const Point p0, p1;
    const double r;
};

#endif
