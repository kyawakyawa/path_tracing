#pragma once

#include <vector>
#include <chrono>
#include "../Shape/Shape.hpp"
#include "../Light_source/Area_light_source.hpp"
#include "../Information/Intersection_info.hpp"
#include "../Information/Light_info.hpp"
#include "../Light_source/Infinity_light_source.hpp"
#include "../Random/Random.hpp"
#include "../Radiance/Radiance.hpp"
#include "../Radiance/Normal_render.hpp"
#include "../Camera/Camera.hpp"

struct Scene{
	std::vector<Shape*> shapes;//物体
	std::vector<Light_source*> lights;//光源
	const int threads = 4; //スレッドの数
	Camera *camera = nullptr;

	inline Scene(Camera *c,Radiance *rad) : camera(c) {
		camera->set_radiance(rad);
	}

	inline void add(Shape *shape){//物体を追加する
		shapes.push_back(shape);
	}

	inline void add_as_light(Shape *shape){
		add(shape);

		lights.push_back(new Area_light_source(shape));
	}

	void draw(int sample) {
		camera->sensitization(&shapes,&lights,sample,threads);

		camera->out_ppm_stdio();
	}

	~Scene(){
		for(Shape *shape : shapes)
			delete shape;
		for(Light_source *light : lights)
			delete light;
		delete camera;
	}
};
