#include<stdio.h>
#include<iomanip>
#include<chrono>
#include <iostream>
#include<gperftools/profiler.h>

#include "../Scene/Scene.hpp"
#include "../Shape/Geom.hpp"
#include "../GL_out/glout.h"

int main(int argc, char **argv){

	if(argc < 2){
		printf("objファイルを指定してー\n");
		return 0;
	}

	Geom geom(argv[1],100.0,Vec3(0,0,0),Vec3(1,1,1),0);

	int root = geom.bvh.root;
	Vec3 MAX = Vec3(geom.bvh.aabbs[root].aabb[0][0],
					geom.bvh.aabbs[root].aabb[0][1],
					geom.bvh.aabbs[root].aabb[0][2]);
	Vec3 MIN = Vec3(geom.bvh.aabbs[root].aabb[1][0],
					geom.bvh.aabbs[root].aabb[1][1],
					geom.bvh.aabbs[root].aabb[1][2]);

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
			Intersection_point *inter = geom.get_intersection(Ray(ss[i][j],ds[i][j]));
			if(inter != nullptr)count++;
			delete inter;
		}
	}
	ProfilerStart("/tmp/traverse.prof");
	for(int i = 0;i < 101;i++){
		for(int j = 0;j < 101;j++){
			delete geom.get_intersection(Ray(ss[i][j],ds[i][j]));
		}
	}
	ProfilerStop();

	double ti = 0;
	for(int ccc = 0 ;ccc < 100;ccc++){
	std::chrono::system_clock::time_point start,end;
    start = std::chrono::system_clock::now();
	for(int i = 0;i < 100;i++){
		#pragma omp parallel for schedule(dynamic, 1) num_threads(4)
		for(int j = 0;j < 100;j++){
			//delete geom.get_intersection(Ray(ss[i][j],ds[i][j]));
			geom.bvh.traverse(Ray(ss[i][j],ds[i][j]),geom.vertices,geom.prims);
		}
	}
	end = std::chrono::system_clock::now();

    auto elapsed = std::chrono::duration_cast< std::chrono::milliseconds >(end - start).count();
    std::cout << elapsed <<"ms"<< std::endl;
	ti += elapsed;
	}
	std::cout << "ave " << ti / 100.0 << "ms" << std::endl;

	int ii = 76,jj = 48;
	Intersection_point* inter =	geom.get_intersection(Ray(ss[ii][jj],ds[ii][jj]));
	
	std::cout << "衝突回数 " << count << std::endl;
	std::cout << "origin" << std::setprecision(10) << ss[ii][jj] << std::endl;
	std::cout << "direction" << std::setprecision(10) << ds[ii][jj].normalized() << std::endl;
	
	if(nullptr != inter)std::cout << std::setprecision(10) << inter->position << std::endl;
	if(nullptr != inter)std::cout << std::setprecision(10) << inter->distance << std::endl;
	delete inter;

	std::cerr<< "polygon intersection   " << (R)BVH_count_polygon_intersection / BVH_count << std::endl;
	std::cerr << "polygon intersection time   " << (R)BVH_time_polygon_intersection / BVH_count << " ns" << std::endl;
	std::cerr << "traverse   " << (R)BVH_count_traverse / BVH_count << std::endl;
	
	return 0;
}
