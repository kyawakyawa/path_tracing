#pragma once

#include<vector>

#include "../Vec3.h"

struct Node_BVH {
	std::vector< int > children;
	int child = -1;
	R aabb_max[3];
	R aabb_min[3];

	int polygon_index = -1;

	Node_BVH(){
		for(int i = 0;i < 3;i++){
			aabb_max[i] = -1000000000000.0;
			aabb_min[i] = 1000000000000.0;
		}
		children.push_back(-1);
		children.push_back(-1);
	}
	//Node_BVH(int p) : polygon_index(p){};
	//Node_BVH(int l,int r) : left(l),right(r){};
	Node_BVH(const Node_BVH  &obj){
		children = obj.children;
		child = obj.child;
		for(int i = 0;i < 3;i++){
			aabb_max[i] = obj.aabb_max[i];
			aabb_min[i] = obj.aabb_min[i];
		}
		polygon_index = obj.polygon_index;
	}
};
