#pragma once

#include<vector>

#include "../Vec3.h"

struct Node_BVH {
	//std::vector< int > children;
	int children[2];
	R aabb_max[3];
	R aabb_min[3];

	int polygon_index = -1;

	Node_BVH(){
		for(int i = 0;i < 3;i++){
			aabb_max[i] = -1000000000000.0;
			aabb_min[i] = 1000000000000.0;
		}
		children[0] = -1;
		children[1] = -1;
	}
};

struct Node_Thread_BVH{
	int hit;
	int miss;
	R aabb_max[3];
	R aabb_min[3];

	int polygon_index = -1;

	Node_Thread_BVH() = default;
	Node_Thread_BVH(const Node_Thread_BVH &obj) = default;
};