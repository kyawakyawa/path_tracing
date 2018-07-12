#include "../Scene/Scene.hpp"
#include "../Shape/Sphere.hpp"
#include "../Shape/Plane.hpp"
#include "../GL_out/glout.hpp"

int main(int argc, char **argv){
    Scene scene(1440,1080);
    
	R a = 0.85;
	R b = 0.10;
	
	scene.add(new Sphere(Vec3(1e5+1,40.8,81.6),1e5,Material(FColor(a,b,b))));
	scene.add(new Sphere(Vec3(-1e5+99,40.8,81.6),1e5,Material(FColor(b,a,b))));
	scene.add(new Sphere(Vec3(50,40.8,1e5),1e5,Material(FColor(a,a,a))));
	scene.add(new Sphere(Vec3(50,40.8,-1e5 + 250),1e5,Material(FColor(a,a,a))));
	//scene.add(new Sphere(Vec3(50,1e5,81.6),1e5,Material(FColor(a,a,a))));
    scene.add(new Plane(Vec3(0,1,0),Vec3(50,0,81.6),PL_CHECKERBOARD_CHECK));
	scene.add(new Sphere(Vec3(50,-1e5 + 81.6,81.6),1e5,Material(FColor(a,a,a))));
	scene.add(new Sphere(Vec3(65,20,20),20,Material(FColor(b,b,a))));
	scene.add(new Sphere(Vec3(27,16.5,47),16.5,Material(FColor(240.0 / 255,210.0 / 255,37.0 / 255),MT_PERFECT_REF)));
	scene.add(new Sphere(Vec3(77,16.5,78),16.5,Material(MT_REFRACTION)));
	scene.add(new Sphere(Vec3(50.0,90.0,81.6),15,Material(FColor(0.30,0.30,0.30),FColor(32,32,32))));

	//drawgl(argc,argv,scene);
	scene.draw(256,3);
	return 0;
}
