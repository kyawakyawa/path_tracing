#pragma once

#include "../Ray.h"
#include "../FColor.h"
#include "../Radiance/Radiance.h"

/*struct Camera {
    const Vec3 position = Vec3(0,0,0);
    const Vec3 dir = Vec3(0,0,-1);
    const Vec3 up = Vec3(0,1,0);

    const int pixel_w = 480;
    const int pixel_h = 480;

    long sample_num = 0;

    FColor *img = nullptr;

    Radiance *radiance = nullptr;

    Camera();
    Camera(const int w,const int h);
    Camera(const  int w,const int h,const Vec3 &p,const Vec3 &d,const Vec3 &u);


    void init();

    void set_radiance(Radiance *rad);
    virtual void sensitization(std::vector<Shape*> *shapes,std::vector<Light_source*> *lights, int samples) = 0;
    void out_ppm_stdio() const;
    void out_ppm_file() const;

    virtual ~Camera();
};*/

struct Camera {
    const Vec3 position = Vec3(0,0,0);
    const Vec3 dir = Vec3(0,0,-1);
    const Vec3 up = Vec3(0,1,0);

    const int pixel_w = 480;
    const int pixel_h = 480;

    long sample_num = 0;

    FColor *img = nullptr;

    Radiance *radiance = nullptr;

    inline Camera() : Camera(480,480) {};
    inline Camera(const int w,const int h) : Camera(w,h,Vec3(0,0,0),Vec3(0,0,-1),Vec3(0,1,0)){}; 
    inline Camera(const int w,const int h,const Vec3 &p,const Vec3 &d,const Vec3 &u) : pixel_w(w),pixel_h(h),img(new FColor[h * w]),position(p),dir(d.normalized()),up(u.normalized()){}; 

    inline void init() {
        for(int i = 0;i < pixel_h;i++) {
            for(int j = 0;j < pixel_w;j++) {
            img[i * pixel_w + j] = FColor(0,0,0);
            }
        }
    }

    inline void set_radiance(Radiance *rad) {
        radiance = rad;
    }
    virtual void sensitization(std::vector<Shape*> *shapes,std::vector<Light_source*> *lights, const int samples,const int threads) = 0;
    inline void out_ppm_stdio() const {
        std::printf("P3\n%d %d\n255\n",pixel_w,pixel_h);
        for(int i = 0;i < pixel_h * pixel_w;i++) 
            img[i].print255();
    }

    inline void out_ppm_file() const {
        out_ppm_stdio();
    }
    virtual ~Camera() {
        delete[] img;
    }
};