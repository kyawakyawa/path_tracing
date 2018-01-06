#pragma once

#include<vector>

#include "Vec3.h"

struct Node_BVH {
	int children[2];
	R aabb[2][3];

	int polygon_index = -1;

	Node_BVH(){
		for(int i = 0;i < 3;i++){
			aabb[0][i] = -1000000000000.0f;
			aabb[1][i] = 1000000000000.0f;
		}
		children[0] = -1;
		children[1] = -1;
	}
};

struct AABB {
	R aabb[2][3];//0 max 1 min
	AABB() {
		for(int i = 0;i < 3;i++){
			aabb[0][i] = -1000000000000.0f;
			aabb[1][i] = 1000000000000.0f;
		}
	}
};

struct Link{
	int hit;
	int miss;
	int aabb_index;

	int polygon_index = -1;

	Link() = default;
	Link(const Link &obj) = default;
};

struct BVH_Prim {
	int index;
	R aabb[3][3];//0 max 1 min 2 cen
	BVH_Prim() {
		for(int i = 0;i < 3;i++){
			aabb[0][i] = -1000000000000.0f;
			aabb[1][i] = 1000000000000.0f;
		}
	}
};