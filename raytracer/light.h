//
//  Framework for a raytracer
//  File: light.h
//
//  Created for the Computer Science course "Introduction Computer Graphics"
//  taught at the University of Groningen by Tobias Isenberg.
//
//  Authors:
//    Maarten Everts
//    Jasper van de Gronde
//    adjustment of Hit class by Olivier Wavrin
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

#ifndef LIGHT_H_PG2BAJRA
#define LIGHT_H_PG2BAJRA

#include <iostream>
#include <limits>
#include "triple.h"

class Light
{
public:
    Light(Point pos,Color c) : position(pos), color(c)
    { }

    Point position;
    Color color;
};

// Forward declaration (as object.h is dependant of light.h)
class Object;

class Ray
{
public:
    static const int maxReflections = 2;
    Point O;
    Vector D;
    int reflection;
    Object* origin;

    Ray(const Point &from, const Vector &dir, int reflection = 0, Object* origin = NULL)
        : O(from), D(dir), reflection(reflection), origin(origin)
    {
		if (reflection == maxReflections)
			throw string("Too many reflections");
	}

    Point at(double t) const
    { return O + t*D; }

};

class Hit
{
public:
    double t;
    Vector N;
    bool no_hit;
    
    Hit(const double t, const Vector &normal, bool nohit = false)
        : t(t), N(normal), no_hit(nohit)
    { }

    static const Hit NO_HIT() { static Hit no_hit(std::numeric_limits<double>::quiet_NaN(),Vector(std::numeric_limits<double>::quiet_NaN(),std::numeric_limits<double>::quiet_NaN(),std::numeric_limits<double>::quiet_NaN()), true); return no_hit; }

};

#endif /* end of include guard: LIGHT_H_PG2BAJRA */
