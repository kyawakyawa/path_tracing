#include "Scene.h"
#include "Sphere.h"
#include "Plane.h"
#include "glout.h"

int main(int argc, char **argv){
    Scene scene;
    
	R a = 0.75;
	
	scene.add(new Sphere(Vec3(1e5+1,40.8,81.6),1e5,Material(FColor(a,0.25,0.25))));
	scene.add(new Sphere(Vec3(-1e5+99,40.8,81.6),1e5,Material(FColor(0.25,0.25,a))));
	scene.add(new Sphere(Vec3(50,40.8,1e5),1e5,Material(FColor(a,a,a))));
	scene.add(new Sphere(Vec3(50,40.8,-1e5 + 250),1e5,Material(FColor(0,0,0))));
	scene.add(new Sphere(Vec3(50,1e5,81.6),1e5,Material(FColor(a,a,a))));
	scene.add(new Sphere(Vec3(50,-1e5 + 81.6,81.6),1e5,Material(FColor(a,a,a))));
	scene.add(new Sphere(Vec3(65,20,20),20,Material(FColor(0.25,a,0.25))));
	scene.add(new Sphere(Vec3(27,16.5,47),16.5,Material(MT_PERFECT_REF)));
	scene.add(new Sphere(Vec3(77,16.5,78),16.5,Material(MT_REFRACTION)));
	scene.add(new Sphere(Vec3(50.0,90.0,81.6),15,Material(FColor(0.30,0.30,0.30),FColor(36,36,36))));

    /*scene.add(new Sphere(Vec3(0,-0.5,3),0.5,Material(FColor(0.01,0.01,0.01),FColor(0.25,a,0.25),FColor(0.30,0.30,0.30),8)));
    scene.add(new Sphere(Vec3(0,1.25,3),0.5,Material(FColor(0.01,0.01,0.01),FColor(0.00,0.00,0.00),FColor(0.30,0.30,0.30),8,FColor(5,5,5))));
    //scene.add(new Sphere(Vec3(-0.25,-0.5,3),0.5,Material(MT_PERFECT_REF)));
	scene.add(new Plane(Vec3(0,1,0),Vec3(0,-1,0),Material(FColor(0.00,0.00,0.00),FColor(a,a,a),FColor(0.00,0.00,0.00),8)));
	scene.add(new Plane(Vec3(0,-1,0),Vec3(0,1,0),Material(FColor(0.00,0.00,0.00),FColor(a,a,a),FColor(0.00,0.00,0.00),8)));
	scene.add(new Plane(Vec3(-1,0,0),Vec3(1,0,0),Material(FColor(0.00,0.00,0.00),FColor(0.25,0.25,a),FColor(0.00,0.00,0.00),8)));
	scene.add(new Plane(Vec3(1,0,0),Vec3(-1,0,0),Material(FColor(0.00,0.00,0.00),FColor(a,0.25,0.25),FColor(0.00,0.00,0.00),8)));
	scene.add(new Plane(Vec3(0,0,-1),Vec3(0,0,5),Material(FColor(0.00,0.00,0.00),FColor(a,a,a),FColor(0.00,0.00,0.00),8)));
    //scene.add(new Plane(Vec3(0,0,1),Vec3(0,0,-5.1),Material(FColor(0.00,0.00,0.00),FColor(a,a,a),FColor(0.00,0.00,0.00),8,FColor(0.3,0.3,0.3))));*/
    
	drawgl(argc,argv,scene);
	//scene.draw();
	return 0;
}
