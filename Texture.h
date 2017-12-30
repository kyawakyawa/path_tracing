#pragma once

#include <iostream>
#include <string>
#include "FColor.h"

//#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


struct Texture{
    std::string name;
    std::vector< FColor > map_kd;
    int width = 1;
    int height = 1;
    Texture() = delete;
    Texture(const std::string &n):name(n){
        load_img();
    };
    Texture(const char *n):name(n){
        load_img();
    };

    inline void load_img(){

        if(name.empty()){
            std::cerr << "テクスチャ名が指定されて無いぞ" << std::endl;
            map_kd.push_back(FColor(0.0,1.0,0.0));
            return;
        }
        int n;
        float *pixelf = stbi_loadf(name.c_str(), &width, &height, &n, 0);
        if(pixelf == nullptr){
            std::cerr << "テクスチャファイル"  << name << "が無いぞ" << std::endl;
            map_kd.push_back(FColor(0.0,1.0,0.0));
            return;
        }

        for(int i = 0;i < width * height;i++){
            map_kd.push_back(FColor(pixelf[i * n],pixelf[i * n + 1],pixelf[i * n + 2]));
        }
        stbi_image_free(pixelf);
    }

    inline FColor get_kd(const Vec3 &v) const{
        int i = height * (1.0 - v.y);
        int j = width * (v.x);

        if(i < 0){
            i = 0;
        }else if(i >= height){
            i = height - 1;
        }
        if(j < 0){
            j = 0;
        }else if(j >= width){
            j = width - 1;
        }

        return map_kd[i * width + j];
    }
};