#pragma once

#include "Camera.hpp"

struct Thin_lens : Camera {

    const Vec3 x,y;

    const R width_of_sensor = 50;
    const R height_of_sensor;
    const R dist_sensor_lens;
    const Vec3 sensor_center;

    const R focal_length;//レンズの焦点距離
    const R lens_radius;//レンズの半径
    const R f_number;

    const R dist_lens_object_plane;

    const R pdf_i;
    const R pdf_l;

    Thin_lens() = delete;
    Thin_lens(const int w,const int h,const Vec3 &p,const Vec3 &d,const Vec3 &u,const R fov_,const R focus_distance/*レンズからの焦点の合う距離(焦点距離ではない、焦点距離はこの値の半分)*/,const R f_number_/*F値*/);

    void print() const;

    void sensitization(std::vector<Shape*> *shapes,std::vector<Light_source*> *lights,const int samples,const int threads); 
};