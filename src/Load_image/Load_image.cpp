#include "../../inc/Load_image/Load_image.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "../../inc/ext_libs/stb_image.h"

Load_image::~Load_image() {
    delete[] img;
}

bool Load_image::load_image(std::string filename) {
    int n;

    float *pixelf = stbi_loadf(filename.c_str(),&width,&height,&n,0);

    if(pixelf == nullptr) {
        return false;
    }

    img = new FColor[width * height];

    for(int i = 0;i < width * height;i++) {
        img[i] = FColor(pixelf[i * n],pixelf[i * n + 1],pixelf[i * n + 2]);
    }
    stbi_image_free(pixelf);
    return true;
}