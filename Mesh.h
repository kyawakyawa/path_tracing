#pragma once

#include <vector>

#define TINYOBJLOADER_IMPLEMENTATION 
//#define TINYOBJLOADER_USE_DOUBLE
#include "tiny_obj_loader.h"

#include "Vec3.h"
#include "Shape.h"
#include "BVH.h"
#include "Polygon.h"
#include "Polygon_info.h"

enum Mesh_shading_type{
	Ms_DEFAULT,//通常
	Ms_SMOOTH//頂点法線を平均化して滑らかにする
};

struct Mesh : public Shape{

	const Mesh_shading_type type;
	std::vector< Polygon > polygons;
	BVH bvh;

	Mesh() = delete;
	inline Mesh(const std::string inputfile,const R magni,const Vec3 slide,Vec3 r,R theta,Mesh_shading_type t):type(t){
		load_and_precompute(inputfile,magni,slide,r,theta);
	}
	inline Mesh(const std::string inputfile,const R magni,const Vec3 slide,Vec3 r,R theta):type(Ms_DEFAULT){
		load_and_precompute(inputfile,magni,slide,r,theta);
	}
	inline void load_and_precompute(const std::string inputfile,const R magni,const Vec3 slide,Vec3 r,R theta){
		r = r.normalized();

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

		for(const auto &sh : shs){
			int p = 0;
			//for(const auto &nfv : sh.mesh.num_face_vertices){
			for(int i = 0;i < sh.mesh.num_face_vertices.size();i++){
				auto &nfv = sh.mesh.num_face_vertices[i];
				Polygon poly;
				for(int j = 0;j < nfv;j++){
					poly.vertex.push_back(get_vertice(at,sh.mesh.indices[p].vertex_index,magni,slide,r,theta));
					poly.uv.push_back(get_texcoord(at,sh.mesh.indices[p].texcoord_index));
					if(type == Ms_DEFAULT && sh.mesh.indices[p].normal_index >= 0){
						poly.vertex_normal.push_back(get_normal(at,sh.mesh.indices[p].normal_index));
					}
					p++;
				}
				if(sh.mesh.material_ids[i] >= 0) poly.mtl_id = sh.mesh.material_ids[i];
				//std::cout << sh.mesh.indices[p - 1].texcoord_index << std::endl;
				//std::cout << sh.mesh.material_ids[i] << std::endl;
				polygons.push_back(poly);
			}
		}
		for(auto &polygon : polygons)
			polygon.face_normal = (cross(polygon.vertex[1] - polygon.vertex[0],polygon.vertex[2] - polygon.vertex[1])).normalized();

		if(type == Ms_SMOOTH){
			std::vector< Vec3 > normals;
			for(int i = 0;i < at.vertices.size() / 3 ;i++){
				normals.push_back(Vec3(0,0,0));
			}
			int q = 0;
			for(int i = 0;i < shs.size();i++){
				int n = shs[i].mesh.num_face_vertices.size();
				int p = 0;
				for(int j = 0;j < n;j++){
					auto &nfv = shs[i].mesh.num_face_vertices[j];
					Polygon &poly = polygons[q++];
					for(int k = 0;k < nfv;k++){
						normals[shs[i].mesh.indices[p].vertex_index] += poly.face_normal;
						p++;
					}
				}
			}
			for(auto &normal : normals){
				normal = normal.normalized();
			}
			q = 0;
			for(int i = 0;i < shs.size();i++){
				int n = shs[i].mesh.num_face_vertices.size();
				int p = 0;
				for(int j = 0;j < n;j++){
					auto &nfv = shs[i].mesh.num_face_vertices[j];
					Polygon &poly = polygons[q++];
					for(int k = 0;k < nfv;k++){
						poly.vertex_normal.push_back(normals[shs[i].mesh.indices[p].vertex_index]);
						p++;
					}
				}
			}
		}

		if(mtls.begin() == mtls.end()){
			materials.push_back(Material(FColor(240.0 / 255,210.0 / 255,37.0 / 255),MT_PERFECT_REF));
		}else{
			for(auto &mtl : mtls){
				materials.push_back(Material(FColor(mtl.diffuse[0],mtl.diffuse[1],mtl.diffuse[1])));
				if(!mtl.diffuse_texname.empty()){
					textures.push_back(Texture(mtl.diffuse_texname));
				}
			}
		}
		if(textures.size() > 0){
			for(auto &polygon : polygons){
				for(int i = 0;i < textures.size();i++){
					if(textures[i].name == mtls[polygon.mtl_id].diffuse_texname){
						polygon.texture_id = i;
						break;
					}
				}
			}
		}

		bvh.construction(polygons);
		/*for(const auto &polygon : polygons){
		  	for(const auto &v : polygon){
		  		std::cout << v << " ";
		  	}
		  	std::cout << std::endl;
		  }*/
	};

	static inline Vec3 get_vertice(const tinyobj::attrib_t &at,const int index,const R magni,const Vec3 slide,const Vec3 r,const R theta){
		//return Vec3(at.vertices[index * 3],at.vertices[index * 3 + 1],at.vertices[index * 3 + 2]) * 30 + Vec3(50,10,70);//bunny
		//return Vec3(at.vertices[index * 3],at.vertices[index * 3 + 1],at.vertices[index * 3 + 2]) * 70 + Vec3(70,20,50);//dragon
		//return Vec3(at.vertices[index * 3],at.vertices[index * 3 + 1],at.vertices[index * 3 + 2]) * (R)(0.2) + Vec3(50,10,50);//car
		//return Vec3(at.vertices[index * 3],at.vertices[index * 3 + 1],at.vertices[index * 3 + 2]) + Vec3(50,60,70);//sibenik
		//return Vec3(at.vertices[index * 3],at.vertices[index * 3 + 1],at.vertices[index * 3 + 2]) * 70 + Vec3(50,30,60);//buddha
		//return Vec3(at.vertices[index * 3],at.vertices[index * 3 + 1],at.vertices[index * 3 + 2]) * 100;

		return rotate(Vec3(at.vertices[index * 3],at.vertices[index * 3 + 1],at.vertices[index * 3 + 2]),r,theta) * magni + slide;
		//return Vec3(at.vertices[index * 3],at.vertices[index * 3 + 1],at.vertices[index * 3 + 2]) * magni + slide;
	}

	static inline Vec3 get_texcoord(const tinyobj::attrib_t &at,const int index){
		if(index < 0){
			return Vec3(0.0,0.0,0.0);
		}
		return Vec3(at.texcoords[index * 2],at.texcoords[index * 2 + 1],0.0);
	}

	static inline Vec3 get_normal(const tinyobj::attrib_t &at,const int index){
		return Vec3(at.normals[index * 3],at.normals[index * 3 + 1],at.normals[index * 3 + 2]);
	}

	static inline void compute_vertex_normal(tinyobj::attrib_t &at,std::vector<tinyobj::shape_t> &shs){

	}

	inline Intersection_point* get_intersection(const Ray &ray) const {
		/*Intersection_point *ret = nullptr;
		  R mint = 1000000000.0;

		  for(const auto &polygon : polygons){
		  	Intersection_point *p = polygon_intersection(ray,polygon);
		  	if(p != nullptr && p->distance < mint){
		  		mint = p->distance;
		  		delete ret;
		  		ret = p;
		  	}
		  }

		  return ret;*/

		const Polygon_info *polygon_info = bvh.traverse(ray,polygons);
		if(polygon_info == nullptr)
			return nullptr;
		Intersection_point *ret = polygon_intersection(ray,polygons[polygon_info->polygon_index]);
		delete polygon_info;
		return ret;
	}

	inline Intersection_point* polygon_intersection(const Ray &ray,const Polygon &polygon) const {
		//const Vec3 &normal = polygon.face_normal;//(cross(polygon.vertex[1] - polygon.vertex[0],polygon.vertex[2] - polygon.vertex[1])).normalized();

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
				const Vec3 intersection = ray.start + t * r;
				Vec3 normal;
				if(polygon.vertex.size() == polygon.vertex_normal.size()){
					R a = cross(polygon.vertex[i + 1] - intersection,polygon.vertex[i + 2] - intersection).abs();
					R b = cross(E2,intersection - polygon.vertex[0]).abs();
					R c = cross(E1,intersection - polygon.vertex[0]).abs();
					normal = (a * polygon.vertex_normal[0] + b * polygon.vertex_normal[i + 1] + c * polygon.vertex_normal[i + 2]).normalized();
				}else{
					normal = polygon.face_normal;
				}
				if(polygon.texture_id >= 0){
					return new Intersection_point(t,intersection,normal,Material(textures[polygon.texture_id].get_kd(u * (polygon.uv[i + 1] - polygon.uv[0]) + v * (polygon.uv[i + 2] - polygon.uv[0]) + polygon.uv[0])));
				}
				return new Intersection_point(t,intersection,normal,materials[polygon.mtl_id]);
				//return new Intersection_point(t,intersection,normal,Material(FColor(0.7,0.7,0.7)));
			}
		}
		return nullptr;
	}

	//inline Material get_material(const Vec3 &position) const {
		//return materials[0];
	//}
};
