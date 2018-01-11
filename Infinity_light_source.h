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
        //light.push_back(FColor(100.0 / 255,149.0 / 255,237.0 / 255));
        light.push_back(FColor(0.75,0.75,0.75));
    }
    Infinity_light_source(const std::string filename):Infinity_light_source(filename,FColor(0.75,0.75,0.75)){};
    Infinity_light_source(const std::string filename,const FColor &cl){
        int n;
        float *pixel = stbi_loadf(filename.c_str(), &width, &height, &n, 0);
		if(pixel == nullptr){
			width = 1;
			height = 1;
        	//light.push_back(FColor(100.0 / 255,149.0 / 255,237.0 / 255));
            light.push_back(cl);
			return;
		}

        for(int i = 0;i < width * height;i++){
            light.push_back(FColor(pixel[i * n],pixel[i * n + 1],pixel[i * n + 2]));
        }
        stbi_image_free(pixel);
    }

    FColor get_radiance(const Vec3 &direction) const{//正規化されている必要あり
        const R theta = std::acos(direction.y);
        R phi = std::atan(direction.z / direction.x) + ((direction.x > 0) ? 2.0 * M_PI : M_PI);
		phi += 0;
		phi = std::fmod(phi,2.0 * M_PI);

        const int i = (int)((theta * M_1_PI) * height) % height;
        const int j = (int)((phi * (0.5 * M_1_PI)) * width) % width;

        return light[i * width + j];
    }
};
