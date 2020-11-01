#pragma once

#include "../Vec3/Vec3.hpp"

struct Ray{
	Vec3 start;//レイの始点
	Vec3 direction;//レイの方向

	Ray() = delete;
	inline Ray(Vec3 s,Vec3 d): start(s),direction(d.normalized()){};

};
