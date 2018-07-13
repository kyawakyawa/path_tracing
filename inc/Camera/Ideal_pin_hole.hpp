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

    void sensitization(std::vector<Shape*> *shapes,std::vector<Light_source*> *lights,const int samples,const int threads);/* {
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
    }*/
    ~Ideal_pin_hole();
};