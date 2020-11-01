#pragma once

#include "Camera.hpp"

struct Ideal_pin_hole : Camera {
    const R dist = 30;

    const R width = 1.0;//センサーの幅
    const R height = 1.0;//センサーの高さ

    const Vec3 x,y,center;

    Ideal_pin_hole();
    Ideal_pin_hole(const int w,const int h);
    Ideal_pin_hole(const int w,const int h,const Vec3 &p,const Vec3 &d,const Vec3 &u,const R fov);

    void sensitization(std::vector<Shape*> *shapes,std::vector<Light_source*> *lights,const int samples,const int threads);

    ~Ideal_pin_hole();
};