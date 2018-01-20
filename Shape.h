#pragma once

#include <vector>

#include "Vec3.h"
#include "Material.h"
#include "Texture.h"
#include "Ray.h"
#include "Intersection_point.h"

struct Shape{
	std::vector< Material > materials;//材質
	std::vector< Texture > textures;//材質

	inline Shape() = default;
	//inline constexpr Shape(const Material &m): material(m){};

	inline virtual Intersection_point* get_intersection(const Ray &ray) const = 0;//レイとの交差についての情報を返す

	inline virtual R get_S() const = 0;

	inline virtual void sample_one_point (Vec3 &point,Vec3 &normal,R &pdf,FColor &emission) const = 0;
	//inline virtual Material get_material(const Vec3 &position) const = 0;//その点における材質を返す

	inline virtual ~Shape() = default;
};
