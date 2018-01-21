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
#include "plane.h"
#include "material.h"
#include "light.h"
#include "image.h"
#include "yaml/yaml.h"
#include "glm.h"
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
    try
    { node["opacity"] >> m->opacity;}
	catch (...)
	{ m->opacity = 1.0;}
	try
	{ node["eta"] >> m->eta;}
	catch (...)
	{ m->eta=1.0; }
	try
	{
		std::string s ;
		node["texture"] >> s;
		m->texture = new Image(s.c_str());
    }
	catch (...)
	{ m->texture=NULL; }
    node["color"] >> m->color;	
    node["ka"] >> m->ka;
    node["kd"] >> m->kd;
    node["ks"] >> m->ks;
    node["n"] >> m->n;
    return m;
}

vector<Object*> Raytracer::parseObject(const YAML::Node& node)
{
    std::string objectType;
    node["type"] >> objectType;
    vector<Object*> objs;

    if (objectType == "sphere")
    {
        Point pos;
        node["position"] >> pos;
        double r;
        node["radius"] >> r;
        
        Vector up;
        try
		{ node["up"] >> up;}
		catch (...)
		{ up = Vector(0,0,1); }
		
		double spin;
		try
		{ node["spin"] >> spin;}
		catch (...)
		{ spin=0.0; }
		
		Sphere *sphere = new Sphere(pos,r,up,spin);
        sphere->material = parseMaterial(node["material"]);
        objs.push_back(sphere);
    }
    else if(objectType == "triangle")
    {
        Point p0, p1, p2;
        node["point1"] >> p0;
        node["point2"] >> p1;
        node["point3"] >> p2;
        Triangle* triangle = new Triangle(p0, p1, p2);
        triangle->material = parseMaterial(node["material"]);
        objs.push_back(triangle);
    }
    
    else if(objectType == "plane")
    {
        Point p;
        node["position"] >> p;
        Vector N;
        node["normal"] >> N;
        Plane* plane = new Plane(p, N);
        plane->material = parseMaterial(node["material"]);
        objs.push_back(plane);
    }
    
    else if(objectType == "cylinder")
    {
        Point p0, p1;
        node["point1"] >> p0;
        node["point2"] >> p1;
        double r;
        node["radius"] >> r;
        Cylinder* cylinder = new Cylinder(p0, p1, r);
        cylinder->material = parseMaterial(node["material"]);
        objs.push_back(cylinder);
    }
    else if(objectType == "model")
    {
        // Reading model parameters
        GLMmodel* model; 
        Point p;
        double size;
        string fileName;
        node["file"] >> fileName;
        model = glmReadOBJ(&fileName[0u]);

        // Sets the position to (0, 0, 0) and scale it to fit into a 1x1x1 cube
        //double factor = (double) glmUnitize(model);
        try // Set a position other than (0, 0, 0) if wanted
        {
            node["position"] >> p;
            model->position[0] = p.x;
            model->position[1] = p.y;
            model->position[2] = p.z;
        }
        catch(YAML::TypedKeyNotFound<std::string>) {}
        try // Scale it to the wanted size
        {
            node["size"] >> size;
            glmScale(model, size);
        }
        catch(YAML::TypedKeyNotFound<std::string>) // Give back its original size
        {
        //    glmScale(model, (float)(1.0 / factor));
        }

        Material* material = nullptr;
        bool uniformMaterial;
        try // Same material for all triangles
        {
            material = parseMaterial(node["material"]);
            uniformMaterial = true;
        }
        catch(YAML::TypedKeyNotFound<std::string>)
        {
            uniformMaterial = false;
        }

        // Converting model into triangles
        GLMgroup* group = model->groups;
        while(group != nullptr)
        {
            if(!uniformMaterial) // Material is specific to group
            {
                material = new Material();
                material->color.r = (double) model->materials[group->material].emmissive[0];
                material->color.g = (double) model->materials[group->material].emmissive[1];
                material->color.b = (double) model->materials[group->material].emmissive[2];
                // Since we have implemented ka, kd and ks as intensities and not RGB components,
                // we average the intensities from the specified RGB components.
                material->ka = (double) (
                    model->materials[group->material].ambient[0]
                    + model->materials[group->material].ambient[1]
                    + model->materials[group->material].ambient[2] ) / 3.0;
                material->kd = (double) (
                    model->materials[group->material].diffuse[0]
                    + model->materials[group->material].diffuse[1]
                    + model->materials[group->material].diffuse[2] ) / 3.0;
                material->ks = (double) (
                    model->materials[group->material].specular[0]
                    + model->materials[group->material].specular[1]
                    + model->materials[group->material].specular[2] ) / 3.0;
                material->n = (double) model->materials[group->material].shininess;
            }

            for(unsigned int i = 0; i < group->numtriangles; i++)
            {
                GLMtriangle* triangle = &model->triangles[group->triangles[i]];
                Triangle* newTriangle = new Triangle(Point(
                    (double) model->vertices[triangle->vindices[0]*3+0] + model->position[0],
                    (double) model->vertices[triangle->vindices[0]*3+1] + model->position[1],
                    (double) model->vertices[triangle->vindices[0]*3+2] + model->position[2]
                    ),  Point(
                    (double) model->vertices[triangle->vindices[1]*3+0] + model->position[0],
                    (double) model->vertices[triangle->vindices[1]*3+1] + model->position[1],
                    (double) model->vertices[triangle->vindices[1]*3+2] + model->position[2]
                    ), Point(
                    (double) model->vertices[triangle->vindices[2]*3+0] + model->position[0],
                    (double) model->vertices[triangle->vindices[2]*3+1] + model->position[1],   
                    (double) model->vertices[triangle->vindices[2]*3+2] + model->position[2]
                    ));
                newTriangle->material = material;
                objs.push_back(newTriangle);
            }
            group = group->next;
        }

        // Deleting model
        glmDelete(model);
    }

    return objs;
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
			else if (renderMode == "gooch")
            {
                const YAML::Node& gooch = doc["GoochParameters"];
                scene->setRenderMode(Scene::gooch);
                scene->setB(gooch["b"]);
                scene->setY(gooch["y"]);
                scene->setAlpha(gooch["alpha"]);
                scene->setBeta(gooch["beta"]);
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

            // Read the optional progression printing parameter
            try
            { scene->setPrintProgression(doc["PrintProgression"]); }
            catch (YAML::TypedKeyNotFound<std::string>)
            { scene->setPrintProgression(0.0f); }

            // Read camera configuration
            try
            { 
                // Old camera "Eye" : has default parameters
                Triple eye = doc["Eye"];
                scene->setEye(parseTriple(doc["Eye"]));
                scene->setLookAt(Vector(eye.x, eye.y, eye.z - 1.0));
                scene->setUpVector(Vector(0.0, 22.6198649, 0.0));
                scene->setWidth(400);
                scene->setHeight(400);
            }
            catch(YAML::TypedKeyNotFound<std::string>)
            {
                try
                    {
                    const YAML::Node& camera = doc["Camera"];
                    scene->setEye(parseTriple(camera["position"]));
                    scene->setLookAt(parseTriple(camera["lookat"]));
                    scene->setUpVector(parseTriple(camera["up"]));
                    scene->setWidth(camera["resolution"][0]);
                    scene->setHeight(camera["resolution"][1]);
                }
                catch(YAML::TypedKeyNotFound<std::string> error)
                {
                    cerr << "Error: expected a camera or an eye definition." << endl;
                    return false;
                }
            }
			try
            { scene->setsuperSamplingMult(doc["SuperSampling"]); }
            catch (YAML::TypedKeyNotFound<std::string>)
            { scene->setsuperSamplingMult(1); }
            
            try
            { scene->setEnableDepthOfField(doc["DepthOfField"]); }
            catch (YAML::TypedKeyNotFound<std::string>)
            { scene->setEnableDepthOfField(false); }
            
            try
            { scene->setApertureDiameter(doc["ApertureDiameter"]); }
            catch (YAML::TypedKeyNotFound<std::string>)
            { scene->setApertureDiameter(1.0); }
            
            try
            { scene->setFocalLength(doc["FocalLength"]); }
            catch (YAML::TypedKeyNotFound<std::string>)
            { scene->setFocalLength(0.5); }
            
            try
            { scene->setFocusDistance(doc["FocusDistance"]); }
            catch (YAML::TypedKeyNotFound<std::string>)
            { scene->setFocusDistance(50); }


            // Read and parse the scene objects
            const YAML::Node& sceneObjects = doc["Objects"];
            if (sceneObjects.GetType() != YAML::CT_SEQUENCE) {
                cerr << "Error: expected a sequence of objects." << endl;
                return false;
            }
            for(YAML::Iterator it=sceneObjects.begin();it!=sceneObjects.end();++it) {
                vector<Object*> objs = parseObject(*it);
                // Only add object if it is recognized
                if (objs.size() > 0) {
                    scene->addObject(objs);
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
