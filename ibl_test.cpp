#define TINYOBJLOADER_IMPLEMENTATION 
#include<stdio.h>
#include<iomanip>
#include<chrono>
#include <iostream>
#include<gperftools/profiler.h>

//https://qiita.com/tyanmahou/items/8497d6e815ebf7ea90c6

//#include "tiny_obj_loader.h"

#include "Scene.h"
#include "Sphere.h"
#include "Plane.h"
#include "Mesh.h"
#include "glout.h"
#include "BVH.h"

int main(int argc, char **argv){
	R a = 0.85;
	R b = 0.10;

	if(argc < 2){
		printf("objファイルを指定してー\n");
		return 0;
	}

	/*std::string inputfile = argv[1];
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
	}*/

	//std::cout << attrib.vertices.size() << std::endl;

	//Mesh mesh(attrib,shapes,Material(FColor(240.0 / 255,210.0 / 255,37.0 / 255),MT_PERFECT_REF/*FColor(a,a,a)*/));
	Mesh mesh(argv[1],40,Vec3(70,20,50),Vec3(2432.0,9328.0,782.0),3.14);

	Scene scene(Camera(600,600,Vec3(-70,-5.0,52.0),Vec3(1.0,0.2,0.0),Vec3(0.0,1.0,1.0),50.0));
    
	/*inputfile = "/home/kai/programing/cpp/path_tracing/obj/plane.obj";


	ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, inputfile.c_str());
  
	if (!err.empty()) { // `err` may contain warning message.
 		std::cerr << err << std::endl;
	}

	if (!ret) {
  		exit(1);
	}


	scene.add(new Mesh(attrib,shapes,Material(FColor(a,a,a))));*/


	scene.add(&mesh);
	//scene.draw(1,32);
	drawgl(argc,argv,scene);
	
	return 0;
}
