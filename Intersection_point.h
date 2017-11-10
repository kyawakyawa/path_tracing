#pragma once

#include "Vec3.h"

struct Intersection_point{
	const R distance;//交点までの距離
	const Vec3 position;//交点座標
	const Vec3 normal;//交点における法線

	Intersection_point() = delete;
	inline Intersection_point(const R d,const Vec3 &p,const Vec3 &n): distance(d),position(p),normal(n.normalized()){};
};
