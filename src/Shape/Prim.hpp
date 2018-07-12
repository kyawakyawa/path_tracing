#pragma once

#include "../Vec3/Vec3.hpp"

struct Prim {
    int vertices_index[3];
    int normals_index[3];
    int uvs_index[3];

    int mtl_id = 0;
    int texture_id = -1;

    R S = 0;
    Prim() = default;
};