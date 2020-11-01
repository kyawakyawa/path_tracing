#include <chrono>
#include<iomanip>

#define TINYOBJLOADER_IMPLEMENTATION 
//#define TINYOBJLOADER_USE_DOUBLE
#include "../../inc/ext_libs/tiny_obj_loader.h"

#include "../../inc/Shape/Geom.hpp"

Geom::Geom(const std::string inputfile,const R magni,const Vec3 slide,const Vec3 r,R theta) : type(Gm_STRICT){
	Geom::load_and_precompute(inputfile,magni,slide,r,theta);
}

Geom::Geom(const std::string inputfile,const R magni,const Vec3 slide,const Vec3 r,R theta,const Geom_shading_type t) : type(t){
	load_and_precompute(inputfile,magni,slide,r,theta);
}

Geom::Geom(const std::string inputfile,const std::vector<Scene_data::Transform> transforms,const Material *material,const Geom_shading_type t) : type(t){
	load_and_precompute(inputfile,transforms,material);
}

void Geom::load_and_precompute(const std::string inputfile,const std::vector<Scene_data::Transform> transforms,const Material *material) {

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

void Geom::load_and_precompute(const std::string inputfile,const R magni,const Vec3 slide,Vec3 r,R theta) {

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


Geom::~Geom() {
	delete[] vertices;
	delete[] normals;
	delete[] uvs;
	delete[] prims;
}

Geom::Geom(const Geom &obj){

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
