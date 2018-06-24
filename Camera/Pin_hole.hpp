#pragma once

#include "Camera.hpp"

struct Pin_hole : Camera {

    const R dist;

    const Vec3 x,y;

    const R width_of_sensor = 1.0;
    const R height_of_sensor = 1.0;

    const Vec3 hole_center = Vec3(0,0,1);
    const R hole_radius = 1.0;

    const R pdf_i = 1.0;
    const R pdf_h = 1.0 / M_PI;

    Pin_hole() = delete;
    inline Pin_hole(const int w,const int h,const Vec3 &p,const Vec3 &d,const Vec3 &u,const R dis) : Camera(w,h,p,d,u)
        ,dist(dis),x(cross(dir,up).normalized()),y(cross(dir,x).normalized())
        ,
        ,x(cross(dir,up).normalized()),y(cross(dir,x).normalized())
        ,object_center(dir * (A + B)),width_of_object_plane(width_of_sensor * B / A),height_of_object_plane(height_of_sensor * B / A)
        ,lens_center(dir * A),lens_radius(3.0)
        ,pdf_i(1.0/width_of_sensor/height_of_sensor),pdf_l(1.0/lens_radius/lens_radius/M_PI){};
    inline void sensitization(std::vector<Shape*> *shapes,std::vector<Light_source*> *lights,const int samples,const int threads) {
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
                    const R dx = Random::rando();
                    const R dy = Random::rando();
                    const Vec3 xi = position 
                                    + x * width_of_sensor * ((j + dx) / pixel_w - 0.5)
                                    + y * height_of_sensor * ((i + dy) / pixel_h - 0.5);
                    const R r = Random::rando();
                    const R theta = Random::rando();

                    const Vec3 x0 = position + lens_center
                                    + x * (std::sqrt(r) * std::cos(theta)) * lens_radius;
                                    + y * (std::sqrt(r) * std::sin(theta)) * lens_radius;

                    const int i2  = pixel_h - i;
                    const int j2  = pixel_w - j;

                    const Vec3 x1 = (x0 - xi) + x0;//position + object_center 
                                    //+ x * width_of_object_plane * ((j2 - dx) / pixel_w - 0.5)
                                    //+ y * height_of_object_plane * ((i2 - dy) / pixel_h - 0.5);

                    const FColor L = radiance->radiance(Ray(x0,x1 - x0));

                    const R W = 1.0;// pixel_h * pixel_w / height_of_sensor * width_of_sensor;

                    const R cos_theta_nijo = std::pow(dir * (x0 - xi).normalized(),2.0);

                    const R dist_nijo = (x0 - xi).abs_square();

                    img[(i2 - 1) * pixel_w + j2 - 1] += (W * cos_theta_nijo / pdf_i / pdf_l / dist_nijo) * L;

                }
            }
        }
        sample_num += samples;
        const R r = 1.0 / sample_num;

        for(int i = 0;i < pixel_h;i++) for(int j = 0;j < pixel_w;j++) img[i * pixel_w + j] *= r;
    }
};