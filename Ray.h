#pragma once

#include "Vec3.h"

struct Ray{
	const Vec3 start;//レイの始点
	const Vec3 direction;//レイの方向

	Ray() = delete;
	inline Ray(Vec3 s,Vec3 d): start(s),direction(d.normalized()){};

};
