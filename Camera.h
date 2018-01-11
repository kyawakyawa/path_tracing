#pragma once

#include "Ray.h"


struct Camera{
    const Vec3 position = Vec3(50,52.0,220.0);
    //const Vec3 position = Vec3(50,52.0,-90.0);//budda
    //const Vec3 position = Vec3(-70,52.0,52.0);//dragon
    const Vec3 dir = Vec3(0.0,-0.04,-1.0).normalized();
    //const Vec3 dir = Vec3(0.0,-0.04,1.0).normalized();//budda
    //const Vec3 dir = Vec3(1.0,-0.04,0.0).normalized();//dragon
    const Vec3 up = Vec3(0.0,1.0,0.0);

    const int picW;
    const int picH;

    const R width;
    const R height;

    const R dist = 40.0;

    const Vec3 x;
    const Vec3 y;
    const Vec3 center;

    Camera() = delete;
    Camera(int w,int h): picW(w),picH(h),width(30.0),height(30.0 * h / w),x(cross(dir,up).normalized() * width),y(cross(dir,x).normalized() * height),center(dir * dist) {};
    Camera(int w,int h,Vec3 p,Vec3 d,Vec3 u,R ds): position(p),dir(d.normalized()),up(u.normalized()),picW(w),picH(h),width(30.0),height(30.0 * h / w),dist(ds),x(cross(dir,up).normalized() * width),y(cross(dir,x).normalized() * height),center(dir * dist){};

    Ray get_ray(int i,int j) const {
        return Ray(position,center + ((R)j / picW - 0.5) * x + ((R)i / picH - 0.5) * y);
    }

    Ray get_ray(int sy,int sx,int supersamples,int i,int j) const{
        const R d = 1.0 / supersamples;
        const R dx = sx * d + d * 0.5;
        const R dy = sy * d + d * 0.5;

        return Ray(position,center + ((R)(j + dx) / picW - 0.5) * x + ((R)(i + dy) / picH - 0.5) * y);
    }
};
