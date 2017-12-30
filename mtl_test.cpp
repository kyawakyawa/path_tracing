#include "Scene.h"
#include "Mesh.h"
#include "glout.h"

int main(int argc, char **argv){
	R a = 0.85;
	R b = 0.10;

	if(argc < 2){
		printf("objファイルを指定してー\n");
		return 0;
	}

	Scene scene(Camera(600,600,Vec3(-70,52.0,52.0),Vec3(1.0,-0.04,0.0),Vec3(0.0,1.0,0.0),50.0));
	//Scene scene(Camera(600,600,Vec3(20,52.0,102.0),Vec3(1.0,-0.04,-1.03),Vec3(0.0,1.0,0.0),30.0));

	scene.add(new Mesh(argv[1],40,Vec3(70,20,50),Vec3(0.0,1.0,0.0),0));//roadBike
	//scene.add(new Mesh(argv[1],15,Vec3(70,20,50),Vec3(0.0,1.0,0.0),0));//miku

    
	scene.draw(4,4);
	//drawgl(argc,argv,scene);
	
	return 0;
}
