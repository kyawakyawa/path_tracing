#pragma once

#include "Vec3.h"
#include "Material.h"
#include "Ray.h"
#include "Intersection_point.h"

struct Shape{
	const Material material;//材質

	inline Shape() = delete;
	inline constexpr Shape(const Material &m): material(m){};

	inline virtual Intersection_point* get_intersection(const Ray &ray) const = 0;//レイとの交差についての情報を返す

	inline virtual Material get_material(const Vec3 &position) const = 0;//その点における材質を返す

	inline virtual ~Shape() = default;
};
