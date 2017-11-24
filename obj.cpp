#define TINYOBJLOADER_IMPLEMENTATION 
#include <iostream>

#include "tiny_obj_loader.h"

#include "Scene.h"
#include "Sphere.h"
#include "Plane.h"
#include "Mesh.h"
#include "glout.h"

int main(int argc, char **argv){
    Scene scene(200,200);
    
	R a = 0.85;
	R b = 0.10;

	if(argc < 2){
		printf("objファイルを指定してー\n");
		return 0;
	}

	std::string inputfile = argv[1];
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;


	std::string err;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, inputfile.c_str());
  
	if (!err.empty()) { // `err` may contain warning message.
 		std::cerr << err << std::endl;
	}

	if (!ret) {
  		exit(1);
	}

	std::cout << attrib.vertices.size() << std::endl;

	scene.add(new Mesh(attrib,shapes,Material(FColor(b,b,a))));
	//scene.add(new Mesh(attrib,shapes,Material(FColor(240.0 / 255,210.0 / 255,37.0 / 255),MT_PERFECT_REF)));
	//scene.add(new Mesh(attrib,shapes,Material(MT_REFRACTION)));
	
	//scene.add(new Sphere(Vec3(1e5+1,40.8,81.6),1e5,Material(FColor(a,b,b))));
	//scene.add(new Sphere(Vec3(-1e5+99,40.8,81.6),1e5,Material(FColor(b,a,b))));
	//scene.add(new Sphere(Vec3(50,40.8,1e5),1e5,Material(FColor(a,a,a))));
	//scene.add(new Sphere(Vec3(50,40.8,-1e5 + 250),1e5,Material(FColor(a,a,a))));
	//scene.add(new Sphere(Vec3(50,1e5,81.6),1e5,Material(FColor(a,a,a))));
    scene.add(new Plane(Vec3(0,1,0),Vec3(50,0,81.6),PL_CHECKERBOARD_CHECK));
	//scene.add(new Sphere(Vec3(50,-1e5 + 81.6,81.6),1e5,Material(FColor(a,a,a))));
	//scene.add(new Sphere(Vec3(65,20,20),20,Material(FColor(b,b,a))));
	//scene.add(new Sphere(Vec3(27,16.5,47),16.5,Material(FColor(240.0 / 255,210.0 / 255,37.0 / 255),MT_PERFECT_REF)));
	//scene.add(new Sphere(Vec3(77,16.5,78),16.5,Material(MT_REFRACTION)));
	scene.add(new Sphere(Vec3(50.0,90.0,81.6),15,Material(FColor(0.30,0.30,0.30),FColor(32,32,32))));

	drawgl(argc,argv,scene);
	//scene.draw(256,3);
	return 0;
}
