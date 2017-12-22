#pragma once

#include<vector>

#include "Vec3.h"

struct Polygon {
    std::vector< Vec3 > vertex;
    std::vector< Vec3 > uv;
    Vec3 normal;
    R aabb_max[3] = {-1000000000000.0,-1000000000000.0,-1000000000000.0};
    R aabb_min[3] = {1000000000000.0,1000000000000.0,10000000000000.0};
    int mtl_id = 0;
    Polygon() = default;
};