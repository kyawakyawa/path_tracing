#include "../../inc/Camera/Ideal_pin_hole.hpp"

Ideal_pin_hole::Ideal_pin_hole() : Ideal_pin_hole(480,480) {};
Ideal_pin_hole::Ideal_pin_hole(const int w,const int h) : Ideal_pin_hole(w,h,Vec3(0,0,0),Vec3(0,0,-1),Vec3(0,1,0),40) {};
Ideal_pin_hole::Ideal_pin_hole(const int w,const int h,const Vec3 &p,const Vec3 &d,const Vec3 &u,const R fov) : Camera(w,h,p,d,u),width(1.0),height(1.0 * h / w),dist(0.5 / std::tan(fov / 360.0 * M_PI)),x(cross(dir,up).normalized() * width),y(cross(dir,x).normalized() * height),center(dir.normalized() * dist){};
Ideal_pin_hole::~Ideal_pin_hole() {
	delete[] img;
}

void Ideal_pin_hole::sensitization(std::vector<Shape*> *shapes,std::vector<Light_source*> *lights,const int samples,const int threads) {
    if(radiance == nullptr) {
        std::cerr << "Not Found radiance" << std::endl;
        return;
    }
    if(sample_num == 0) {
        init();
        radiance ->set(shapes,lights);
    }

    for(int i = 0;i < pixel_h;i++) for(int j = 0;j < pixel_w;j++) img[i * pixel_w + j] *= sample_num;

    #pragma omp parallel for schedule(dynamic, 1) num_threads(threads)
    for(int i = 0;i < pixel_h;i++) {
        for(int j = 0;j < pixel_w;j++) {
            for(int k = 0;k < samples;k++) {
                img[i * pixel_w + j] += radiance->radiance(Ray(position,center + ((j + Random::rando()) / pixel_w - 0.5) * x + ((i + Random::rando()) / pixel_h - 0.5) * y));
            }
        }
    }
    sample_num += samples;
    const R r = 1.0 / sample_num;

    for(int i = 0;i < pixel_h;i++) for(int j = 0;j < pixel_w;j++) img[i * pixel_w + j] *= r;
}