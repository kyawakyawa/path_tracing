#include<iomanip>
#include <iostream>

#include "Scene.h"
#include "Geom.h"
#include "glout.h"

int main(int argc, char **argv){

	if(argc < 2){
		printf("objファイルを指定してー\n");
		return 0;
	}

	//Mesh mesh(argv[1],10.0,Vec3(70,50,50),Vec3(0.0,1,0.0),0);//miku
	//Mesh mesh(argv[1],5.0,Vec3(0,0,0),Vec3(0.0,1,0.0),0);//woman
	//Mesh mesh(argv[1],30,Vec3(70,20,50),Vec3(0,1.0,0),0);//lps
	//Mesh mesh(argv[1],1,Vec3(70,15,50),Vec3(0,1.0,0),3.14*7.0/4.0,Ms_SMOOTH);//miku
	//Mesh mesh(argv[1],5,Vec3(70,15,50),Vec3(0,1.0,0),3.14*0/4.0,Ms_SMOOTH);//animasa_miku


	Scene scene(Camera(600,600,Vec3(20,22.0,102.0) + 32 * Vec3(1.0,0.07,-1.09),Vec3(1.0,0.09,-1.09),Vec3(0.0,1.0,0.0),30.0));//lps miku

	scene.add(new Geom(argv[1],30,Vec3(70,20,50),Vec3(0,1.0,0),0));
	//scene.draw(1,10);
	//scene.normal_render();
	drawgl(argc,argv,scene);
	//scene.debug(1,10);

	std::cerr<< "polygon intersection   " << (R)BVH_count_polygon_intersection / BVH_count << std::endl;
	std::cerr << "polygon intersection time   " << (R)BVH_time_polygon_intersection / BVH_count << " ns" << std::endl;
	std::cerr << "traverse   " << (R)BVH_count_traverse / BVH_count << std::endl;
	
	return 0;
}
