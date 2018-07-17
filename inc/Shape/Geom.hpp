#pragma once

#include "Shape.hpp"
#include "Prim.hpp"
#include "../BVH/BVH.hpp"
#include "../Random/Random.hpp"

#include "../Input_scene/Scene_data.hpp"

enum Geom_shading_type {
    Gm_STRICT,//通常
    Gm_SMOOTH//頂点法線を平均化して滑らかにする
};

struct Geom : public Shape {
    Geom_shading_type type;
    Vec3 *vertices = nullptr;
    Vec3 *normals = nullptr;
    Vec3 *uvs = nullptr;

    int vertices_num;
    int normals_num;
    int uvs_num;

    Prim *prims = nullptr;
    int prims_num;
    R S = 0.0;

    BVH bvh;

    Geom() = delete;
    Geom(const std::string inputfile,const R magni,const Vec3 slide,const Vec3 r,R theta);

    Geom(const std::string inputfile,const R magni,const Vec3 slide,const Vec3 r,R theta,const Geom_shading_type t);

    Geom(const std::string inputfile,const std::vector<Scene_data::Transform> transforms,const Material *material,const Geom_shading_type t);

    void load_and_precompute(const std::string inputfile,const std::vector<Scene_data::Transform> transforms,const Material *material);

    void load_and_precompute(const std::string inputfile,const R magni,const Vec3 slide,Vec3 r,R theta);

    inline Intersection_point* get_intersection(const Ray &ray) const {
       int index = bvh.traverse(ray,vertices,prims);
        if(index > -1){
            return polygon_intersection(ray,prims[index]);
        }
        return nullptr;
    }

	inline Intersection_point* polygon_intersection(const Ray &ray,const Prim &prim) const {
        const Vec3 vertex[3] = {vertices[prim.vertices_index[0]] , vertices[prim.vertices_index[1]] , vertices[prim.vertices_index[2]] };
        const Vec3 normal[3] = {normals[prim.normals_index[0]] , normals[prim.normals_index[1]] , normals[prim.normals_index[2]] };
        const Vec3 uv[3] = {uvs[prim.uvs_index[0]] , uvs[prim.uvs_index[1]] , uvs[prim.uvs_index[2]] };

		const Vec3 &r = ray.direction;
		const Vec3 T = ray.start - vertex[0];
		const Vec3 E1 = vertex[1] - vertex[0];
		const Vec3 E2 = vertex[2] - vertex[0];
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
			const Vec3 intersection = ray.start + t * r;
			const R a = cross(vertex[1] - intersection,vertex[2] - intersection).abs();
			const R b = cross(E2,intersection - vertex[0]).abs();
			const R c = cross(E1,intersection - vertex[0]).abs();

			const Vec3 ret_normal = (a * normal[0] + b * normal[1] + c * normal[2]).normalized();
			if(prim.texture_id >= 0){
				return new Intersection_point(t,intersection,ret_normal,Material(textures[prim.texture_id].get_kd(u * (uv[1] - uv[0]) + v * (uv[2] - uv[0]) + uv[0])));
			}
			return new Intersection_point(t,intersection,ret_normal,materials[prim.mtl_id]);
		}
		return nullptr;
	}

    inline R get_S() const {
        return S;
    }

	inline void sample_one_point (Vec3 &point,Vec3 &normal,R &pdf,FColor &emission) const {//http://zwxadz.hateblo.jp/entry/2016/06/08/020550  http://www.sciencedirect.com/science/article/pii/S0895717708002665
        int prim_id = (int)(Random::rando() * prims_num);
        if(prim_id < 0)
            prim_id = 0;
        if(prim_id >= prims_num)
            prim_id = prims_num - 1;
        const Prim &prim = prims[prim_id];
        const R u1 = Random::rando();
        const R u2 = Random::rando();

        const R M = (u1 > u2) ? u1 : u2;
        const R m = (u1 < u2) ? u1 : u2;

        point = vertices[prim.vertices_index[0]] * m
                + vertices[prim.vertices_index[1]] * (1.0 - M)
                + vertices[prim.vertices_index[2]] * (M - m);

        normal = (cross(vertices[prim.vertices_index[1]] - vertices[prim.vertices_index[0]],vertices[prim.vertices_index[2]] - vertices[prim.vertices_index[1]])).normalized();

        pdf = 1.0 / (prims_num * prim.S);
        emission = materials[0].Le;
	}

    ~Geom();

    Geom(const Geom &obj);
};