//
//  Framework for a raytracer
//  File: raytracer.cpp
//
//  Created for the Computer Science course "Introduction Computer Graphics"
//  taught at the University of Groningen by Tobias Isenberg.
//
//  Author: Maarten Everts
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

#include "raytracer.h"
#include "object.h"
#include "sphere.h"
#include "triangle.h"
#include "cylinder.h"
#include "material.h"
#include "light.h"
#include "image.h"
#include "yaml/yaml.h"
#include <ctype.h>
#include <fstream>
#include <assert.h>

// Functions to ease reading from YAML input
void operator >> (const YAML::Node& node, Triple& t);
Triple parseTriple(const YAML::Node& node);

void operator >> (const YAML::Node& node, Triple& t)
{
    assert(node.size()==3);
    node[0] >> t.x;
    node[1] >> t.y;
    node[2] >> t.z;
}

Triple parseTriple(const YAML::Node& node)
{
    Triple t;
    node[0] >> t.x;
    node[1] >> t.y;
    node[2] >> t.z;	
    return t;
}

Material* Raytracer::parseMaterial(const YAML::Node& node)
{
    Material *m = new Material();
    node["color"] >> m->color;	
    node["ka"] >> m->ka;
    node["kd"] >> m->kd;
    node["ks"] >> m->ks;
    node["n"] >> m->n;
    return m;
}

Object* Raytracer::parseObject(const YAML::Node& node)
{
    Object *returnObject = NULL;
    std::string objectType;
    node["type"] >> objectType;

    if (objectType == "sphere")
    {
        Point pos;
        node["position"] >> pos;
        double r;
        node["radius"] >> r;
        Sphere *sphere = new Sphere(pos,r);		
        returnObject = sphere;
    }
    else if(objectType == "triangle")
    {
        Point p0, p1, p2;
        node["point1"] >> p0;
        node["point2"] >> p1;
        node["point3"] >> p2;
        Triangle* triangle = new Triangle(p0, p1, p2);
        returnObject = triangle;
    }
    else if(objectType == "cylinder")
    {
        Point p0, p1;
        node["point1"] >> p0;
        node["point2"] >> p1;
        double r;
        node["radius"] >> r;
        Cylinder* cylinder = new Cylinder(p0, p1, r);
        returnObject = cylinder;
    }

    if (returnObject)
    {
        // read the material and attach to object
        returnObject->material = parseMaterial(node["material"]);
    }

    return returnObject;
}

Light* Raytracer::parseLight(const YAML::Node& node)
{
    Point position;
    node["position"] >> position;
    Color color;
    node["color"] >> color;
    return new Light(position,color);
}

/*
* Read a scene from file
*/

bool Raytracer::readScene(const std::string& inputFilename)
{
    // Initialize a new scene
    scene = new Scene();

    // Open file stream for reading and have the YAML module parse it
    std::ifstream fin(inputFilename.c_str());
    if (!fin) {
        cerr << "Error: unable to open " << inputFilename << " for reading." << endl;;
        return false;
    }
    try {
        YAML::Parser parser(fin);
        if (parser) {
            YAML::Node doc;
            parser.GetNextDocument(doc);

            std::string renderMode;
            try
            { doc["RenderMode"] >> renderMode; }
            catch (YAML::TypedKeyNotFound<std::string>)
            { renderMode = "phong"; }
			
			if (renderMode == "normal")
				scene->setRenderMode(Scene::normal);
            else if (renderMode == "zbuffer")
            {
				scene->setRenderMode(Scene::zbuffer);
                double near, far ;
                doc["nearClippingPlane"] >> near;
                doc["farClippingPlane"] >> far;
                if(near >= far)
                {
                    std::cerr << "Error: far clipping plane distance should be stricly greater than near clipping distance." << std::endl;
                    return false;
                }
                scene->setNearClippingDistance(near);
                scene->setFarClippingDistance(far);
            }
			
            // Read whether shadows should be used or not
            try
            { scene->setEnableShadows(doc["Shadows"]); }
            catch (YAML::TypedKeyNotFound<std::string>)
            { scene->setEnableShadows(false); }
            
            // Read the number of recursions for reflexions
            try
            { scene->setMaxRecursionDepth(doc["MaxRecursionDepth"]); }
            catch (YAML::TypedKeyNotFound<std::string>)
            { scene->setMaxRecursionDepth(0); }

            // Read camera configuration
            try
            { 
                // Old camera "Eye" : has default parameters
                Triple eye = doc["Eye"];
                scene->setEye(parseTriple(doc["Eye"]));
                scene->setLookAt(Vector(eye.x, eye.y, eye.z + 1.0));
                scene->setUpVector(Vector(0.0, 1.0, 0.0));
                scene->setWidth(400);
                scene->setHeight(400);
            }
            catch(YAML::TypedKeyNotFound<std::string>)
            {
                const YAML::Node& camera = doc["Camera"];
                scene->setEye(parseTriple(camera["position"]);
                scene->setLookAt(parseTriple(camera["lookat"]));
                scene->setUpVector(parseTriple(camera["up"]));
                scene->setWidth(camera["resolution"][0]);
                scene->setHeight(camera["resolution"][1]);
            }
			try
            { scene->setsuperSamplingMult(doc["SuperSampling"]); }
            catch (YAML::TypedKeyNotFound<std::string>)
            { scene->setsuperSamplingMult(1); }

            // Read and parse the scene objects
            const YAML::Node& sceneObjects = doc["Objects"];
            if (sceneObjects.GetType() != YAML::CT_SEQUENCE) {
                cerr << "Error: expected a sequence of objects." << endl;
                return false;
            }
            for(YAML::Iterator it=sceneObjects.begin();it!=sceneObjects.end();++it) {
                Object *obj = parseObject(*it);
                // Only add object if it is recognized
                if (obj) {
                    scene->addObject(obj);
                } else {
                    cerr << "Warning: found object of unknown type, ignored." << endl;
                }
            }

            // Read and parse light definitions
            const YAML::Node& sceneLights = doc["Lights"];
            if (sceneObjects.GetType() != YAML::CT_SEQUENCE) {
                cerr << "Error: expected a sequence of lights." << endl;
                return false;
            }
            for(YAML::Iterator it=sceneLights.begin();it!=sceneLights.end();++it) {
                scene->addLight(parseLight(*it));
            }
        }
        if (parser) {
            cerr << "Warning: unexpected YAML document, ignored." << endl;
        }
    } catch(YAML::ParserException& e) {
        std::cerr << "Error at line " << e.mark.line + 1 << ", col " << e.mark.column + 1 << ": " << e.msg << std::endl;
        return false;
    }

    cout << "YAML parsing results: " << scene->getNumObjects() << " objects read." << endl;
    return true;
}

void Raytracer::renderToFile(const std::string& outputFilename)
{
    Image img(scene->getWidth(), scene->getHeight());
    cout << "Tracing..." << endl;
    scene->render(img);
    cout << "Writing image to " << outputFilename << "..." << endl;
    img.write_png(outputFilename.c_str());
    cout << "Done." << endl;
}
