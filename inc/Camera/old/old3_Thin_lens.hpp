#pragma once

#include "Camera.hpp"

struct Thin_lens : Camera {

    const Vec3 x,y;

    const R dist_sensor_lens;
    const R width_of_sensor;
    const R height_of_sensor;
    const Vec3 sensor_center;

    const R f_number;
    const R lens_radius;

    const R dist_lens_object_plane;

    const R pdf_i;
    const R pdf_l;

    Thin_lens() = delete;
    inline Thin_lens(const int w,const int h,const Vec3 &p,const Vec3 &d,const Vec3 &u,const R fov_/*fov*/,const R focus_distance/*レンズからの焦点の合う距離(焦点距離ではない、焦点距離はこの値の半分)*/,const R f_number_/*F値*/) : Camera(w,h,p,d,u)
        ,x(cross(dir,up).normalized()),y(cross(dir,x).normalized())
        ,dist_sensor_lens(focus_distance)
        ,width_of_sensor(2.0 * dist_sensor_lens * std::tan(fov_ / 360.0 * M_PI)),height_of_sensor(width_of_sensor * h / w)
        ,sensor_center(-dir * dist_sensor_lens)

        ,f_number(f_number_),lens_radius(focus_distance / 2.0 / f_number_ / 2.0)

        ,dist_lens_object_plane(focus_distance)

        ,pdf_i(1.0 / (height_of_sensor * width_of_sensor / pixel_h / pixel_w)),pdf_l(1.0 / M_PI / lens_radius / lens_radius) {};

    inline void print() const {
        std::cerr << "width_of_sensor: " << width_of_sensor << std::endl;
        std::cerr << "height_of_sensor: " << height_of_sensor << std::endl;
        std::cerr << "lens_radius: " << lens_radius << std::endl; 
    }
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
                    const Vec3 xi = position + sensor_center
                                    + x * width_of_sensor * ((j + dx) / pixel_w - 0.5)
                                    + y * height_of_sensor * ((i + dy) / pixel_h - 0.5);
                    /*const R r = Random::rando();
                    const R theta = Random::rando() * 2.0 * M_PI;

                    const Vec3 x0 = position
                                    + x * (lens_radius * std::sqrt(r) * std::cos(theta));
                                    + y * (lens_radius * std::sqrt(r) * std::sin(theta));*/
                    R u1,u2;
                    do {
                        u1 = Random::rando();
                        u2 = Random::rando();
                    }while(u1 * u1 + u2 * u2 > 1.0);

                    const Vec3 x0 = position 
                                    + u1 * lens_radius * x
                                    + u2 * lens_radius * y;

                    const Vec3 e = (position - xi).normalized();

                    const Vec3 x1 = position + e * dist_lens_object_plane / (e * dir);

                    const FColor L = radiance->radiance(Ray(x0,x1 - x0));

                    const R W = 0.00001 * f_number * f_number * pixel_h * pixel_w / height_of_sensor * width_of_sensor;

                    const R cos_theta_nijo = std::pow(dir * (x0 - xi).normalized(),2.0);

                    const R dist_nijo = (x0 - xi).abs_square();

                    img[(pixel_h - i - 1) * pixel_w + pixel_w - j - 1] += (W * cos_theta_nijo / pdf_i / pdf_l / dist_nijo) * L;

                }
            }
        }
        sample_num += samples;
        const R r = 1.0 / sample_num;

        for(int i = 0;i < pixel_h;i++) for(int j = 0;j < pixel_w;j++) img[i * pixel_w + j] *= r;
    }
};