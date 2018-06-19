#pragma once

#include <vector>
#include <queue>
#include <algorithm>
#include <chrono>
#include <tbb/parallel_sort.h>

#include "Vec3.h"
#include "BVH_data_structures.h"
#include "Prim.h"



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

static bool compX(BVH_Prim &a,BVH_Prim &b){
	//return a.aabb[2][0] < b.aabb[2][0];
	return a.aabb[0][0] < b.aabb[1][0];
}
static bool compY(BVH_Prim &a,BVH_Prim &b){
	//return a.aabb[2][1] < b.aabb[2][1];
	return a.aabb[0][1] < b.aabb[1][1];
}
static bool compZ(BVH_Prim &a,BVH_Prim &b){
	//return a.aabb[2][2] < b.aabb[2][2];
	return a.aabb[0][2] < b.aabb[1][2];
}

int BVH_count = 0;
int BVH_count_traverse = 0;
int BVH_count_polygon_intersection = 0;
unsigned long long BVH_time_polygon_intersection = 0;

struct BVH {

	std::vector< Node_BVH > nodes;
	Link *links[6];// 0 +X 1 -X 2 +Y 3 -Y 4 +Z 5 -Z
	int links_num = 0;
	BVH_Prim *bvh_prims = nullptr;
	int bvh_prims_num = 0;
	AABB *aabbs = nullptr;
	int root = 0;

	BVH() {
		for(int i = 0;i < 6;i++){
			links[i] = nullptr;
		}
	}

	void construction(const Vec3 *vertices,const Prim *prims,const int prims_num){
		prims_copy(vertices,prims,prims_num);
		bvh_prims_num = prims_num;
		root = construction(0,prims_num);
		node_sort();
		links_num = nodes.size();
		for(int i = 0;i < 6;i++){
			links[i] = new Link[links_num];
			make_link(i,root,-1);
		}
		set_aabbs();
		root = 0;
		nodes.clear();
	}

	void prims_copy(const Vec3 *vertices,const Prim *prims,const int prims_num) {
		bvh_prims = new BVH_Prim[prims_num];
		for(int i = 0;i < prims_num;i++){
			bvh_prims[i].index = i;
			for(int j = 0;j < 3;j++) {
				bvh_prims[i].aabb[0][0] = std::max(bvh_prims[i].aabb[0][0],vertices[prims[i].vertices_index[j]].x);
				bvh_prims[i].aabb[0][1] = std::max(bvh_prims[i].aabb[0][1],vertices[prims[i].vertices_index[j]].y);
				bvh_prims[i].aabb[0][2] = std::max(bvh_prims[i].aabb[0][2],vertices[prims[i].vertices_index[j]].z);

				bvh_prims[i].aabb[1][0] = std::min(bvh_prims[i].aabb[1][0],vertices[prims[i].vertices_index[j]].x);
				bvh_prims[i].aabb[1][1] = std::min(bvh_prims[i].aabb[1][1],vertices[prims[i].vertices_index[j]].y);
				bvh_prims[i].aabb[1][2] = std::min(bvh_prims[i].aabb[1][2],vertices[prims[i].vertices_index[j]].z);
			}

			//for(int j = 0;j < 3;j++){
				//bvh_prims[i].aabb[2][j] = (bvh_prims[i].aabb[0][j] + bvh_prims[i].aabb[1][j]) * 0.5;
			//}
		}
	}

	int construction(const int left,const int right){
		int n = right - left;

		Node_BVH node;

		for(int i = left;i < right;i++){//aabbのマージ
			for(int j = 0;j < 3;j++){
				node.aabb[0][j] = std::max(node.aabb[0][j],bvh_prims[i].aabb[0][j]);
				node.aabb[1][j] = std::min(node.aabb[1][j],bvh_prims[i].aabb[1][j]);
			}
		}

		if(n == 1){ //葉
			node.polygon_index = left;
			nodes.push_back(node);
			return nodes.size() - 1;
		}

		R max_w = node.aabb[0][0] - node.aabb[1][0];//一番長い軸でソート
		int max_i = 0;
		for(int i = 1;i < 3;i++){
			if(max_w < node.aabb[0][i] - node.aabb[1][i]){
				max_w = node.aabb[0][i] - node.aabb[1][i];
				max_i = i;
			}
		}

		if(max_i == 0)
			tbb::parallel_sort(bvh_prims + left,bvh_prims + right,compX);
		else if(max_i == 1)
			tbb::parallel_sort(bvh_prims + left,bvh_prims + right,compY);
		else
			tbb::parallel_sort(bvh_prims + left,bvh_prims + right,compZ);

		/*int mini_i = 0;R mini_cost = 1000000000000.0;//costがnanになったらx方向にする
		for(int i = 0;i < 3;i++){
			if(i == 0)
				tbb::parallel_sort(bvh_prims + left,bvh_prims + right,compX);
			else if(i == 1)
				tbb::parallel_sort(bvh_prims + left,bvh_prims + right,compY);
			else
				tbb::parallel_sort(bvh_prims + left,bvh_prims + right,compZ);
			R cost;
			divide_point(left,right,aabb_S(node.aabb),&cost);
			if(mini_cost > cost){
				mini_cost = cost;
				mini_i = i;
			}
		}

		if(mini_i == 0)
			tbb::parallel_sort(bvh_prims + left,bvh_prims + right,compX);
		else if(mini_i == 1)
			tbb::parallel_sort(bvh_prims + left,bvh_prims + right,compY);
		else
			tbb::parallel_sort(bvh_prims + left,bvh_prims + right,compZ);*/

		const int center = divide_point(left,right,aabb_S(node.aabb),nullptr);

		if(center > 0){
			node.children[0] = construction(left,left + center);
		}
		if(right - left - center > 0){
			node.children[1] = construction(left + center,right);
		}
		nodes.push_back(node);
		return nodes.size() - 1;
	}


	inline int divide_point(const int left,const int right,const R A_S,R *ret_cost) {
		const int n = right - left - 1;
		R *A_S1 = new R[n];
		AABB aabb1;
		for(int i = 0;i < n;i++){
			for(int j = 0;j < 3;j++){
				aabb1.aabb[0][j] = std::max(aabb1.aabb[0][j],bvh_prims[left + i].aabb[0][j]);
				aabb1.aabb[1][j] = std::min(aabb1.aabb[1][j],bvh_prims[left + i].aabb[1][j]);
			}
			A_S1[i] = aabb_S(aabb1.aabb);
		}
		R  *A_S2 = new R[n];
		AABB aabb2;
		for(int i = 0;i < n;i++){
			for(int j = 0;j < 3;j++){
				aabb2.aabb[0][j] = std::max(aabb2.aabb[0][j],bvh_prims[right - i - 1].aabb[0][j]);
				aabb2.aabb[1][j] = std::min(aabb2.aabb[1][j],bvh_prims[right - i - 1].aabb[1][j]);
			}
			A_S2[n - i - 1] = aabb_S(aabb2.aabb);
		}

		const R Taabb = 1.0;const R Ttri = 7.0;const R A_S_inv = 1.0 / A_S;
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

	static R aabb_S(R aabb[2][3]) {
		R d[3];
		for(int i = 0;i < 3;i++){
			d[i] = aabb[0][i] - aabb[1][i];
		}
		R ret = 0;
		for(int i = 0;i < 3;i++){
			for(int j = i + 1;j < 3;j++){
				ret += d[i] * d[j];
			}
		}
		return 2.0 * ret;
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

	void make_link(const int axis,const int now,const int nearest_right){
		Node_BVH &node = nodes[now];
		Link &link = links[axis][now];
		link.aabb_index = now;
		link.polygon_index = node.polygon_index;
		const bool f = is_exchange(node.children[0],node.children[1],axis);
		const int left = f ? node.children[1] : node.children[0];
		const int right = f ? node.children[0] : node.children[1];

		if(node.polygon_index == -1){
			link.hit = left;
		}else{
			link.hit = nearest_right;
		}
		link.miss = nearest_right;

		if(node.polygon_index > -1)
			return;
		make_link(axis,left,right);
		make_link(axis,right,nearest_right);
	}

	inline bool is_exchange(const int l,const int r,const int axis){
		if(l < 0 || r < 0)
			return false;
		const Node_BVH &left = nodes[l];
		const Node_BVH &right = nodes[r];

		switch (axis){
			case 0:
				return left.aabb[0][0] < right.aabb[0][0];
			case 1:
				return left.aabb[1][0] > right.aabb[1][0];
			case 2:
				return left.aabb[0][1] < right.aabb[0][1];
			case 3:
				return left.aabb[1][1] > right.aabb[1][1];
			case 4:
				return left.aabb[0][2] < right.aabb[0][2];
				break;
		}
		return left.aabb[1][2] > right.aabb[1][2];
	}

	void set_aabbs(){
		aabbs = new AABB[nodes.size()];
		for(int i = 0;i < nodes.size();i++){
			for(int j = 0;j < 2;j++){
				for(int k = 0;k < 3;k++){
					aabbs[i].aabb[j][k] = nodes[i].aabb[j][k];
				}
			}
		}
	}

	int traverse(const Ray &ray,const Vec3 *vertices,const Prim *prims) const {
		//R start[3];start[0] = ray.start.x;start[1] = ray.start.y;start[2] = ray.start.z;
		const R start[3] = { ray.start.x , ray.start.y , ray.start.z};
		//R direction[3];direction[0] = 1.0 / ray.direction.x;direction[1] = 1.0 / ray.direction.y;direction[2] = 1.0 / ray.direction.z;
		const R direction[3] = {1.0f / ray.direction.x , 1.0f / ray.direction.y , 1.0f / ray.direction.z};
		R min_d = 1000000000000.0;
		int ret = -1;

		//BVH_count++;

		int now = root;

		int axis = 0;;
		const R dir[3] = {ray.direction.x , ray.direction.y , ray.direction.z };
		for(int i = 1;i < 3;i++){
			if(std::abs(dir[axis]) < std::abs(dir[i]))
				axis = i;
		}
		axis *= 2;
		if(dir[axis >> 1] > 0.0)
			axis++;

		const Link *links = this->links[axis];

		while(now != -1){
			const Link link = links[now];
			//BVH_count_traverse++;

			R t_max = 1000000000000.0;
			R t_min = -1000000000000.0;

			#pragma omp simd
			for(int i = 0;i < 3;i++){
				R t[2];
				for(int j = 0;j < 2;j++){
					t[j] = (aabbs[link.aabb_index].aabb[j][i] - start[i]) * direction[i];
				}
				R t_near = (t[0] < t[1]) ? t[0] : t[1];
				R t_far = (t[0] > t[1]) ? t[0] : t[1];
				if(t_max >  t_far)t_max = t_far;
				if(t_min < t_near)t_min = t_near;
			}

			if((t_min - t_max > EPS) || (min_d < t_min)){
				now = link.miss;
			}else{
				if(link.polygon_index > -1){

					int p_index = bvh_prims[link.polygon_index].index;

					//std::chrono::system_clock::time_point start,end;
    				//start = std::chrono::system_clock::now();

					R distance = polygon_intersection(ray,vertices,prims[p_index]);

					//end = std::chrono::system_clock::now();
					//auto elapsed = std::chrono::duration_cast< std::chrono::nanoseconds >(end - start).count();
					//BVH_time_polygon_intersection += elapsed;

					//BVH_count_polygon_intersection++;

					if(distance > -1 && (ret == -1 || distance < min_d)){
						ret = p_index;
						min_d = distance;
					}
				}
				now = link.hit;
			}
		}
		return ret;
	}

	static inline R polygon_intersection(const Ray &ray,const Vec3 *vertices,const Prim &prim){
		const Vec3 vertex[3] = {vertices[prim.vertices_index[0]] , vertices[prim.vertices_index[1]] , vertices[prim.vertices_index[2]] };

		const Vec3 &r = ray.direction;
		const Vec3 T = ray.start - vertex[0];
		const Vec3 E1 = vertex[1] - vertex[0];
		const Vec3 E2 = vertex[2] - vertex[0];
		const Vec3 P = cross(r,E2);
		const Vec3 Q = cross(T,E1);

		const R bunbo = P * E1;
		if(bunbo < EPS * EPS && bunbo > -EPS * EPS)
			return -10000000000.0;
		const R inv = 1.0 / bunbo;

		const R t = Q * E2 * inv;
		const R u = P * T * inv;
		const R v = Q * r * inv;
		if(t > EPS && u > 0 && v > 0 && u + v < 1.0){
			return t;
		}
		return -1000000000.0;
	}

	~BVH(){
		for(int i = 0;i < 6;i++){
			delete[] links[i];
		}
		delete[] bvh_prims;
		delete[] aabbs;
	}

	BVH(const BVH &obj){
		nodes = obj.nodes;;

		for(int i = 0;i < 6;i++){
			delete[] links[i];
			links_num = obj.links_num;
			links[i] = new Link[links_num];
			for(int j = 0;j < links_num;j++){
				links[i][j] = obj.links[i][j];
			}
		}

		delete[] bvh_prims;
		bvh_prims_num = obj.bvh_prims_num;
		bvh_prims = new BVH_Prim[bvh_prims_num];
		for(int i = 0;i < bvh_prims_num;i++){
			bvh_prims[i] = obj.bvh_prims[i];
		}

		delete[] aabbs;
		aabbs = new AABB[links_num];
		for(int i = 0;i < links_num;i++){
			aabbs[i] = obj.aabbs[i];
		}

		root = obj.root;
	}

	BVH& operator =(const BVH &obj){
		nodes = obj.nodes;;

		for(int i = 0;i < 6;i++){
			delete[] links[i];
			links_num = obj.links_num;
			links[i] = new Link[links_num];
			for(int j = 0;j < links_num;j++){
				links[i][j] = obj.links[i][j];
			}
		}

		delete[] bvh_prims;
		bvh_prims_num = obj.bvh_prims_num;
		bvh_prims = new BVH_Prim[bvh_prims_num];
		for(int i = 0;i < bvh_prims_num;i++){
			bvh_prims[i] = obj.bvh_prims[i];
		}

		delete[] aabbs;
		aabbs = new AABB[links_num];
		for(int i = 0;i < links_num;i++){
			aabbs[i] = obj.aabbs[i];
		}

		root = obj.root;
		return *this;
	}
};

//#include "polygon_intersection.h"
