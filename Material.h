#pragma once

#include "FColor.h"

enum Material_type{
    MT_DEFAULT,//通常
    MT_PERFECT_REF,//完全鏡面反射
    MT_REFRACTION//屈折
};

struct Material{
    const FColor kd;//拡散光反射係数
    const Material_type type;//通常　完全鏡面反射 屈折
    const R n = 1.51;//絶対屈折率
    const FColor Le = FColor(0.0,0.0,0.0);//発光の放射輝度`

    inline Material() = delete;
    inline Material(const FColor &d): kd(d),type(MT_DEFAULT){};
    inline Material(Material_type t): kd(FColor(0.99,0.99,0.99)),type(t){};
    inline Material(const FColor &d,Material_type t): kd(d),type(t){};
    inline Material(const FColor &d,R _n): kd(d),type(MT_REFRACTION),n(_n){};
    inline Material(const FColor &d,const FColor &e): kd(d),type(MT_DEFAULT),Le(e){};
};