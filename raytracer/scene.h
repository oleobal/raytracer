//
//  Framework for a raytracer
//  File: scene.h
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

#ifndef SCENE_H_KNBLQLP6
#define SCENE_H_KNBLQLP6

#include <vector>
#include "triple.h"
#include "light.h"
#include "object.h"
#include "image.h"

class Scene
{
public:
    enum RenderMode {
        phong, zbuffer, normal
    };

private:
    std::vector<Object*> objects;
    std::vector<Light*> lights;
    Triple eye;
    RenderMode renderMode;
    double nearClippingDistance;
    double farClippingDistance;
    bool enableShadows;
    int maxRecursionDepth;
    Triple lookAt;
    Triple upVector;
    int width;
    int height;

public:
    Color trace(const Ray &ray, int recursionDepth);
    Color trace(const Ray &ray);
    void render(Image &img);
    void addObject(Object *o);
    void addLight(Light *l);
    void setEye(Triple e);
    unsigned int getNumObjects() { return objects.size(); }
    unsigned int getNumLights() { return lights.size(); }

    void setRenderMode(RenderMode value) { renderMode = value; }
    void setNearClippingDistance(double value) { nearClippingDistance = value; }
    void setFarClippingDistance(double value) { farClippingDistance = value; }
    void setEnableShadows(bool value) { enableShadows = value; }
    void setMaxRecursionDepth(int value) {maxRecursionDepth = value; }
    void setLookAt(Triple value) { lookAt = value; }
    void setUpVector(Triple value) { upVector = value; }
    void setWidth(int value) { width = value; }
    void setHeight(int value) { height = value; }
    int getWidth() { return width; }
    int getHeight() { return height; }
};

#endif /* end of include guard: SCENE_H_KNBLQLP6 */
