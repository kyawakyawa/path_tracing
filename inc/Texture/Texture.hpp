#pragma once

#include <iostream>
#include <string>
#include "../Color/FColor.hpp"

#include "../Load_image/Load_image.hpp"

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

        Load_image image;

        if(!image.load_image(name)) {
            std::cerr << "テクスチャファイル"  << name << "が無いぞ" << std::endl;
            map_kd.push_back(FColor(0.0,1.0,0.0));
            return;
        }

        width = image.width;
        height = image.height;

        for(int i = 0;i < width * height;i++){
            map_kd.push_back(image.img[i]);
        }
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