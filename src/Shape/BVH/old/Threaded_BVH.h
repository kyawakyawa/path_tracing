#pragma once

#include <vector>
#include <queue>
#include <algorithm>
#include<chrono>
#include <tbb/parallel_sort.h>

#include "../Vec3.h"
#include "Node_Threaded_BVH.h"
#include "../Polygon.h"
#include "../Polygon_info.h"



/*struct CompX{//インライン展開できるよう関数オブジェクトを使う
	bool operator()(Polygon &a,Polygon &b){
		return a.aabb_max[0] < b.aabb_min[0];
	}
} compX;

struct CompY{
	bool operator()(Polygon &a,Polygon &b){
		return a.aabb_max[1] < b.aabb_min[1];
	}
} compY;

struct CompZ{
	bool operator()(Polygon &a,Polygon &b){
		return a.aabb_max[2] < b.aabb_min[2];
	}
} compZ;*/

static bool compX(Polygon &a,Polygon &b){
	return a.aabb_max[0] < b.aabb_min[0];
}
static bool compY(Polygon &a,Polygon &b){
	return a.aabb_max[1] < b.aabb_min[1];
}
static bool compZ(Polygon &a,Polygon &b){
	return a.aabb_max[2] < b.aabb_min[2];
}

int BVH_count = 0;
int BVH_count_traverse = 0;
int BVH_count_polygon_intersection = 0;
unsigned long long BVH_time_polygon_intersection = 0;

struct BVH {

	std::vector< Node_BVH > nodes;
	Node_Thread_BVH *threaded_nodes = nullptr;
	int root = 0;

	BVH() = default;

	void construction(std::vector< Polygon > &polygons){
		make_polygons_aabbs(polygons);
		int n = 1;
		while(polygons.size() > n){
			n *= 2;
		}
		//nodes.resize(n * 2 + 100);
		root = construction(polygons,polygons.begin(),polygons.end());
		node_sort();
		threaded_nodes = new Node_Thread_BVH[nodes.size()];
		make_Threaded_BVH(root,-1);
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

	int construction(std::vector< Polygon > &polygons,const std::vector< Polygon >::iterator left,const std::vector< Polygon >::iterator right){
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
			nodes.push_back(node);
			return nodes.size() - 1;
		}

		R max_w = node.aabb_max[0] - node.aabb_min[0];//一番長い軸でソート
		int max_i = 0;
		for(int i = 1;i < 3;i++){
			if(max_w < node.aabb_max[i] - node.aabb_min[i]){
				max_w = node.aabb_max[i] - node.aabb_min[i];
				max_i = i;
			}
		}

		std::chrono::system_clock::time_point start,end;
    	start = std::chrono::system_clock::now();
		if(max_i == 0)
			//std::sort(left,right,compX);
			tbb::parallel_sort(left,right,compX);
		else if(max_i == 1)
			tbb::parallel_sort(left,right,compY);
		else
			tbb::parallel_sort(left,right,compZ);
		end = std::chrono::system_clock::now();

		if(left == polygons.begin() && right == polygons.end()){
	    	auto elapsed = std::chrono::duration_cast< std::chrono::milliseconds >(end - start).count();
	    	std::cerr << "SORT " << elapsed <<"ms"<< std::endl;
		}

		/*int mini_i = 0;R mini_cost = 1000000000000.0;//costがnanになったらx方向にする
		for(int i = 0;i < 3;i++){
			if(i == 0)
				std::sort(left,right,compX);
			else if(i == 1)
				std::sort(left,right,compY);
			else
				std::sort(left,right,compZ);
			R cost;
			divide_point(left,right,aabb_S(node.aabb_max,node.aabb_min),&cost);
			if(mini_cost > cost){
				mini_cost = cost;
				mini_i = i;
			}
		}

		if(mini_i == 0)
			std::sort(left,right,compX);
		else if(mini_i == 1)
			std::sort(left,right,compY);
		else
			std::sort(left,right,compZ);*/

		const int center = divide_point(left,right,aabb_S(node.aabb_max,node.aabb_min),nullptr);

		/*if(center < 1 || center > n - 1){
			std::cerr << "不正な分割です" << std::endl;
		}*/
		
		if(center > 0){
			node.children[0] = construction(polygons,left,left + center);
		}
		if(std::distance(left + center,right) > 0){
			node.children[1] = construction(polygons,left + center,right);
		}
		nodes.push_back(node);
		return nodes.size() - 1;
	}

	static inline int divide_point(const std::vector< Polygon >::iterator left,const std::vector< Polygon >::iterator right,const R A_S,R *ret_cost) {
		const int n = std::distance(left,right) - 1;
		R *A_S1 = new R[n];
		Polygon aabb1;
		for(int i = 0;i < n;i++){
			for(int j = 0;j < 3;j++){
				aabb1.aabb_max[j] = std::max(aabb1.aabb_max[j],(left + i)->aabb_max[j]);
				aabb1.aabb_min[j] = std::min(aabb1.aabb_min[j],(left + i)->aabb_min[j]);
			}
			A_S1[i] = aabb_S(aabb1.aabb_max,aabb1.aabb_min);
		}
		R  *A_S2 = new R[n];
		Polygon aabb2;
		for(int i = 0;i < n;i++){
			for(int j = 0;j < 3;j++){
				aabb2.aabb_max[j] = std::max(aabb2.aabb_max[j],(right - i - 1)->aabb_max[j]);
				aabb2.aabb_min[j] = std::min(aabb2.aabb_min[j],(right - i - 1)->aabb_min[j]);
			}
			A_S2[n - i - 1] = aabb_S(aabb2.aabb_max,aabb2.aabb_min);
		}

		const R Taabb = 2.0;const R Ttri = 1.0;const R A_S_inv = 1.0 / A_S;
		R mini = 2 * Taabb + A_S1[0] * A_S_inv * (1) * Ttri + A_S2[0] * A_S_inv * (n) * Ttri; 
		int mini_i = 0;
		for(int i = 1;i < n;i++){
			const R cost = 2 * Taabb + A_S1[i] * A_S_inv * (i + 1) * Ttri + A_S2[i] * A_S_inv * (n - i) * Ttri;//aabbが小さい時はnanになることもあるよ
			if(mini > cost){
				mini = cost;
				mini_i = i;
			}
		}

		delete[] A_S1;
		delete[] A_S2;
		if(ret_cost != nullptr)
			*ret_cost = mini;
		return mini_i + 1;
	}

	void node_sort(){
		std::vector< Node_BVH > copy = nodes;
		nodes.clear();

		std::queue< int > que;

		que.push(root);

		while(!que.empty()){
			int now = que.front();
			que.pop();
			Node_BVH node = copy[now];

			if(node.polygon_index > -1){
				nodes.push_back(node);
				continue;
			}

			node.children[0] = que.size() + nodes.size() + 1;
			node.children[1] = que.size() + nodes.size() + 2;
			nodes.push_back(node);
			que.push(copy[now].children[0]);
			que.push(copy[now].children[1]);
		}
		root = 0;
	}

	void make_Threaded_BVH(const int now,const int nearest_right){
		Node_BVH &node = nodes[now];
		Node_Thread_BVH &threaded_node = threaded_nodes[now];
		for(int i = 0;i < 3;i++){
			threaded_node.aabb_max[i] = node.aabb_max[i];
			threaded_node.aabb_min[i] = node.aabb_min[i];
		}
		threaded_node.polygon_index = node.polygon_index;

		if(node.polygon_index == -1){//子がいる
			/*const Node_BVH &left = nodes[node.children[0]];
			const Node_BVH &right = nodes[node.children[1]];

			if(left.aabb_min[2] > right.aabb_min[2]){
				std::swap(node.children[0],node.children[1]);
			}*/

			threaded_node.hit = node.children[0];
		}else{
			threaded_node.hit = nearest_right;
		}
		threaded_node.miss = nearest_right;

		if(node.polygon_index > -1)
			return;

		make_Threaded_BVH(node.children[0],node.children[1]);
		make_Threaded_BVH(node.children[1],nearest_right);
	}

	static R aabb_S(R aabb_max[],R aabb_min[]) {
		R d[3];
		for(int i = 0;i < 3;i++){
			d[i] = aabb_max[i] - aabb_min[i];
		}
		R ret = 0;
		for(int i = 0;i < 3;i++){
			for(int j = i + 1;j < 3;j++){
				ret += d[i] * d[j];
			}
		}
		return 2.0 * ret;
	}

	Polygon_info* traverse(const Ray &ray,const std::vector< Polygon > &polygons) const{
		R start[3];start[0] = ray.start.x;start[1] = ray.start.y;start[2] = ray.start.z;
		R direction[3];direction[0] = 1.0 / ray.direction.x;direction[1] = 1.0 / ray.direction.y;direction[2] = 1.0 / ray.direction.z;
		R min_d = 1000000000000.0;
		Polygon_info *ret = nullptr;

		BVH_count++;

		int now = root;
		while(now != -1){
			const Node_Thread_BVH &node = threaded_nodes[now];
			BVH_count_traverse++;

			R t_max = 1000000000000.0;
			R t_min = -1000000000000.0;

			#pragma omp simd
			for(int i = 0;i < 3;i++){
				R t1 = (node.aabb_min[i] - start[i]) * direction[i];
				R t2 = (node.aabb_max[i] - start[i]) * direction[i];
				R t_near = (t1 < t2) ? t1 : t2;
				R t_far = (t1 > t2) ? t1 : t2;
				if(t_max >  t_far)t_max = t_far;
				if(t_min < t_near)t_min = t_near;
			}

			if((t_min - t_max > EPS) || (min_d < t_min)){
				now = node.miss;
			}else{
				if(node.polygon_index > -1){

					std::chrono::system_clock::time_point start,end;
    				start = std::chrono::system_clock::now();
					Polygon_info *intersection = polygon_intersection(ray,polygons[node.polygon_index],node.polygon_index);
					end = std::chrono::system_clock::now();
					auto elapsed = std::chrono::duration_cast< std::chrono::nanoseconds >(end - start).count();
					BVH_time_polygon_intersection += elapsed;

					BVH_count_polygon_intersection++;
					if(intersection != nullptr && (ret == nullptr || intersection->distance < ret->distance)){
						delete ret;
						ret = intersection;
						min_d = intersection->distance;
					}
				}
				now = node.hit;
			}
		}
		return ret;
	}

	static inline Polygon_info* polygon_intersection(const Ray &ray,const Polygon &polygon,const int index);
	/*static inline Polygon_info* polygon_intersection(const Ray &ray,const Polygon &polygon,const int index) {
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
	
	}*/

	void out() const{
		for(auto &node : nodes){
			for(int i = 0;i < 3;i++){
				printf("max%d %f min%d %f ",i,node.aabb_max[i],i,node.aabb_min[i]);
			}
			printf("\n");
		}
		printf("\n");
	}


	~BVH(){
		delete threaded_nodes;
	}
};

#include "polygon_intersection.h"