#pragma once

#include<vector>

#include "Vec3.h"

struct Polygon {
    std::vector< Vec3 > vertex;
    R aabb_max[3] = {-1000000000000.0,-1000000000000.0,-1000000000000.0};
    R aabb_min[3] = {1000000000000.0,1000000000000.0,10000000000000.0};
    Polygon() = default;
};