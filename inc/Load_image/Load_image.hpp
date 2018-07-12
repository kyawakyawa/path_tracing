#pragma once

#include <string>

#include "../Color/FColor.hpp"

struct Load_image {
    int width = 0;
    int height = 0;

    FColor *img = nullptr;

    Load_image() = default;
    ~Load_image();

    bool load_image(std::string filename);
};
