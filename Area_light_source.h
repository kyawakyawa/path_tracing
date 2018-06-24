#pragma once

#include "Vec3.h"
#include "Light_source.h"
#include "Random.h"

struct Area_light_source : Light_source{//光源の抽象クラス
    const Shape *object = nullptr;
    const R S = 0;
    Area_light_source() = delete;
	inline constexpr Area_light_source(const Shape *obj) : object(obj),S(object->get_S()) {};

	inline Light_info* light_point() const {//引数の位置からの光源の距離、強さ、方向を返す
        Vec3 point,normal;R pdf;FColor emission;
        object->sample_one_point(point,normal,pdf,emission);
        return new Light_info(point,pdf,emission,normal);
    }
	inline R get_weight() const {//光源の「重み」を取得 この光源がサンプルされる確率を計算
        return S * object->materials[(int)(Random::rando() * object->materials.size())].Le.abs();
    }

	~Area_light_source() = default;
};