//
//  Framework for a raytracer
//  File: triangle.cpp
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

#include "triangle.h"
#include <math.h>

/************************** Sphere **********************************/

Hit Triangle::intersect(const Ray &ray)
{
    const double EPSILON = 0.0000001;
    Vector edge1, edge2, h, s, q;
    double a,f,u,v;
    edge1 = p2 - p1;
    edge2 = p3 - p1;
    h = ray.D.cross(edge2);
    a = edge1.dot(h);
    if (a > -EPSILON && a < EPSILON)
        return Hit::NO_HIT();

    f = 1/a;
    s = ray.O - p1;
    u = f * (s.dot(h));
    if (u < 0.0 || u > 1.0)
        return Hit::NO_HIT();

    q = s.cross(edge1);
    v = f * ray.D.dot(q);
    if (v < 0.0 || u + v > 1.0)
        return Hit::NO_HIT();

    double t = f * edge2.dot(q);
    if (t <= EPSILON)
        return Hit::NO_HIT();

    Vector N = (ray.at(t) - p1).cross(ray.at(t) - p2).normalized();

    return Hit(t,N);
}
