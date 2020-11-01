#pragma once

#include "../Color/FColor.hpp"
#include "../Radiance/Radiance.hpp"

struct Camera {
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
    Camera(const int w,const int h,const Vec3 &p,const Vec3 &d,const Vec3 &u);

    void init();

    void set_radiance(Radiance *rad);

    virtual void sensitization(std::vector<Shape*> *shapes,std::vector<Light_source*> *lights, const int samples,const int threads) = 0;

    void out_ppm_stdio() const;

    void out_ppm_file() const;
    virtual ~Camera();
};