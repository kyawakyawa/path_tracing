#pragma once

#include<cmath>

#include "Shape.hpp"
#include "../Random/Random.hpp"

struct Sphere : public Shape{
	const Vec3 center;//中心の座標
	const R radius;//半径

	Sphere() = delete;
	inline Sphere(const Vec3 &c,const R &r,const Material &m): center(c),radius(r){
		materials.push_back(m);
	};
	
	inline Intersection_point* get_intersection(const Ray &ray) const{
		const Vec3 &d = ray.direction;
		const Vec3 s = ray.start - center;
		const R A = d.abs_square();
		const R B = 2 * (s * d);
		const R C = s.abs_square() - radius * radius;
		const R D = B * B - 4.0 * A * C;
		
		if(D < -EPS)
			return nullptr;

		const R sqrt_d = std::sqrt(D);
		R t = (-B - sqrt_d) * 0.5 / A;
		if(t < EPS * 100){
			t += sqrt_d / A;
			if(t < EPS * 100)
				return nullptr;
		}

		return new Intersection_point(t,ray.start + t * d,s + t * d,materials[0]);
	}

	inline R get_S() const {
		return 4.0 * M_PI * radius * radius;
	}

	inline void sample_one_point (Vec3 &point,Vec3 &normal,R &pdf,FColor &emission) const { //http://tapioca.hatenablog.jp/entry/2017/02/19/015556
		const R z = Random::rando() * 2.0 - 1.0;
		const R phi = Random::rando() * 2.0 * M_PI;

		const R w = std::sqrt(1.0 - z * z);
		const Vec3 dir = Vec3(w * std::cos(phi),w * std::sin(phi),z);
		point = center + radius * dir;
		normal = dir;
		pdf = 1.0 / get_S();
		emission = materials[0].Le;
	}
};
