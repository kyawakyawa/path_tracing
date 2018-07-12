#pragma once

#include <vector>
#include <chrono>
#include "../Light_source/Light_source.hpp"
#include "../Information/Intersection_info.hpp"
#include "../Light_source/Infinity_light_source.hpp"
#include "../Random/Random.hpp"

struct Radiance {
    std::vector<Shape*> *shapes = nullptr;
    std::vector<Light_source*> *lights = nullptr;

    Radiance() = default;

    virtual FColor radiance(const Ray &ray) const = 0;

    virtual void set(std::vector<Shape*> *s,std::vector<Light_source*> *l) = 0;

    inline Intersection_info* get_intersection_of_nearest(const Ray &ray) const {
		R min_t = INF;
		Intersection_info *intersection_info = new Intersection_info();

		for(Shape *shape : *shapes){
			Intersection_point *intersection = shape->get_intersection(ray);

			if(intersection != nullptr && min_t > intersection->distance){
				intersection_info->rewrite(shape,intersection);
				min_t = intersection->distance;
			}else
				delete intersection;
		}

		if(intersection_info->shape == nullptr){
			delete intersection_info;
			return nullptr;
		}

		return intersection_info;
    }

	inline bool is_shadow(const Ray &ray,const Vec3 light_point) const {
		const R max_t = (light_point - ray.start).abs() - 0.1;
		for(Shape *shape : *shapes){
			Intersection_point *intersection = shape->get_intersection(ray);
			if(intersection != nullptr && max_t > intersection->distance ){
				delete intersection;
				return true;
			}
			delete intersection;
		}
		return false;
    }

	virtual ~Radiance() = default;
};