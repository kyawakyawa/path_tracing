#include<iomanip>
#include <iostream>

#include "Scene.h"
#include "Mesh.h"
#include "glout.h"

int main(int argc, char **argv){

	if(argc < 2){
		printf("objファイルを指定してー\n");
		return 0;
	}

	//Mesh mesh(argv[1],10.0,Vec3(70,50,50),Vec3(0.0,1,0.0),0);//miku
	//Mesh mesh(argv[1],5.0,Vec3(0,0,0),Vec3(0.0,1,0.0),0);//woman
	Mesh mesh(argv[1],30,Vec3(70,20,50),Vec3(0,1.0,0),0);//lps
	//Mesh mesh(argv[1],1,Vec3(70,15,50),Vec3(0,1.0,0),3.14*7.0/4.0,Ms_SMOOTH);//miku
	//Mesh mesh(argv[1],5,Vec3(70,15,50),Vec3(0,1.0,0),3.14*0/4.0,Ms_SMOOTH);//animasa_miku


	int root = mesh.bvh.root;
	Vec3 MAX = Vec3(mesh.bvh.nodes[root].aabb_max[0],
				mesh.bvh.nodes[root].aabb_max[1],
				mesh.bvh.nodes[root].aabb_max[2]);
	Vec3 MIN = Vec3(mesh.bvh.nodes[root].aabb_min[0],
				mesh.bvh.nodes[root].aabb_min[1],
				mesh.bvh.nodes[root].aabb_min[2]);
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

	std::cerr << "MIN  " << MIN << std::endl;
	std::cerr << "MAX  " << MAX << std::endl;
	std::cerr << "CEN" << c << std::endl;


	int count = 0;
	for(int i = 0;i < 101;i++){
		for(int j = 0;j < 101;j++){
			Intersection_point *inter = mesh.get_intersection(Ray(ss[i][j],ds[i][j]));
			if(inter != nullptr)count++;
			delete inter;
		}
	}
	int ii = 76,jj = 48;
	Intersection_point* inter =	mesh.get_intersection(Ray(ss[ii][jj],ds[ii][jj]));
	std::cerr << "衝突回数 " << count << std::endl;
	std::cerr << "origin" << std::setprecision(10) << ss[ii][jj] << std::endl;
	std::cerr << "direction" << std::setprecision(10) << ds[ii][jj].normalized() << std::endl;
	std::cerr << count << std::endl;
	if(nullptr != inter)std::cerr << std::setprecision(10) << inter->position << std::endl;
	delete inter;

	//Scene scene(Camera(600,600,Vec3(10,67.0,62.0) + 40 * Vec3(1.0,-0.04,-0.9),Vec3(1.0,-0.04,0.0),Vec3(0.0,1.0,0.0),60.0));//miku
	//Scene scene(Camera(600,600,Vec3(30,70.5,-40),Vec3(-0.8,-0.04,1.0),Vec3(0.0,1.0,0.0),60.0));//woman
	Scene scene(Camera(600,600,Vec3(20,22.0,102.0) + 32 * Vec3(1.0,0.07,-1.09),Vec3(1.0,0.09,-1.09),Vec3(0.0,1.0,0.0),30.0));//lps miku

	scene.add(new Mesh(mesh));
	//scene.draw(1,10);
	scene.normal_render();
	//drawgl(argc,argv,scene);
	
	return 0;
}
