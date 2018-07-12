#pragma once

#include<vector>

#include "Vec3.h"

struct Polygon {
    std::vector< Vec3 > vertex;
    std::vector< Vec3 > uv;
    std::vector< Vec3 > vertex_normal;
    Vec3 face_normal;
    R aabb_max[3] = {-1000000000000.0,-1000000000000.0,-1000000000000.0};
    R aabb_min[3] = {1000000000000.0,1000000000000.0,10000000000000.0};
    int mtl_id = 0;
    int texture_id = -1;
    Polygon() = default;
    /*Polygon(const Polygon &obj){
        vertex = obj.vertex;
        uv = obj.uv;
        vertex_normal = obj.vertex_normal;
        face_normal = obj.face_normal;
        for(int i = 0;i < 3;i++){
            aabb_max[i] = obj.aabb_max[i];
            aabb_min[i] = obj.aabb_min[i];
        }
        mtl_id = obj.mtl_id;
        texture_id = obj.texture_id;
    }*/
};