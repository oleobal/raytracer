//
//  Framework for a raytracer
//  File: cylinder.cpp
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

#include "cylinder.h"
#include <math.h>


Hit Cylinder::intersect(const Ray &ray)
{
	// intersection with circular borders
	
	Vector axis = p1 - p0 ;
	
	Triple buf = ray.D - (ray.D.dot(axis)/axis.dot(axis)) * axis;
	Triple buf2 = (ray.O - p0) - ((ray.O - p0).dot(axis)/axis.dot(axis)) * axis;


	
	double a = buf.dot(buf);
	double b = b = 2* (buf.dot(buf2));
	double c = (buf2.dot(buf2)) - r*r;
	
	double delta = b*b - 4*a*c ;
	
	if (delta < 0.0000001)
		return Hit::NO_HIT();
	
	double x1 = (-b + sqrt(delta)) / (2*a);
	double x2 = (-b - sqrt(delta)) / (2*a);
	
	double rightSol = x1;
	double otherSol = x2;
	if (x1 < x2)
	{
		rightSol = x1;
		otherSol = x2;
	}
	else
	{
		rightSol = x2;
		otherSol = x1;
	}	
	
	
	// check whether the point is indeed inside the boundaries of the cylinder
	
	Vector z0 = Vector(p0 - ray.at(rightSol)).normalized();
	Vector z1 = Vector(p1 - ray.at(rightSol)).normalized();
	
	if (z0.dot(p0-p1) >= 0 && z1.dot(p1-p0) >= 0)
		return Hit(x1, (ray.O + ray.D*rightSol - p0).normalized());
	else
	{
		z0 = Vector(p0 - ray.at(otherSol)).normalized();
		z1 = Vector(p1 - ray.at(otherSol)).normalized();
		if (z0.dot(p0-p1) >= 0 && z1.dot(p1-p0) >= 0)
			return Hit(x1, (ray.O + ray.D*otherSol - p0).normalized());
		else
			return Hit::NO_HIT();
	}
}
