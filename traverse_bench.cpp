#define TINYOBJLOADER_IMPLEMENTATION 
#include<stdio.h>
#include<iomanip>
#include<chrono>
#include <iostream>
#include<gperftools/profiler.h>

//https://qiita.com/tyanmahou/items/8497d6e815ebf7ea90c6

#include "tiny_obj_loader.h"

#include "Scene.h"
#include "Sphere.h"
#include "Plane.h"
#include "Mesh.h"
//#include "glout.h"
#include "BVH.h"

int main(int argc, char **argv){
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

	//std::cout << attrib.vertices.size() << std::endl;

	Mesh mesh(attrib,shapes,Material(FColor(b,b,a)));

	Vec3 MAX = Vec3(mesh.bvh.nodes[1].aabb_max[0],
				mesh.bvh.nodes[1].aabb_max[1],
				mesh.bvh.nodes[1].aabb_max[2]);
	Vec3 MIN = Vec3(mesh.bvh.nodes[1].aabb_min[0],
				mesh.bvh.nodes[1].aabb_min[1],
				mesh.bvh.nodes[1].aabb_min[2]);
	Vec3 d = MAX - MIN;
	Vec3 c = d * 0.5 + MIN;
	Vec3 ss[101][101];
	Vec3 ds[101][101];

	for(int i = 0;i < 101;i++){
		for(int j = 0;j < 101;j++){
			Vec3 s = MIN + Vec3((MAX.x - MIN.x) * (i / 100.0),(MAX.y - MIN.y) * (j / 100.0),0.0);
			ss[i][j] = s;
			ds[i][j] = c - s;
		}
	}

	std::cout << "MIN  " << MIN << std::endl;
	std::cout << "MAX  " << MAX << std::endl;
	std::cout << "CEN" << c << std::endl;


	int count = 0;
	for(int i = 0;i < 101;i++){
		for(int j = 0;j < 101;j++){
			Intersection_point *inter = mesh.get_intersection(Ray(ss[i][j],ds[i][j]));
			if(inter != nullptr)count++;
		}
	}
	ProfilerStart("/tmp/traverse.prof");
	for(int i = 0;i < 101;i++){
		for(int j = 0;j < 101;j++){
			mesh.get_intersection(Ray(ss[i][j],ds[i][j]));
		}
	}
	ProfilerStop();

	double ti = 0;
	for(int ccc = 0 ;ccc < 100;ccc++){
	std::chrono::system_clock::time_point start,end;
    start = std::chrono::system_clock::now();
	for(int i = 0;i < 101;i++){
		for(int j = 0;j < 101;j++){
			mesh.get_intersection(Ray(ss[i][j],ds[i][j]));
		}
	}
	end = std::chrono::system_clock::now();

    auto elapsed = std::chrono::duration_cast< std::chrono::milliseconds >(end - start).count();
    std::cout << elapsed <<"ms"<< std::endl;
	ti += elapsed;
	}
	std::cout << "ave " << ti / 100.0 << "ms" << std::endl;

	int ii = 76,jj = 48;
	Intersection_point* inter =	mesh.get_intersection(Ray(ss[ii][jj],ds[ii][jj]));
	
	std::cout << "衝突回数 " << count << std::endl;
	std::cout << "origin" << std::setprecision(10) << ss[ii][jj] << std::endl;
	std::cout << "direction" << std::setprecision(10) << ds[ii][jj].normalized() << std::endl;
	
	if(nullptr != inter)std::cout << std::setprecision(10) << inter->position << std::endl;
	delete inter;
    /*for(int i = 0;i < 3;i++){
        printf("%lf ",bvh.nodes[bvh.root].aabb_max[i]);
    }
    printf("\n");
    for(int i = 0;i < 3;i++){
        printf("%lf ",bvh.nodes[bvh.root].aabb_min[i]);
    }
    printf("\n");*/
	//Scene scene(600,600);
    
	//scene.add(&mesh);
	//drawgl(argc,argv,scene);
	
	return 0;
}
