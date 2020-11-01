#pragma once

#include "../Ray.h"
#include "../Vec3.h"
#include "../Polygon.h"
#include "../Polygon_info.h"


inline Polygon_info* BVH::polygon_intersection(const Ray &ray,const Polygon &polygon,const int index) {
		const Vec3 &r = ray.direction;
		const int n = polygon.vertex.size() - 2;
		for(int i = 0;i < n;i++){
			const Vec3 T = ray.start - polygon.vertex[0];
			const Vec3 E1 = polygon.vertex[i + 1] - polygon.vertex[0];
			const Vec3 E2 = polygon.vertex[i + 2] - polygon.vertex[0];
			const Vec3 P = cross(r,E2);
			const Vec3 Q = cross(T,E1);

			const R bunbo = P * E1;
			if(bunbo < EPS * EPS && bunbo > -EPS * EPS)
				return nullptr;
			const R inv = 1.0 / bunbo;

			const R t = Q * E2 * inv;
			const R u = P * T * inv;
			const R v = Q * r * inv;
			if(t > EPS && u > 0 && v > 0 && u + v < 1.0){
				return new Polygon_info(t,index);
			}
		}
		return nullptr;

		/*
		const Vec3 &normal = polygon.face_normal;//(cross(polygon.vertex[1] - polygon.vertex[0],polygon.vertex[2] - polygon.vertex[1])).normalized();
		const Vec3 &d = ray.direction;
		Vec3 s2;

		////平面上のもう一つの点を取得
		if(std::abs(normal.x) > EPS)
			s2 = Vec3(-(normal.y + normal.z) / normal.x,1,1);
		else if(std::abs(normal.y) > EPS)
			s2 = Vec3(1,-(normal.x + normal.z) / normal.y,1);
		else
			s2 = Vec3(1,1,-(normal.x + normal.y) / normal.z);

		const Vec3 s = ray.start - (s2 + polygon.vertex[0]);

		if(std::abs(d * normal) < EPS)//レイと平面が並行
			return nullptr;

		const R t = -(s * normal) / (d * normal);

		if(t < EPS)//平面が視線の後側
			return nullptr;

		Vec3 intersection = ray.start + t * d;

		int m = polygon.vertex.size();
		for(int i = 0;i < m;i++){
			if(!((cross(polygon.vertex[(i + 1) % m] - polygon.vertex[i],intersection - polygon.vertex[(i + 1) % m])).normalized() == normal)){
				return nullptr;
			}
		}
		return new Polygon_info(t,index);*/
		
	}
