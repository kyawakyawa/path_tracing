#include "Camera.hpp"

inline Camera::Camera() : Camera(480,480) {};
inline Camera::Camera(const int w,const int h) : Camera(w,h,Vec3(0,0,0),Vec3(0,0,-1),Vec3(0,1,0)){}; 
inline Camera::Camera(const int w,const int h,const Vec3 &p,const Vec3 &d,const Vec3 &u) : pixel_w(w),pixel_h(h),img(new FColor[h * w]),position(p),dir(d.normalized()),up(u.normalized()){}; 
inline Camera::~Camera() {
    delete[] img;
}

inline void Camera::init() {
    for(int i = 0;i < pixel_h;i++) {
        for(int j = 0;j < pixel_w;j++) {
            img[i * pixel_w + j] = FColor(0,0,0);
        }
    }
}

inline void Camera::set_radiance(Radiance *rad) {
    radiance = rad;
}

inline void Camera::out_ppm_stdio() const {
    std::printf("P3\n%d %d\n255\n",pixel_w,pixel_h);
    for(int i = 0;i < pixel_h * pixel_w;i++) 
        img[i].print255();
}

inline void Camera::out_ppm_file() const {
    out_ppm_stdio();
}