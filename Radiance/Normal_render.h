#pragma once

#include "Radiance.h"

struct Normal_render: Radiance {

    Normal_render() = default;

    FColor radiance(const Ray &ray) const {
        Intersection_info *intersection_info = get_intersection_of_nearest(ray);
        if(intersection_info == nullptr)return FColor(0,0,0);

        const Vec3 &normal = intersection_info->intersection_point->normal;
        return FColor((normal.x + 1.0) * 0.5,(normal.y + 1.0) * 0.5,(normal.z + 1.0) * 0.5);
    }

    void set(std::vector<Shape*> *s,std::vector<Light_source*> *l) {
        shapes = s;
    }

};