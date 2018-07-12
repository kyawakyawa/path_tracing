#pragma once

#include "Radiance.hpp"

struct Path_tracing : Radiance {
    const int min_depth = 4;
    const int max_depth = 64;
	const Infinity_light_source back;

    Path_tracing() = default;
    Path_tracing(int md,int Md) : Path_tracing(md,Md,"") {
    };
    Path_tracing(int md,int Md,const std::string &ibl) : Path_tracing(md,Md,ibl,FColor(0,0,0)) {};
    Path_tracing(int md,int Md,const std::string &ibl,const FColor& cl) : min_depth(md),max_depth(Md),back(ibl,cl) {};

    FColor radiance(const Ray &ray) const {
        return pathtracing(ray,0);
    }

    void set(std::vector<Shape*> *s,std::vector<Light_source*> *l) {
        shapes = s;
        lights = l;
    }
    
	inline FColor pathtracing(const Ray &ray,const int depth) const{
		const Intersection_info *intersection_info = get_intersection_of_nearest(ray);
			
		if(intersection_info == nullptr){//物体が存在しない
			return back.get_radiance(ray.direction);
		}
		
		const Intersection_point *intersection = intersection_info->intersection_point;
		//const Shape *intersection_shape = intersection_info->shape;
		const Material material = intersection->material;//intersection_shape->get_material(intersection->position);
		const Vec3 normal = ((ray.direction * intersection->normal < 0.0) ? 1.0 : -1.0) * intersection->normal;
		
		FColor L = (ray.direction * intersection->normal < 0.0) ? material.Le : FColor(0,0,0);

		R P = std::max(std::max(material.kd.red,material.kd.green),material.kd.blue);

		if(depth < min_depth)
			P = 1.0;
		
		if (depth >= max_depth)//最大値で大幅に確率を下げる
			P *= pow(0.5, depth - max_depth);
		
		

		if(P < Random::rando()){//再帰が深すぎ
			delete intersection_info;
			return L;
		}

		switch (material.type){

		case MT_DEFAULT: {
			Vec3 u;
			if (std::abs(normal.x) > 1e-6) // ベクトルwと直交するベクトルを作る。w.xが0に近い場合とそうでない場合とで使うベクトルを変える。
				u = (cross(Vec3(0.0, 1.0, 0.0), normal)).normalized();
			else
				u = (cross(Vec3(1.0, 0.0, 0.0), normal)).normalized();
			Vec3 v = cross(normal,u);

			R u1 = Random::rando() * 2.0 * 3.1415926535,u2 = Random::rando(),u3 = std::sqrt(u2);

			Vec3 omega = 
				u * std::cos(u1) * u3 +
				v * std::sin(u1) * u3 +
				normal * std::sqrt(1 - u2);

			L += material.kd * pathtracing(Ray(intersection->position /*+ epsilon * omega*/,omega),depth + 1) * (1.0 / P);
		} break;
		case MT_PERFECT_REF: {
			const Vec3 r = -2.0 * (normal * ray.direction) * normal + ray.direction;
			const Vec3 p = intersection->position;
			L += material.kd * pathtracing(Ray(p,r),depth + 1) * (1.0 / P);
		} break;
		case MT_REFRACTION: {
			const R n = ((ray.direction * intersection->normal < 0.0) ? 1.0 / material.n : material.n);
			const R dn = ray.direction * normal;
			const R cos2t = 1.0 - n * n * (1.0 - dn * dn);

			const Vec3 p = intersection->position;
			const Vec3 r = -2.0 * (normal * ray.direction) * normal + ray.direction;

			if(cos2t < 0.0){
				L += material.kd * pathtracing(Ray(p,r),depth + 1) * (1.0 / P);
				break;
			}

			Vec3 T = (n * ray.direction - (n * dn + std::sqrt(cos2t) ) * normal).normalized();
			const R a = 1.0 - material.n,b = 1.0 + material.n;
			const R F0 = (a * a) / (b * b);

			const R Fr = F0 + (1.0 - F0) * std::pow(1.0 + ((ray.direction * intersection->normal < 0.0) ? dn : T * normal),5);
			const R Tr = (1.0 - Fr) * n * n;

			const R probability = 0.25 * 0.5 * Fr;
			if(depth > 2){
				if(Random::rando() < probability){
					L += material.kd * pathtracing(Ray(p,r),depth + 1) * (1.0 / P / probability) * Fr;
				}else{
					L += material.kd * pathtracing(Ray(p,T),depth + 1) * (1.0 / P / (1.0 - probability)) * Tr;
				}
			}else {
				L += material.kd * (pathtracing(Ray(p,r),depth + 1) * Fr + pathtracing(Ray(p,T),depth + 1) * Tr) * (1.0 / P);
			}
		} break;
		case MT_NORMALIZED_PHONG: {
			Vec3 u;
			if (std::abs(normal.x) > 1e-6) // ベクトルwと直交するベクトルを作る。w.xが0に近い場合とそうでない場合とで使うベクトルを変える。
				u = (cross(Vec3(0.0, 1.0, 0.0), normal)).normalized();
			else
				u = (cross(Vec3(1.0, 0.0, 0.0), normal)).normalized();
			const Vec3 v = cross(normal,u);

			const R u1 = Random::rando() * 2.0 * 3.1415926535,t = std::pow(Random::rando(),1.0 / (2 + material.n));//alpha = 5
			const R t2 = std::sqrt(1.0 - t * t);

			const Vec3 omega = 
				u * std::cos(u1) * t2 +
				v * std::sin(u1) * t2 +
				normal * t;

			const R nome = normal * omega;
			const R f = (omega - 2 * nome * normal) * ray.direction / nome;

			L += material.kd * std::pow(f,material.n) * pathtracing(Ray(intersection->position /*+ epsilon * omega*/,omega),depth + 1) * (1.0 / P);
		}
		}

		delete intersection_info;
		return L;
	}
};