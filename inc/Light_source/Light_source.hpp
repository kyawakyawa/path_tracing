#pragma once

#include "../Vec3/Vec3.hpp"
#include "../Information/Light_info.hpp"
#include "../Shape/Shape.hpp"

struct Light_source{//光源の抽象クラス
	inline constexpr Light_source() = default;

	inline virtual Light_info* light_point() const = 0;
	inline virtual R get_weight() const = 0;//光源の「重み」を取得 pdfの値決定に利用

	virtual ~Light_source() = default;
};
