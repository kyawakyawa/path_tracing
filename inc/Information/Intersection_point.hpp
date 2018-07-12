#pragma once

#include "../Vec3/Vec3.hpp"
#include "../Material/Material.hpp"

struct Intersection_point{
	const R distance;//交点までの距離
	const Vec3 position;//交点座標
	const Vec3 normal;//交点における法線
	const Material material;

	Intersection_point() = delete;
	inline Intersection_point(const R d,const Vec3 &p,const Vec3 &n,const Material &m): distance(d),position(p),normal(n.normalized()),material(m){};
};
