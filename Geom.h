#pragma once

#include <chrono>
#include <math.h>
#include<iomanip>

#define TINYOBJLOADER_IMPLEMENTATION 
//#define TINYOBJLOADER_USE_DOUBLE
#include "tiny_obj_loader.h"

#include "Vec3.h"
#include "Shape.h"
#include "Prim.h"
#include "BVH.h"

#include "Scene_data.h"

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
    inline Geom(const std::string inputfile,const R magni,const Vec3 slide,const Vec3 r,R theta) : type(Gm_STRICT){
        load_and_precompute(inputfile,magni,slide,r,theta);
    }

    inline Geom(const std::string inputfile,const R magni,const Vec3 slide,const Vec3 r,R theta,const Geom_shading_type t) : type(t){
        load_and_precompute(inputfile,magni,slide,r,theta);
    }

    inline Geom(const std::string inputfile,const std::vector<Scene_data::Transform> transforms,const Material *material,const Geom_shading_type t) : type(t){
        load_and_precompute(inputfile,transforms,material);
    }

    inline void load_and_precompute(const std::string inputfile,const std::vector<Scene_data::Transform> transforms,const Material *material) {

		tinyobj::attrib_t at;
		std::vector<tinyobj::shape_t> shs;
		std::vector<tinyobj::material_t> mtls;

		std::string err;
		bool ret = tinyobj::LoadObj(&at, &shs, &mtls, &err, inputfile.c_str());

		if (!err.empty()) { // `err` may contain warning message.
			std::cerr << err << std::endl;
		}

		if (!ret) {
			exit(1);
		}

        prims_num = 0;
        for(const auto &sh : shs){
            for(const auto num : sh.mesh.num_face_vertices){
                prims_num += (num - 2);
            }
        }

        prims = new Prim[prims_num];

        if(type == Gm_SMOOTH){
            vertices = new Vec3[vertices_num = at.vertices.size() / 3];
            normals = new Vec3[normals_num = at.vertices.size() / 3];
            for(int i = 0;i < at.vertices.size() / 3;i++){
                vertices[i] = Vec3(at.vertices[i * 3],at.vertices[i * 3 + 1],at.vertices[i * 3 + 2]);
                for(const auto transform : transforms){
                    if(transform.type == Scene_data::TRANSLATE) {
                        vertices[i] += transform.vec3[0];
                    }
                    if(transform.type == Scene_data::SCALE) {
                        vertices[i].x *= transform.vec3[0].x;
                        vertices[i].y *= transform.vec3[0].y;
                        vertices[i].z *= transform.vec3[0].z;
                    }
                    if(transform.type == Scene_data::AXIS_ANGLE) {
                        vertices[i] = rotate(vertices[i],transform.vec3[0],transform.f / 18.0 * M_PI);
                    }
                }
                normals[i] = Vec3(0,0,0);
            }
        }else if(type == Gm_STRICT){
            vertices = new Vec3[vertices_num = at.vertices.size() / 3];
            for(int i = 0;i < at.vertices.size() / 3;i++){
                vertices[i] = Vec3(at.vertices[i * 3],at.vertices[i * 3 + 1],at.vertices[i * 3 + 2]);
                for(const auto transform : transforms){
                    if(transform.type == Scene_data::TRANSLATE) {
                        vertices[i] += transform.vec3[0];
                    }
                    if(transform.type == Scene_data::SCALE) {
                        vertices[i].x *= transform.vec3[0].x;
                        vertices[i].y *= transform.vec3[0].y;
                        vertices[i].z *= transform.vec3[0].z;
                    }
                    if(transform.type == Scene_data::AXIS_ANGLE) {
                        vertices[i] = rotate(vertices[i],transform.vec3[0],transform.f / 180.0 * M_PI);
                    }
                }
            }
            normals = new Vec3[normals_num = prims_num];
        }

        uvs = new Vec3[uvs_num = at.texcoords.size() / 2 + 1]; 
        uvs[0] = Vec3(0.0,0.0,0.0);
        for(int i = 0;i < at.texcoords.size() / 2;i++){
            uvs[i] = Vec3(at.texcoords[i * 2],at.texcoords[i * 2 + 1],0.0); 
        }
        
        int p = 0;bool is_warning = true;
        for(const auto &sh : shs){
            int q = 0;
            for(int i = 0;i < sh.mesh.num_face_vertices.size();i++) {
                int nfv = sh.mesh.num_face_vertices[i];
                for(int j = 0;j < nfv - 2;j++) {
                    Prim &prim = prims[p];
                    prim.vertices_index[0] = sh.mesh.indices[q].vertex_index;
                    prim.vertices_index[1] = sh.mesh.indices[q + j + 1].vertex_index;
                    prim.vertices_index[2] = sh.mesh.indices[q + j + 2].vertex_index;

                    prim.uvs_index[0] = (sh.mesh.indices[q].texcoord_index >= 0) ? sh.mesh.indices[q].texcoord_index : 0;//無い場合は-1
                    prim.uvs_index[1] = (sh.mesh.indices[q + j + 1].texcoord_index >= 0) ? sh.mesh.indices[q + j + 1].texcoord_index : 0;//無い場合は-1
                    prim.uvs_index[2] = (sh.mesh.indices[q + j + 2].texcoord_index >= 0) ? sh.mesh.indices[q + j + 2].texcoord_index : 0;//無い場合は-1

                    if(type == Gm_SMOOTH){
                        Vec3 normal =
                            (cross(vertices[prim.vertices_index[1]] - vertices[prim.vertices_index[0]],vertices[prim.vertices_index[2]] - vertices[prim.vertices_index[1]])).normalized();
                        for(int k = 0;k < 3;k++){
                            prim.normals_index[k] = prim.vertices_index[k];
                            if(j == 1 && (k == 0 || k == 1)){
                                continue;//だぶらないように
                            }
                            if(std::isnan(normal.abs())){
                                if(is_warning){
                                    std::cerr << "Warning! ポリゴンが潰れていて法線が正しく計算できませんでした。" << std::endl;
                                    is_warning = false;
                                }
                                continue;
                            }
                            normals[prim.normals_index[k]] += normal;
                        }
                    }else if(type == Gm_STRICT){
                        normals[p] = (cross(vertices[prim.vertices_index[1]] - vertices[prim.vertices_index[0]],vertices[prim.vertices_index[2]] - vertices[prim.vertices_index[1]])).normalized();
                        for(int k = 0;k < 3;k++){
                            prim.normals_index[k] = p;
                        }
                    }
                    if(material == nullptr && sh.mesh.material_ids[i] >= 0) prim.mtl_id = sh.mesh.material_ids[i];
                    p++;
                }
                q += nfv;
            }
        }

        if(type == Gm_SMOOTH) {
            for(int i = 0;i < normals_num;i++) {
                normals[i] = normals[i].normalized();
            }
        }

        if(material != nullptr){
            materials.push_back(*material);
        }else if(mtls.begin() == mtls.end()){
			//materials.push_back(Material(FColor(240.0 / 255,210.0 / 255,37.0 / 255),MT_PERFECT_REF));
			materials.push_back(FColor(0.75,0.75,0.75));
		}else{
			for(auto &mtl : mtls){
				materials.push_back(Material(FColor(mtl.diffuse[0],mtl.diffuse[1],mtl.diffuse[2])));
				if(!mtl.diffuse_texname.empty()){
					textures.push_back(Texture(mtl.diffuse_texname));
				}
			}
		}
		if(textures.size() > 0){
			for(int i = 0;i < prims_num;i++){
				for(int j = 0;j < textures.size();i++){
					if(textures[j].name == mtls[prims[i].mtl_id].diffuse_texname){
						prims[i].texture_id = j;
						break;
					}
				}
			}
		}

        S = 0;
        for(int i = 0;i < prims_num;i++) {
            prims[i].S = cross(vertices[prims[i].vertices_index[2]] - vertices[prims[i].vertices_index[0]],vertices[prims[i].vertices_index[1]] - vertices[prims[i].vertices_index[0]]).abs() * 0.5;
            S += prims[i].S;
        }

	    std::chrono::system_clock::time_point start,end;
        start = std::chrono::system_clock::now();
        bvh.construction(vertices,prims,prims_num);
	    end = std::chrono::system_clock::now();

        auto elapsed = std::chrono::duration_cast< std::chrono::milliseconds >(end - start).count();
        std::cerr <<"BVH construction  " << elapsed <<"ms"<< std::endl;
    }

    inline void load_and_precompute(const std::string inputfile,const R magni,const Vec3 slide,Vec3 r,R theta) {

		tinyobj::attrib_t at;
		std::vector<tinyobj::shape_t> shs;
		std::vector<tinyobj::material_t> mtls;

		std::string err;
		bool ret = tinyobj::LoadObj(&at, &shs, &mtls, &err, inputfile.c_str());

		if (!err.empty()) { // `err` may contain warning message.
			std::cerr << err << std::endl;
		}

		if (!ret) {
			exit(1);
		}

        prims_num = 0;
        for(const auto &sh : shs){
            for(const auto num : sh.mesh.num_face_vertices){
                prims_num += (num - 2);
            }
        }

        prims = new Prim[prims_num];

        if(type == Gm_SMOOTH){
            vertices = new Vec3[vertices_num = at.vertices.size() / 3];
            normals = new Vec3[normals_num = at.vertices.size() / 3];
            for(int i = 0;i < at.vertices.size() / 3;i++){
                vertices[i] = rotate(Vec3(at.vertices[i * 3],at.vertices[i * 3 + 1],at.vertices[i * 3 + 2]),r,theta) * magni + slide;
                normals[i] = Vec3(0,0,0);
            }
        }else if(type == Gm_STRICT){
            vertices = new Vec3[vertices_num = at.vertices.size() / 3];
            for(int i = 0;i < at.vertices.size() / 3;i++){
                vertices[i] = rotate(Vec3(at.vertices[i * 3],at.vertices[i * 3 + 1],at.vertices[i * 3 + 2]),r,theta) * magni + slide;
            }
            normals = new Vec3[normals_num = prims_num];
        }

        uvs = new Vec3[uvs_num = at.texcoords.size() / 2 + 1]; 
        uvs[0] = Vec3(0.0,0.0,0.0);
        for(int i = 0;i < at.texcoords.size() / 2;i++){
            uvs[i] = Vec3(at.texcoords[i * 2],at.texcoords[i * 2 + 1],0.0); 
        }
        
        int p = 0;bool is_warning = true;
        for(const auto &sh : shs){
            int q = 0;
            for(int i = 0;i < sh.mesh.num_face_vertices.size();i++) {
                int nfv = sh.mesh.num_face_vertices[i];
                for(int j = 0;j < nfv - 2;j++) {
                    Prim &prim = prims[p];
                    prim.vertices_index[0] = sh.mesh.indices[q].vertex_index;
                    prim.vertices_index[1] = sh.mesh.indices[q + j + 1].vertex_index;
                    prim.vertices_index[2] = sh.mesh.indices[q + j + 2].vertex_index;

                    prim.uvs_index[0] = (sh.mesh.indices[q].texcoord_index >= 0) ? sh.mesh.indices[q].texcoord_index : 0;//無い場合は-1
                    prim.uvs_index[1] = (sh.mesh.indices[q + j + 1].texcoord_index >= 0) ? sh.mesh.indices[q + j + 1].texcoord_index : 0;//無い場合は-1
                    prim.uvs_index[2] = (sh.mesh.indices[q + j + 2].texcoord_index >= 0) ? sh.mesh.indices[q + j + 2].texcoord_index : 0;//無い場合は-1

                    if(type == Gm_SMOOTH){
                        Vec3 normal =
                            (cross(vertices[prim.vertices_index[1]] - vertices[prim.vertices_index[0]],vertices[prim.vertices_index[2]] - vertices[prim.vertices_index[1]])).normalized();
                        for(int k = 0;k < 3;k++){
                            prim.normals_index[k] = prim.vertices_index[k];
                            if(j == 1 && (k == 0 || k == 1)){
                                continue;//だぶらないように
                            }
                            if(std::isnan(normal.abs())){
                                if(is_warning){
                                    std::cerr << "Warning! ポリゴンが潰れていて法線が正しく計算できませんでした。" << std::endl;
                                    is_warning = false;
                                }
                                continue;
                            }
                            normals[prim.normals_index[k]] += normal;
                        }
                    }else if(type == Gm_STRICT){
                        normals[p] = (cross(vertices[prim.vertices_index[1]] - vertices[prim.vertices_index[0]],vertices[prim.vertices_index[2]] - vertices[prim.vertices_index[1]])).normalized();
                        for(int k = 0;k < 3;k++){
                            prim.normals_index[k] = p;
                        }
                    }
                    if(sh.mesh.material_ids[i] >= 0) prim.mtl_id = sh.mesh.material_ids[i];
                    p++;
                }
                q += nfv;
            }
        }

        if(type == Gm_SMOOTH) {
            for(int i = 0;i < normals_num;i++) {
                normals[i] = normals[i].normalized();
            }
        }

		if(mtls.begin() == mtls.end()){
			//materials.push_back(Material(FColor(240.0 / 255,210.0 / 255,37.0 / 255),MT_PERFECT_REF));
			materials.push_back(Material(FColor(0.75,0.75,0.75)));
		}else{
			for(auto &mtl : mtls){
				materials.push_back(Material(FColor(mtl.diffuse[0],mtl.diffuse[1],mtl.diffuse[1])));
				if(!mtl.diffuse_texname.empty()){
					textures.push_back(Texture(mtl.diffuse_texname));
				}
			}
		}
		if(textures.size() > 0){
			for(int i = 0;i < prims_num;i++){
				for(int j = 0;j < textures.size();i++){
					if(textures[j].name == mtls[prims[i].mtl_id].diffuse_texname){
						prims[i].texture_id = j;
						break;
					}
				}
			}
		}

        S = 0;
        for(int i = 0;i < prims_num;i++) {
            prims[i].S = cross(vertices[prims[i].vertices_index[2]] - vertices[prims[i].vertices_index[0]],vertices[prims[i].vertices_index[1]] - vertices[prims[i].vertices_index[0]]).abs() * 0.5;
            S += prims[i].S;
        }

	    std::chrono::system_clock::time_point start,end;
        start = std::chrono::system_clock::now();
        bvh.construction(vertices,prims,prims_num);
	    end = std::chrono::system_clock::now();

        auto elapsed = std::chrono::duration_cast< std::chrono::milliseconds >(end - start).count();
        std::cerr <<"BVH construction  " << elapsed <<"ms"<< std::endl;
    }

    inline Intersection_point* get_intersection(const Ray &ray) const {
		/*Intersection_point *ret = nullptr;
	    R mint = 1000000000.0;

		for(int i = 0;i < prims_num;i++){
		    Intersection_point *p = polygon_intersection(ray,prims[i]);
		    if(p != nullptr && p->distance < mint){
		    	mint = p->distance;
		    	delete ret;
		    	ret = p;
			}
		}
        return ret;*/
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
			//const Vec3 ret_normal = (cross(vertex[1] - vertex[0],vertex[2] - vertex[1])).normalized();
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
        int prim_id = (int)(rando() * prims_num);
        if(prim_id < 0)
            prim_id = 0;
        if(prim_id >= prims_num)
            prim_id = prims_num - 1;
        const Prim &prim = prims[prim_id];
        const R u1 = rando();
        const R u2 = rando();

        const R M = (u1 > u2) ? u1 : u2;
        const R m = (u1 < u2) ? u1 : u2;

        point = vertices[prim.vertices_index[0]] * m
                + vertices[prim.vertices_index[1]] * (1.0 - M)
                + vertices[prim.vertices_index[2]] * (M - m);

        normal = (cross(vertices[prim.vertices_index[1]] - vertices[prim.vertices_index[0]],vertices[prim.vertices_index[2]] - vertices[prim.vertices_index[1]])).normalized();

		/*for(int i = 0;i < 3;i++){
			if(((cross(vertices[prim.vertices_index[(i + 1) % 3]] - vertices[prim.vertices_index[i]],point - vertices[prim.vertices_index[(i + 1) % 3]])).normalized() * normal < EPS)){
                std::cerr << "三角形の外" << std::endl;
                std::cerr << point << std::endl;
                std::cerr << vertices[prim.vertices_index[0]] << " " << vertices[prim.vertices_index[1]] << " " << vertices[prim.vertices_index[2]] << std::endl;
                break;
			}
        }*/

        pdf = 1.0 / (prims_num * prim.S);
        emission = materials[0].Le;
	}

    ~Geom() {
        delete[] vertices;
        delete[] normals;
        delete[] uvs;
        delete[] prims;
    }

    Geom(const Geom &obj){

        type = obj.type;

        delete[] vertices;
        vertices_num = obj.vertices_num;
        vertices = new Vec3[vertices_num];
        for(int i = 0;i < vertices_num;i++){
            vertices[i] = obj.vertices[i];
        }
    
        delete[] normals;
        normals_num = obj.normals_num;
        normals = new Vec3[normals_num];
        for(int i = 0;i < normals_num;i++){
            normals[i] = obj.normals[i];
        }

        delete[] uvs;
        uvs_num = obj.uvs_num;
        uvs = new Vec3[uvs_num];
        for(int i = 0;i < uvs_num;i++){
            uvs[i] = obj.uvs[i];
        }

        delete[] prims;
        prims_num = obj.prims_num;
        prims = new Prim[prims_num];
        for(int i = 0;i < prims_num;i++){
            prims[i] = obj.prims[i];
        }

        bvh = obj.bvh;
    }
};