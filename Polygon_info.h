#pragma once

#include "Vec3.h"

struct Polygon_info{
	const R distance;//交点までの距離
	const int polygon_index;

	Polygon_info() = delete;
	inline Polygon_info(const R d,const int pi): distance(d),polygon_index(pi){};
};
