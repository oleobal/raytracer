//
//  Framework for a raytracer
//  File: sphere.h
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

#ifndef SPHERE_H_115209AE
#define SPHERE_H_115209AE

#include "object.h"

class Sphere : public Object
{
public:
	Sphere (Point position,double r, Vector up, double spin) : position(position), r(r), thisWayUp(up), spin(spin) { }
    Sphere(Point position,double r) : position(position), r(r), thisWayUp(Vector(0,0,1)), spin(0.0) { }

    virtual Hit intersect(const Ray &ray);
    virtual bool hasWithin(Point p);

    const Point position;
    const double r;
    
    const Vector thisWayUp;
    const double spin; // with thisWayUp as axis
};

#endif /* end of include guard: SPHERE_H_115209AE */
