#include "Scene.h"
#include "Sphere.h"
#include "Plane.h"
#include "glout.h"

int main(int argc, char **argv){
    Scene scene(400,400);
    
	R a = 0.75;
	
	scene.add(new Sphere(Vec3(1e5+1,40.8,81.6),1e5,Material(FColor(a,0.25,0.25))));
	scene.add(new Sphere(Vec3(-1e5+99,40.8,81.6),1e5,Material(FColor(0.25,0.25,a))));
	scene.add(new Sphere(Vec3(50,40.8,1e5),1e5,Material(FColor(a,a,a))));
	scene.add(new Sphere(Vec3(50,40.8,-1e5 + 250),1e5,Material(FColor(a,a,a))));
	//scene.add(new Sphere(Vec3(50,1e5,81.6),1e5,Material(FColor(a,a,a))));
    scene.add(new Plane(Vec3(0,1,0),Vec3(50,0,81.6),PL_CHECKERBOARD_CHECK));
	scene.add(new Sphere(Vec3(50,-1e5 + 81.6,81.6),1e5,Material(FColor(a,a,a))));
	scene.add(new Sphere(Vec3(65,20,20),20,Material(FColor(0.25,a,0.25))));
	scene.add(new Sphere(Vec3(27,16.5,47),16.5,Material(MT_PERFECT_REF)));
	scene.add(new Sphere(Vec3(77,16.5,78),16.5,Material(MT_REFRACTION)));
	scene.add(new Sphere(Vec3(50.0,90.0,81.6),15,Material(FColor(0.30,0.30,0.30),FColor(36,36,36))));

	drawgl(argc,argv,scene);
	//scene.draw(250,2);
	return 0;
}
