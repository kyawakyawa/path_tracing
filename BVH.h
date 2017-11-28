#pragma once

#include <vector>
#include <algorithm>

#include "tiny_obj_loader.h"

#include "Vec3.h"
#include "Intersection_point.h"
#include "Node_BVH.h"

typedef std::vector< Vec3 > Polygon;


struct CompX{//インライン展開できるよう関数オブジェクトを使う
	bool operator()(Polygon &a,Polygon &b){
		R max_x_a = -1000000000.0;
		R max_x_b = -1000000000.0;

		for(auto v : a){
			max_x_a = std::max(max_x_a,v.x);
		}
		for(auto v : b){
			max_x_b = std::max(max_x_b,v.x);
		}

		return max_x_a < max_x_b;
	}
} compX;

struct CompY{
	bool operator()(Polygon &a,Polygon &b){
		R max_y_a = -1000000000.0;
		R max_y_b = -1000000000.0;

		for(auto v : a){
			max_y_a = std::max(max_y_a,v.y);
		}
		for(auto v : b){
			max_y_b = std::max(max_y_b,v.y);
		}

		return max_y_a < max_y_b;
	}

} compY;

struct CompZ{
	bool operator()(Polygon &a,Polygon &b){
		R max_z_a = -1000000000.0;
		R max_z_b = -1000000000.0;

		for(auto v : a){
			max_z_a = std::max(max_z_a,v.z);
		}
		for(auto v : b){
			max_z_b = std::max(max_z_b,v.z);
		}

		return max_z_a < max_z_b;
	}
} compZ;

struct BVH {

	std::vector< Node_BVH > nodes;
	int root = 0;

	BVH() = default;
	BVH(std::vector< Polygon > &polygons) : nodes(polygons.size() * 2){
		root = constraction(polygons,polygons.begin(),polygons.end(),0);
	};

	void constraction(std::vector< Polygon > &polygons){
		root = constraction(polygons,polygons.begin(),polygons.end(),0);
	}

	int constraction(std::vector< Polygon > &polygons,std::vector< Polygon >::iterator left,std::vector< Polygon >::iterator right,int depth){
		//depth %= 3;
		int n = std::distance(left,right);

		Node_BVH node;

		for(auto itr = left;itr != right;itr++){
			for(const auto &v : *itr){
				node.aabb_max[0] = std::max(node.aabb_max[0],v.x);
				node.aabb_max[1] = std::max(node.aabb_max[1],v.y);
				node.aabb_max[2] = std::max(node.aabb_max[2],v.z);

				node.aabb_min[0] = std::min(node.aabb_min[0],v.x);
				node.aabb_min[1] = std::min(node.aabb_min[1],v.y);
				node.aabb_min[2] = std::min(node.aabb_min[2],v.z);
			}
		}

		if(n == 1){
			node.polygon_index = std::distance(polygons.begin(),left);
			nodes.push_back(node);
			return nodes.size() - 1;
		}

		R max_w = node.aabb_max[0] - node.aabb_min[0];
		int max_i = 0;
		for(int i = 1;i < 3;i++){
			if(max_w < node.aabb_max[i] - node.aabb_min[i]){
				max_w = node.aabb_max[i] - node.aabb_min[i];
				max_i = i;
			}
		}

		if(max_i == 0)
			std::sort(left,right,compX);
		else if(max_i == 1)
			std::sort(left,right,compY);
		else
			std::sort(left,right,compZ);
		
		if(n / 2 > 0){
			node.children[0] = constraction(polygons,left,left + n / 2 , depth + 1);
		}
		if(std::distance(left + n / 2,right) > 0){
			node.children[1] = constraction(polygons,left + n / 2,right,depth + 1);
		}
		nodes.push_back(node);
		return nodes.size() - 1;
	}

	/*static bool compX(Polygon &a,Polygon &b){
		R max_x_a = -1000000000.0;
		R max_x_b = -1000000000.0;

		for(auto v : a){
			max_x_a = std::max(max_x_a,v.x);
		}
		for(auto v : b){
			max_x_b = std::max(max_x_b,v.x);
		}

		return max_x_a < max_x_b;
	}
	static bool compY(Polygon &a,Polygon &b){
		R max_y_a = -1000000000.0;
		R max_y_b = -1000000000.0;

		for(auto v : a){
			max_y_a = std::max(max_y_a,v.y);
		}
		for(auto v : b){
			max_y_b = std::max(max_y_b,v.y);
		}

		return max_y_a < max_y_b;
	}
	static bool compZ(Polygon &a,Polygon &b){
		R max_z_a = -1000000000.0;
		R max_z_b = -1000000000.0;

		for(auto v : a){
			max_z_a = std::max(max_z_a,v.z);
		}
		for(auto v : b){
			max_z_b = std::max(max_z_b,v.z);
		}

		return max_z_a < max_z_b;
	}*/

	Intersection_point* traverse(const Ray &ray,const std::vector< Polygon > &polygons) const{
		return traverse(ray,polygons,root);
	}

	Intersection_point* traverse(const Ray &ray,const std::vector< Polygon > &polygons,const int now) const {
		const Node_BVH &node = nodes[now];
		if(node.polygon_index > -1){
			return polygon_intersection(ray,polygons[nodes[now].polygon_index]);
		}

		R t_max = 1000000000000.0;
		R t_min = -1000000000000.0;
		R start[3];start[0] = ray.start.x;start[1] = ray.start.y;start[2] = ray.start.z;
		R direction[3];direction[0] = ray.direction.x;direction[1] = ray.direction.y;direction[2] = ray.direction.z;

		for(int i = 0;i < 3;i++){
			R t1 = (node.aabb_min[i] - start[i]) / direction[i];
			R t2 = (node.aabb_max[i] - start[i]) / direction[i];
			R t_near = std::min(t1,t2);
			R t_far = std::max(t1,t2);
			t_max = std::min(t_max,t_far);
			t_min = std::max(t_min,t_near);
		}

		if((t_min - t_max > EPS))
			return nullptr;

		Intersection_point *left = traverse(ray,polygons,node.children[0]);
		Intersection_point *right = traverse(ray,polygons,node.children[1]);

		if(left == nullptr)
			return right;
		else if(right == nullptr){
			return left;
		}else{
			if(left->distance < right->distance){
				delete right;
				return left;
			}else{
				delete left;
				return right;
			}
		}
	}

	static inline Intersection_point* polygon_intersection(const Ray &ray,const Polygon &polygon) {
		const Vec3 normal = (cross(polygon[1] - polygon[0],polygon[2] - polygon[1])).normalized();

		const Vec3 &d = ray.direction;
		Vec3 s2;

		////平面上のもう一つの天を取得
		if(std::abs(normal.x) > EPS)
			s2 = Vec3(-(normal.y + normal.z) / normal.x,1,1);
		else if(std::abs(normal.y) > EPS)
			s2 = Vec3(1,-(normal.x + normal.z) / normal.y,1);
		else
			s2 = Vec3(1,1,-(normal.x + normal.y) / normal.z);

		const Vec3 s = ray.start - (s2 + polygon[0]);

		if(std::abs(d * normal) < EPS)//レイと平面が並行
			return nullptr;

		const R t = -(s * normal) / (d * normal);

		if(t < EPS)//平面が視線の後側
			return nullptr;

		Vec3 intersection = ray.start + t * d;

		int m = polygon.size();
		for(int i = 0;i < m;i++){
			if(!((cross(polygon[(i + 1) % m] - polygon[i],intersection - polygon[(i + 1) % m])).normalized() == normal)){
				return nullptr;
			}
		}
		return new Intersection_point(t,ray.start + t * d,normal);
	}

	void out() const{
		for(auto &node : nodes){
			for(int i = 0;i < 3;i++){
				printf("max%d %f min%d %f ",i,node.aabb_max[i],i,node.aabb_min[i]);
			}
			printf("\n");
		}
		printf("\n");
	}
};
