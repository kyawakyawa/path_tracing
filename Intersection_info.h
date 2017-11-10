#pragma once

#include "Shape.h"
#include "Intersection_point.h"

struct Intersection_info{
    Shape *shape;
    Intersection_point *intersection_point;

    inline constexpr Intersection_info(): shape(nullptr),intersection_point(nullptr){};
    inline constexpr Intersection_info(Shape *s,Intersection_point *i): shape(s),intersection_point(i){};

    inline void rewrite(Shape *s,Intersection_point *i){//メンバの更新
        delete intersection_point;
        shape = s;
        intersection_point = i;
    }

    inline ~Intersection_info(){
        delete intersection_point;
    }
};