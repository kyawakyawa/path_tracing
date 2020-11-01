#pragma once

#include <vector>

#include "../Vec3/Vec3.hpp"
#include "../Color/FColor.hpp"

#include "../Load_image/Load_image.hpp"

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
        Load_image image;
		if(!image.load_image(filename)){
			width = 1;
			height = 1;
            light.push_back(cl);
			return;
		}

        width = image.width;
        height = image.height;

        for(int i = 0;i < width * height;i++){
            light.push_back(image.img[i]);
        }
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
