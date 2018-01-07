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
	Sphere (Point position, double r, Vector up, double spin) : position(position), r(r)
    {
        rotate(up, spin);
    }
    Sphere(Point position,double r) : position(position), r(r), phi(0), theta(0) { }

    virtual Hit intersect(const Ray &ray);
    virtual bool hasWithin(Point p);
    
    virtual Color colorAt(const Point& hit);

    // Rotate the sphere
    void rotate(const Vector& up, double spin);

    const Point position;
    const double r;

    // Sphere rotations
    double phi;
    double theta;
};

#endif /* end of include guard: SPHERE_H_115209AE */
