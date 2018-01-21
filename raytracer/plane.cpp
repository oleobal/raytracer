//
//  Framework for a raytracer
//  File: plane.cpp
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

#include "plane.h"

Hit Plane::intersect(const Ray &ray)
{
	const double EPSILON = 0.0000001;
	
	double d = ray.D.dot(N);
	if (d > EPSILON)
	{
		Vector v(p.x - ray.O.x, p.y - ray.O.y, p.z - ray.O.z);
		double t = v.dot(N) / d ;
		return Hit(t,N);
	}

    return Hit::NO_HIT();
}

// doesn't really make much sense since a plane has no volume but whatever
bool Plane::hasWithin(Point p)
{
	//TODO/FIXME
	return false;
}
