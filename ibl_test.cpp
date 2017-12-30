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

	Scene scene(Camera(600,600,Vec3(-70,-5.0,52.0),Vec3(1.0,0.2,0.0),Vec3(0.0,1.0,1.0),50.0));
    
	scene.add(new Mesh(argv[1],40,Vec3(70,20,50),Vec3(2432.0,9328.0,782.0),3.14));
	scene.draw(4,4);
	//drawgl(argc,argv,scene);
	
	return 0;
}
