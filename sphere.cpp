//
//  Framework for a raytracer
//  File: sphere.cpp
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

#include "sphere.h"
#include <iostream>
#include <math.h>

/************************** Sphere **********************************/

Hit Sphere::intersect(const Ray &ray)
{
    /****************************************************
    * RT1.1: INTERSECTION CALCULATION
    *
    * Given: ray, position, r
    * Sought: intersects? if true: *t
    * 
    * Insert calculation of ray/sphere intersection here. 
    *
    * You have the sphere's center (C) and radius (r) as well as
    * the ray's origin (ray.O) and direction (ray.D).
    *
    * If the ray does not intersect the sphere, return Hit::NO_HIT().
    * Otherwise, return an instance of Hit() with the distance of the
    * intersection point from the ray origin as t and the normal ad N (see example).
    ****************************************************/

    // The equation of the points on our ray is the following:
    //     x = ray.O + ray.D * t
    // The equation of the points on a sphere is the following:
    //     ||x - C||² = r²
    // We are looking for a point on both, which gives the equation:
    //     ||ray.O + ray.D * t - C||² = r²
    // <=> ||OC + ray.D * t||² = r²
    // <=> t² + 2t*(ray.D . OC) + ||OC||² - r² = 0
    // This is a quadratic equation of the form:
    // at² + bt + c = 0
    // Where a = 1, b = 2(ray.D . OC) and c = ||OC||² - r².
    //
    // Checking if delta >= 0 will tell us whether there is an intersection:
    //     delta >= 0
    // <=> b² - 4ac >= 0
    // <=> 4(ray.D . OC)² - 4(||OC||² - r²) >= 0
    // <=> (ray.D . OC)² - ||OC||² - r² >= 0
    // (since we removed the 4 factor, we compute in reality delta/4, so we
    // have to add a 4 factor when using delta for computing the solution)
    //
    // If delta >= 0, we then have the solution:
    //     t = (-b - sqrt(4*delta)) / 2a
    // <=> t = (-b - 2*sqrt(delta)) / 2a
    // <=> t = (-2(ray.D . OC) - 2sqrt(delta)) / 2
    // <=> t = - ray.D . OC - sqrt(delta)


    // Vector from the ray's origin to the sphere's center
    Vector OC = ray.O - position;

    // Dot product of the ray's direction and OC, used two times
    double dotProduct = ray.D.dot(OC);

    // Delta of the quadratic equation defined aboved
    double delta = pow(dotProduct, 2) - OC.length_2() + pow(r, 2);

    // If delta < 0, there is no intersection
    if (delta < 0)
        return Hit::NO_HIT();

    /****************************************************
    * RT1.2: NORMAL CALCULATION
    *
    * Given: t, C, r
    * Sought: N
    * 
    * Insert calculation of the sphere's normal at the intersection point.
    ****************************************************/

    // If delta > 0, then there is one or two solutions.
    // With two possible solutions, we always pick the intersection which is
    // the closest to the ray's origin since we want to draw only that point:
    // we always take the smallest t (hence the -sqrt()).
    double t = -dotProduct - sqrt(delta);

    // The normal vector at the intersection point on a sphere is simply
    // the normalized vector between the center of the sphere and the
    // intersection point.
    Vector N = (ray.at(t) - position).normalized();

    return Hit(t,N);
}
