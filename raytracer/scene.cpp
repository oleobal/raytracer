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

                // Check if another object does not cover the light ray
                bool computeLights = true;
                for (unsigned int i = 0; i < objects.size(); i++)
                {
                    if(objects[i] != obj)
                    {
                        Hit cover(objects[i]->intersect(Ray(hit, L)));
                        if (cover.t != std::numeric_limits<double>::infinity()
                            && cover.t < min_hit.t)
                        {
                            computeLights = false;
                            break;
                        }
                    }
                }

                if(computeLights)
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

			Ray reflRay = Ray(hit, refl, ray.reflection+1, obj);
			reflection = trace(reflRay, recursionDepth+1);
			
			// Refraction/transparency
			Color refraction = Color(0,0,0); ;
			if (material->opacity < 1.0)
			{
				//shoot new ray according to eta (refraction indice)
				//Vector n = N.normalized();
				//Vector refl = ray.D -  2 * (ray.D.dot(n)) * n;
				
				try
				{
					Vector refr = getRefracted(ray.D, N.normalized(), 1, material->eta);
					Ray refrRay = Ray(hit, refr, ray.reflection+1, obj);
					refraction = trace(refrRay, recursionDepth+1);
				}
				catch (...)
				{ }
			}

            // Returning all components together with their coefficients applied.
            // The ambient component is added, and both the ambient and diffuse
            // components are affected by the material color.
            return
            material->opacity * ((material->ka + diffuse * material->kd) * material->color)
            + (1-material->opacity) * refraction
            + (specular + reflection) * material->ks;
        }
    }
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
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
			Color col = Color(0.0,0.0,0.0);
			for (int sx = 0 ; sx < superSamplingMult ; sx++)
			{
				for (int sy = 0 ; sy < superSamplingMult ; sy++)
				{
					Point pixel(x+s+sx*s, h-1-y+s+sy*s, 0);
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


Vector Scene::getRefracted(Vector in, Vector normal, double eta1, double eta2)
{
	Vector res;
	double root = 1.0 - (((eta1*eta1)*(1.0 - (in.dot(normal)*in.dot(normal)))))/  (eta2*eta2);

	if (root < 0)
		throw "Oops";
		
	res = ((eta1*(in - normal*(in.dot(normal))))/eta2) - (normal*sqrt(root));
		
	return res;
}
