#pragma once

#include <vector>

#include "tiny_obj_loader.h"

#include "Vec3.h"
#include "Shape.h"
#include "BVH.h"
#include "Polygon.h"

//typedef std::vector< Vec3 > Polygon;

struct Mesh : public Shape{

	std::vector< Polygon > polygons;
	BVH bvh;

	Mesh() = delete;
	inline Mesh(const tinyobj::attrib_t &at,const std::vector< tinyobj::shape_t > &shs,const Material &m): Shape(m){
		for(const auto &sh : shs){
			int p = 0;
			for(const auto &nfv : sh.mesh.num_face_vertices){
				Polygon poly;
				for(int i = 0;i < nfv;i++){
					poly.vertex.push_back(get_vertice(at,sh.mesh.indices[p].vertex_index));
					p++;
				}
				polygons.push_back(poly);
			}
		}

		bvh.constraction(polygons);
		/*for(const auto &polygon : polygons){
			for(const auto &v : polygon){
				std::cout << v << " ";
			}
			std::cout << std::endl;
		}*/
	};

	inline Vec3 get_vertice(const tinyobj::attrib_t &at,int index){
		//return Vec3(at.vertices[index * 3],at.vertices[index * 3 + 1],at.vertices[index * 3 + 2]) * 30 + Vec3(50,10,70);//bunny
		//return Vec3(at.vertices[index * 3],at.vertices[index * 3 + 1],at.vertices[index * 3 + 2]) * 70 + Vec3(70,20,50);//dragon
		//return Vec3(at.vertices[index * 3],at.vertices[index * 3 + 1],at.vertices[index * 3 + 2]) * (R)(0.2) + Vec3(50,10,50);//car
		//return Vec3(at.vertices[index * 3],at.vertices[index * 3 + 1],at.vertices[index * 3 + 2]) + Vec3(50,60,70);//sibenik
		return Vec3(at.vertices[index * 3],at.vertices[index * 3 + 1],at.vertices[index * 3 + 2]) * 70 + Vec3(50,30,60);//buddha
	}

	static inline Intersection_point* polygon_intersection(const Ray &ray,const Polygon &polygon) {
		const Vec3 normal = (cross(polygon.vertex[1] - polygon.vertex[0],polygon.vertex[2] - polygon.vertex[1])).normalized();

		const Vec3 &d = ray.direction;
		Vec3 s2;

		////平面上のもう一つの天を取得
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
		return new Intersection_point(t,ray.start + t * d,normal);
	}
	
	inline Intersection_point* get_intersection(const Ray &ray) const {
		/*Intersection_point *ret = nullptr;
		R mint = 1000000000.0;

		for(const auto &polygon : polygons){
			Intersection_point *p = polygon_intersection(ray,polygon);
			if(p != nullptr && p->distance < mint){
				mint = p->distance;
				delete ret;
				ret = p;
			}
		}

		return ret;*/

		return bvh.traverse(ray,polygons);
	}

	inline Material get_material(const Vec3 &position) const {
		return material;
	}
};
