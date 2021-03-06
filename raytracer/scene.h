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

#include <iomanip>
#include <iostream>
#include <math.h>
#include <vector>
#include "triple.h"
#include "light.h"
#include "object.h"
#include "image.h"

class Scene
{
public:
    enum RenderMode {
        phong, zbuffer, normal, gooch
    };

private:
    std::vector<Object*> objects;
    std::vector<Light*> lights;
    Triple eye;
    RenderMode renderMode;
    double nearClippingDistance;
    double farClippingDistance;
    bool enableShadows;
    bool enableDepthOfField;
    double apertureDiameter; // diameter of the entrance pupil
    double focalLength;
    double focusDistance;
    int maxRecursionDepth;
    Triple lookAt;
    Triple upVector;
    int width;
    int height;
    int superSamplingMult;
    float printProgression;
    float b;
    float y;
    float alpha;
    float beta;

public:
	/**
	 * *depth_p, if given, is filled with the depth at given pixel
	 */
    Color trace(const Ray &ray, int recursionDepth=0, double* depth_p=0);
    bool checkShadow(const Object* obj, const Point& hit, const Hit& min_hit, const Vector& L);
    void render(Image &img);
    void addObject(vector<Object*> o);
    void addLight(Light *l);
    void setEye(Triple e);
    std::vector<Object*> getObjectsContaining(Point p, Object* excepted);
    Vector getRefracted(Vector in, Vector normal, double eta1, double eta2);
    unsigned int getNumObjects() { return objects.size(); }
    unsigned int getNumLights() { return lights.size(); }

    void setRenderMode(RenderMode value) { renderMode = value; }
    void setNearClippingDistance(double value) { nearClippingDistance = value; }
    void setFarClippingDistance(double value) { farClippingDistance = value; }
    void setEnableShadows(bool value) { enableShadows = value; }
    void setEnableDepthOfField(bool value) { enableDepthOfField = value; }
    void setApertureDiameter(double value) {apertureDiameter = value; }
    void setFocalLength(double value) {focalLength = value; }
    void setFocusDistance(double value) {focusDistance = value; }
    void setMaxRecursionDepth(int value) {maxRecursionDepth = value; }
    void setLookAt(Triple value) { lookAt = value; }
    void setUpVector(Triple value) { upVector = value; }
    void setWidth(int value) { width = value; }
    void setHeight(int value) { height = value; }
    int getWidth() { return width; }
    int getHeight() { return height; }
    void setsuperSamplingMult(int value) { superSamplingMult = value; }
    void setPrintProgression(float value) { printProgression = value; }
    void setB(float value) { b = value; }
    void setY(float value) { y = value; }
    void setAlpha(float value) { alpha = value; }
    void setBeta(float value) { beta = value; }
};

#endif /* end of include guard: SCENE_H_KNBLQLP6 */
