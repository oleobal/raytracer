//
//  Framework for a raytracer
//  File: scene.cpp
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

#include "scene.h"
#include "material.h"

Color Scene::trace(const Ray &ray, int recursionDepth)
{
	if (recursionDepth > maxRecursionDepth)
		return Color(0.0, 0.0, 0.0);
	
    // Find hit object and distance
    Hit min_hit(std::numeric_limits<double>::infinity(),Vector());
    Object *obj = NULL;
    for (unsigned int i = 0; i < objects.size(); ++i) {
        if(objects[i] != ray.origin)
        {
            Hit hit(objects[i]->intersect(ray));
            if (hit.t<min_hit.t) {
                min_hit = hit;
                obj = objects[i];
            }
        }
    }

    // No hit? Return background color.
    if (!obj) return Color(0.0, 0.0, 0.0);

    Material *material = obj->material;            //the hit objects material
    Point hit = ray.at(min_hit.t);                 //the hit point
    Vector N = min_hit.N;                          //the normal at hit point
    Vector V = -ray.D;                             //the view vector

    switch(renderMode)
    {
        case zbuffer: // Display zbuffer values as a grayscale
        {
            double color = (min_hit.t - nearClippingDistance)
                / (farClippingDistance - nearClippingDistance);
            if(color > 1)
                color = 0;
            else if(color < 0)
                color = 1;
            else
                color = 1 - color;
            return Color(color, color, color);
        }
        case normal: // Display normals components as RGB colors
        {
            return Color((N + Vector(1.0, 1.0, 1.0)) / 2.0);
        }
        case gooch: // Gooch illumination model
        {
            Color diffuse, specular, reflection;
            Color materialColor = obj->colorAt(hit);
            Color kCool = Color(0, 0, b) + alpha*materialColor;
            Color kWarm = Color(y, y, 0) + beta*materialColor;
            for(unsigned int i = 0; i < lights.size(); i++)
            { 
                // Light direction vector (from the hit point to the light)
                Vector L = (lights[i]->position - hit).normalized();

                // Computing per-light components of Gooch model
                if(!enableShadows || !checkShadow(obj, hit, min_hit, L))
                {
                    // Using the Gooch shading formula
                    diffuse += kCool *(1 - L.dot(N))/2 + kWarm * (1 + L.dot(N))/2;

                    // Specular per-light component: R.V^n
                    // Maximized when the viewer direction (V) is aligned with
                    // the light reflected on the surface (R).
                    // R is computed using the formula R = 2 * L.N * N - L.
                    // Reusing old angle variable calculated above as L.N.
                    double angle = (2 * L.dot(N) * N - L).normalized().dot(V);
                    if(angle > 0)
                        specular += pow(angle, material->n) * lights[i]->color;
                }

                // Reflections
                Vector n = N.normalized();
                Vector refl = ray.D -  2 * (ray.D.dot(n)) * n;

                Ray reflRay = Ray(hit, refl, obj);
                reflection = trace(reflRay, recursionDepth+1);
            }

            return diffuse * material->kd
                + (specular + reflection) * material->ks;
        }
        default: // Phong rendering
        {
            // Computing of the color using the Phong reflection model:
            // https://en.wikipedia.org/wiki/Phong_reflection_model

            // Computing per-light factors of Phong model components
            // (diffuse and specular).
            Color diffuse, specular, reflection;
            for(unsigned int i = 0; i < lights.size(); i++)
            {   
                // Light direction vector (from the hit point to the light)
                Vector L = (lights[i]->position - hit).normalized();

                if(!enableShadows || !checkShadow(obj, hit, min_hit, L))
                {
                    // Diffuse per-light component: L.N
                    // Maximized when the light direction (L) is aligned with
                    // the normal vector of the surface (N).
                    double angle = L.dot(N);
                    if(angle > 0)
                        diffuse += angle * lights[i]->color;

                    // Specular per-light component: R.V^n
                    // Maximized when the viewer direction (V) is aligned with
                    // the light reflected on the surface (R).
                    // R is computed using the formula R = 2 * L.N * N - L.
                    // Reusing old angle variable calculated above as L.N.
                    angle = (2 * angle * N - L).normalized().dot(V);
                    if(angle > 0)
                        specular += pow(angle, material->n) * lights[i]->color;
                }
            }

            // Reflections
			Vector n = N.normalized();
			Vector refl = ray.D -  2 * (ray.D.dot(n)) * n;

			Ray reflRay = Ray(hit, refl, obj);
			reflection = trace(reflRay, recursionDepth+1);
			
			
			// Refraction/transparency
			Color refraction = Color(0,0,0); ;
			if (material->opacity < 1.0)
			{
				try
				{
					double etaFrom = 1, etaOut=1;
					// where are we coming from ?
					if (ray.parent != NULL)
						etaFrom = ray.parent->eta;
					
					
					// N can be pointing towards or away from us,
					// could be used for determining whether we are entering or exiting
					// but only for convex objects
					Vector refr;
					if (N.dot(ray.D) >=0) // hitting from inside
					{
						// we are getting out, yes, but are we still inside some other object ?
						// the last one defined overwrites the rest
						Object* container = getObjectsContaining(hit, obj).back();
						if (container != NULL) 
							etaOut = container->material->eta;
						refr = getRefracted(ray.D, N.normalized(), etaFrom, etaOut);
					}
					else // hitting from outside
					{
						etaOut = material->eta;
						refr = getRefracted(-ray.D, N.normalized(), etaFrom, etaOut);
					}
					
					Ray refrRay = Ray(hit, refr, obj, &ray, etaOut);
					refraction = trace(refrRay, recursionDepth+1);
				}
				catch (...)
				{ }
			}
			

            // Returning all components together with their coefficients applied.
            // The ambient component is added, and both the ambient and diffuse
            // components are affected by the material color.
            return
            material->opacity * ((material->ka + diffuse * material->kd) * obj->colorAt(hit))
            + (1-material->opacity) * refraction
            + (specular + reflection) * material->ks;
        }
    }
}

// Checks if an object is blocking the light to obj.
bool Scene::checkShadow(const Object* obj, const Point& hit, const Hit& min_hit, const Vector& L)
{
    for (unsigned int i = 0; i < objects.size(); i++)
    {
        if(objects[i] != obj)
        {
            Hit cover(objects[i]->intersect(Ray(hit, L)));
            if (cover.t != std::numeric_limits<double>::infinity()
                && cover.t < min_hit.t)
                return true;
        }
    }
    return false;
}

/**
 * superSamplingMult : size of grid of points for each pixel
 * ex : 2 -> 2*2 = 4 samples per pixel
 * (default should be 1, ie disabled)
 */
void Scene::render(Image &img)
{
	double s = 1.0/(superSamplingMult+1);
	
    int w = img.width();
    int h = img.height();

    // Up and right vector for the screen coordinates in 3D space
    Vector up = upVector.normalized();
    Vector right = up.cross(lookAt-eye).normalized();

    // We always keep the same pixel to unit ratio (1 pixel per unit) and make
    // the focal distance vary according to the horizontal FOV (lenght of the
    // up vector):
    //     fov = arctan(w / 2 / f) * 180 / pi * 2
    // <=> f = w / 2 / tan(fov / 180 * pi / 2)
    double focalDistance = w / 2 / tan(upVector.length() / 180.0 * M_PI / 2);

    // Center of the screen in 3D space
    Point center = (lookAt-eye).normalized() * focalDistance + eye;

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
			Color col = Color(0.0,0.0,0.0);
			for (int sx = 0 ; sx < superSamplingMult ; sx++)
			{
				for (int sy = 0 ; sy < superSamplingMult ; sy++)
				{
                    Point pixel = right * (w / 2 - (x+s+sx*s))
                        + up * (h / 2 - (y+s+sy*s))
                        + center;

                    Ray ray(eye, (pixel-eye).normalized());
					Color colbuf = trace(ray);
					col += (colbuf);
				}
			}
			col = col / (superSamplingMult*superSamplingMult);
			col.clamp();
            img(x,y) = col;
        }
    }
}

void Scene::addObject(Object *o)
{
    objects.push_back(o);
}

void Scene::addLight(Light *l)
{
    lights.push_back(l);
}

void Scene::setEye(Triple e)
{
    eye = e;
}


/**
 * returns a list of object that contain the point p
 * 
 * if excepted is given, it won't be included in the list
 */
std::vector<Object*> Scene::getObjectsContaining(Point p, Object* excepted = NULL)
{
	std::vector<Object*> result = std::vector<Object*>();
	for (unsigned int i = 0 ; i< objects.size() ; i++)
	{
		if (objects[i]->hasWithin(p))
		{
			if (objects[i] != excepted)
				result.push_back(objects[i]);
		}
	}
	return result;
}

Vector Scene::getRefracted(Vector in, Vector normal, double eta1, double eta2)
{
	Vector res;
	double root = 1.0 - (((eta1*eta1)*(1.0 - (in.dot(normal)*in.dot(normal)))))/  (eta2*eta2);

	if (root < 0)
		throw "Oops";
		
	res = ((eta1*(in - normal*(in.dot(normal))))/eta2) - (normal*sqrt(root));
		
	return res;
}
