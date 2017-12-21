#pragma once

#include <vector>
#include <cmath>

#include "Vec3.h"
#include "FColor.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct Infinity_light_source{
    int width = 1;
    int height = 1;
    std::vector< FColor > light;
    Infinity_light_source(){
        light.push_back(FColor(100.0 / 255,149.0 / 255,237.0 / 255));
    }
    Infinity_light_source(std::string filename){
        int n;
        float *pixel = stbi_loadf(filename.c_str(), &width, &height, &n, 0);

        for(int i = 0;i < width * height;i++){
            light.push_back(FColor(pixel[i * n],pixel[i * n + 1],pixel[i * n + 2]));
        }
        stbi_image_free(pixel);
    }

    FColor get_radiance(const Vec3 &direction) const{//正規化されている必要あり
        const R theta = acos(direction.y);
        const R phi = atan(direction.z / direction.x) ;

        const int i = (int)((theta * M_1_PI) * height) % height;
        const int j = (int)((phi * (0.5 * M_1_PI)) * width) % width;

        return light[i * width + j];
    }
};
