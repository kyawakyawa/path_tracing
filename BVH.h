#pragma once

#include <vector>
#include <algorithm>

#include "tiny_obj_loader.h"

#include "Vec3.h"
#include "Node_BVH.h"
#include "Polygon.h"
#include "Polygon_info.h"

//typedef std::vector< Vec3 > Polygon;


struct CompX{//インライン展開できるよう関数オブジェクトを使う
	bool operator()(const Polygon &a,const Polygon &b){
		return a.aabb_max[0] < b.aabb_min[0];
	}
} compX;

struct CompY{
	bool operator()(const Polygon &a,const Polygon &b){
		return a.aabb_max[1] < b.aabb_min[1];
	}
} compY;

struct CompZ{
	bool operator()(const Polygon &a,const Polygon &b){
		return a.aabb_max[2] < b.aabb_min[2];
	}
} compZ;

struct BVH {

	std::vector< Node_BVH > nodes;
	int root = 0;

	BVH() = default;
	//BVH(std::vector< Material > &ms): materials(ms){};
	/*BVH(std::vector< Polygon > &polygons,std::vector< Material > &ms) : nodes(polygons.size() * 2){
		construction(polygons);
	};*/

	void construction(std::vector< Polygon > &polygons){
		make_polygons_aabbs(polygons);
		int n = 1;
		while(polygons.size() > n){
			n *= 2;
		}
		nodes.resize(n * 2 + 100);
		construction(polygons,polygons.begin(),polygons.end(),1);
	}

	void make_polygons_aabbs(std::vector< Polygon > &polygons){
		for(auto &polygon : polygons){
			for(auto & v : polygon.vertex){
				polygon.aabb_max[0] = std::max(polygon.aabb_max[0],v.x);
				polygon.aabb_max[1] = std::max(polygon.aabb_max[1],v.y);
				polygon.aabb_max[2] = std::max(polygon.aabb_max[2],v.z);

				polygon.aabb_min[0] = std::min(polygon.aabb_min[0],v.x);
				polygon.aabb_min[1] = std::min(polygon.aabb_min[1],v.y);
				polygon.aabb_min[2] = std::min(polygon.aabb_min[2],v.z);
			}
		}
	}

	void construction(std::vector< Polygon > &polygons,std::vector< Polygon >::iterator left,std::vector< Polygon >::iterator right,int now){
		int n = std::distance(left,right);
		Node_BVH node;

		for(auto itr = left;itr != right;itr++){//aabbのマージ
			for(int i = 0;i < 3;i++){
				node.aabb_max[i] = std::max(node.aabb_max[i],itr->aabb_max[i]);
				node.aabb_min[i] = std::min(node.aabb_min[i],itr->aabb_min[i]);
			}
		}

		if(n == 1){//葉
			node.polygon_index = std::distance(polygons.begin(),left);
			nodes[now] = node;//nodes.push_back(node);
			return ;//nodes.size() - 1;
		}

		R max_w = node.aabb_max[0] - node.aabb_min[0];//一番長い軸でソート
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
			construction(polygons,left,left + n / 2 ,now * 2);
		}
		if(std::distance(left + n / 2,right) > 0){
			construction(polygons,left + n / 2,right,now * 2 + 1);
		}
		nodes[now] = node;//nodes.push_back(node);
		//return nodes.size() - 1;
	}

	/*int constraction(std::vector< Polygon > &polygons,std::vector< Polygon >::iterator left,std::vector< Polygon >::iterator right){
		int n = std::distance(left,right);

		Node_BVH node;

		for(auto itr = left;itr != right;itr++){
			for(int i = 0;i < 3;i++){
				node.aabb_max[i] = std::max(node.aabb_max[i],itr->aabb_max[i]);
				node.aabb_min[i] = std::min(node.aabb_min[i],itr->aabb_min[i]);
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
			node.children[0] = constraction(polygons,left,left + n / 2 );
		}
		if(std::distance(left + n / 2,right) > 0){
			node.children[1] = constraction(polygons,left + n / 2,right);
		}
		nodes.push_back(node);
		return nodes.size() - 1;
	}*/

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

	Polygon_info* traverse(const Ray &ray,const std::vector< Polygon > &polygons) const{
		R start[3];start[0] = ray.start.x;start[1] = ray.start.y;start[2] = ray.start.z;
		R direction[3];direction[0] = 1.0 / ray.direction.x;direction[1] = 1.0 / ray.direction.y;direction[2] = 1.0 / ray.direction.z;
		return traverse(ray,polygons,1,start,direction,1000000000.0);
	}

	/*Intersection_point* traverse(const Ray &ray,const std::vector< Polygon > &polygons,const int now) const {
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
	}*/

	Polygon_info* traverse(const Ray &ray,const std::vector< Polygon > &polygons,const int now,const R start[],const R direction[],R min_d) const {
		const Node_BVH &node = nodes[now];

		R t_max = 1000000000000.0;
		R t_min = -1000000000000.0;
		//R start[3];start[0] = ray.start.x;start[1] = ray.start.y;start[2] = ray.start.z;
		//R direction[3];direction[0] = ray.direction.x;direction[1] = ray.direction.y;direction[2] = ray.direction.z;

		#pragma omp simd
		for(int i = 0;i < 3;i++){
			R t1 = (node.aabb_min[i] - start[i]) /* / */ * direction[i];
			R t2 = (node.aabb_max[i] - start[i]) /* / */ * direction[i];
			//R t_near = std::min(t1,t2);
			R t_near = (t1 < t2) ? t1 : t2;
			//R t_far = std::max(t1,t2);
			R t_far = (t1 > t2) ? t1 : t2;
			if(t_max >  t_far)t_max = t_far;
			//t_max = std::min(t_max,t_far);
			//(t_max > t_far) ? t_max = t_far : 0 ;
			if(t_min < t_near)t_min = t_near;
			//t_min = std::max(t_min,t_near);
			//(t_min < t_near) ? t_min = t_near : 0 ;
		}

		if((t_min - t_max > EPS))
			return nullptr;
		if(min_d < t_min)
			return nullptr;

		if(node.polygon_index > -1){
			return polygon_intersection(ray,polygons[nodes[now].polygon_index],node.polygon_index);
		}

		Polygon_info *left = traverse(ray,polygons,now * 2,start,direction,min_d);
		if(left != nullptr && min_d > left->distance)
			min_d = left->distance;
		Polygon_info *right = traverse(ray,polygons,now * 2 + 1,start,direction,min_d);

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

	static inline Polygon_info* polygon_intersection(const Ray &ray,const Polygon &polygon,const int index) {
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
