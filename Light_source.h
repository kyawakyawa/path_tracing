#pragma once

#include "Vec3.h"
#include "Light_info.h"
#include "Shape.h"

struct Light_source{//光源の抽象クラス
	inline constexpr Light_source() = default;

	inline virtual Light_info* light_point() const = 0;
	inline virtual R get_weight() const = 0;//光源の「重み」を取得 pdfの値決定に利用

	virtual ~Light_source() = default;
};
