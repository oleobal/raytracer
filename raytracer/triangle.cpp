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

#define TRIANGLE_CCW

/************************** Triangle **********************************/


#ifdef TRIANGLE_CCW
Triangle::Triangle(Point p0, Point p1, Point p2) :
    p0(p0), p1(p1), p2(p2), N((p1 - p0).cross(p2 - p0).normalized()) {}
#else
Triangle::Triangle(Point p0, Point p1, Point p2) :
    p0(p0), p1(p1), p2(p2), N((p1 - p0).cross(p2 - p0).normalized() * -1) {}
#endif


Hit Triangle::intersect(const Ray &ray)
{
    // Using Möller-Trumbore intersection algorithm.
    // Reference: https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/moller-trumbore-ray-triangle-intersection

    // Used to avoid seeing a line when looking at the triangle from a very
    // small angle (ray direction almost parallel to triangle surface).
    const double EPSILON = 0.0000001;

    // Edges of the triangle: used as x and y axis of a 2D plane
    Vector p0p1 = p1 - p0;
    Vector p0p2 = p2 - p0;

    // Coordinates of intersection point in the triangle plane
    double u, v;

    // Unit vectors of the triangle plane
    Vector uvec, vvec;

    // Third unit vector for the ray - intersection point distance
    Vector tvec;

    // Determinant and its inverse
    double det, invDet;

    // Back-face culling
    uvec = ray.D.cross(p0p2);
    det = p0p1.dot(uvec);
#ifdef TRIANGLE_CCW
    if(det < EPSILON)
        return Hit::NO_HIT();
#else
    if(det < -1*EPSILON)
        return Hit::NO_HIT();
#endif

    // Checking if the u (x axis) coordinate is within the triangle bounds [0:1]
    invDet = 1 / det;
    tvec = ray.O - p0;
    u = tvec.dot(uvec) * invDet;
    if(u < 0.0 || u > 1.0)
        return Hit::NO_HIT();
    
    // Checking if the v (y axis) coordinate is within the triangle bounds [0:1]
    // This means we checked if the intersection point is within the quad formed
    // with the x and y axis of the triangle.
    // We then have to check that the point is inside the triangle (not on the
    // other side of the "hypotenuse"): u + v <= 1.
    vvec = tvec.cross(p0p1);
    v = ray.D.dot(vvec) * invDet;
    if (v < 0.0 || u + v > 1.0)
        return Hit::NO_HIT();

    // Computing distance from the ray's origin to the intersection point
    double t = p0p2.dot(vvec) * invDet;
    if (t <= EPSILON) // If we are almost "inside" the triangle
        return Hit::NO_HIT();

    // N (normal vector) has been calculated at initialization
    return Hit(t,N);
}

// doesn't really make much sense since a triangle has no volume but whatever
bool Triangle::hasWithin(Point p)
{
	//TODO/FIXME
	return false;
}
