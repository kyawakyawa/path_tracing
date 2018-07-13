#include "../../inc/Camera/Camera.hpp"

Camera::Camera() : Camera::Camera(480,480) {};
Camera::Camera(const int w,const int h) : Camera::Camera(w,h,Vec3(0,0,0),Vec3(0,0,-1),Vec3(0,1,0)){}; 
Camera::Camera(const int w,const int h,const Vec3 &p,const Vec3 &d,const Vec3 &u) : pixel_w(w),pixel_h(h),img(new FColor[h * w]),position(p),dir(d.normalized()),up(u.normalized()){}; 

Camera::~Camera() {
    delete[] img;
}

void Camera::init() {
    for(int i = 0;i < pixel_h;i++) {
        for(int j = 0;j < pixel_w;j++) {
        img[i * pixel_w + j] = FColor(0,0,0);
        }
    }
}

void Camera::set_radiance(Radiance *rad) {
    radiance = rad;
}

void Camera::out_ppm_stdio() const {
    std::printf("P3\n%d %d\n255\n",pixel_w,pixel_h);
    for(int i = 0;i < pixel_h * pixel_w;i++) 
        img[i].print255();
}

void Camera::out_ppm_file() const {
    Camera::out_ppm_stdio();
}