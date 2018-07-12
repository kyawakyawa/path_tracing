#include <cmath>
#include <vector>
#include "../Vec3/Vec3.hpp"
#include "../Color/FColor.hpp"
#include "../Scene/Scene.hpp"
#include "../Shape/Sphere.hpp"
#include "../Shape/Geom.hpp"
#include "../Radiance/Path_tracing.hpp"
#include "../Radiance/Path_tracing_direct.hpp"
#include "../Radiance/Path_tracing_direct_nonrecursive.hpp"
#include "../Radiance/Normal_render.hpp"
#include "../Input_scene/Scene_data.hpp"
#include "../Camera/Ideal_pin_hole.hpp"
#include "../Camera/Thin_lens.hpp"

#include "../GL_out/glout.h"

#include "../ext_libs/toml.h"//https://github.com/mayah/tinytoml

int main(int argc, char **argv){

	if(argc < 2){
		printf("tomlファイルを指定してー\n");
		return 0;
	}

	std::ifstream ifs(argv[1]);
	toml::ParseResult pr = toml::parse(ifs);

	if (!pr.valid()) {
		std::cerr << pr.errorReason << std::endl;
		return 0;
	}

	const toml::Value& v = pr.value;


	Scene_data::Root root(v);
	
	Vec3 pos,dir,up;
	R dis = 15.0 / tan(root.camera.fov / 360.0 * M_PI);
	for(const auto & trans : root.camera.transform) {
		if(trans.type == Scene_data::LOOK_AT) {
			pos = trans.vec3[0];
			dir = trans.vec3[1];
			up = trans.vec3[2];
		}
	}
	dir -= pos;

	if(root.sky.type == Scene_data::UNIFORM)
		root.sky.path = "";
	else
		root.sky.color = FColor(0.75,0.75,0.75);

	Radiance *radiance = nullptr;
	if(root.renderer.integrator == "pt") {
		radiance = new Path_tracing(root.renderer.depth,root.renderer.depth_limit,root.sky.path,root.sky.color);
	}else if(root.renderer.integrator == "pt-direct") {
		radiance = new Path_tracing_direct(root.renderer.depth,root.renderer.depth_limit,root.sky.path,root.sky.color);
	}else if(root.renderer.integrator == "pt-direct-nonrecursive") {
		radiance = new Path_tracing_direct_nonrecursive(root.renderer.depth,root.renderer.depth_limit,root.sky.path,root.sky.color);
	}else {
		radiance = new Normal_render();
	}

	Camera *camera = nullptr;

	if(root.camera.type == Scene_data::IDEAL_PINHOLE) {
		camera = new Ideal_pin_hole(root.film.resolution[0],root.film.resolution[1],pos,dir,up,root.camera.fov);
	}else if(root.camera.type == Scene_data::THIN_LENS) {
		camera = new Thin_lens(root.film.resolution[0],root.film.resolution[1],pos,dir,up,root.camera.fov,root.camera.focus_distance,root.camera.f_number);
	}else {
	}

	//Scene scene(Camera(root.film.resolution[0],root.film.resolution[1],pos,dir,up,dis),radiance);
	//Scene scene(new Ideal_pin_hole(root.film.resolution[0],root.film.resolution[1],pos,dir,up,root.camera.fov),radiance);
	//Scene scene(new Thin_lens(root.film.resolution[0],root.film.resolution[1],pos,dir,up,39.3077,530,5.4),radiance);
	Scene scene(camera,radiance);

	radiance = nullptr;
	camera = nullptr;

	bool used_obj[root.object.size()];
	std::fill(used_obj,used_obj + root.object.size(),false);

	for(const auto &lig : root.light) {
		if(lig.type == Scene_data::AREA) {
			Scene_data::Object obj;
			int p = 0;
			for(const auto o : root.object) {
				if(lig.object == o.name) {
					obj = o;
					used_obj[p] = true;
					break;
				}
				p++;
			}

			Scene_data::Mesh me;
			for(const auto &m : root.mesh){
				if(obj.mesh == m.name) {
					me = m;
					break;
				}
			}
			Scene_data::Material ma;
			for(const auto &m : root.material) {
				if(obj.material == m.name) {
					ma = m;
					break;
				}
			}

			if(me.type == Scene_data::SPHERE) {
				Vec3 cen(0.0,0.0,0.0);
				for(const auto &transform : obj.transform) {
					if(transform.type == Scene_data::TRANSLATE) {
						cen += transform.vec3[0];
					}
					if(transform.type == Scene_data::SCALE) {
                    	//cen.x *= transform.vec3[0].x;
                    	//cen.y *= transform.vec3[0].y;
                    	//cen.z *= transform.vec3[0].z;
                    	me.radius *= transform.vec3[0].x;
					}
					if(transform.type == Scene_data::AXIS_ANGLE) {
						cen = rotate(cen,transform.vec3[0],transform.f);
					}
				}

				if(ma.type == Scene_data::LAMBERT) {
					scene.add_as_light(new Sphere(cen,me.radius,Material(ma.albedo,lig.emission)));
				}
			}
			if(me.type == Scene_data::OBJ) {
				if(ma.type == Scene_data::LAMBERT) {
					Geom_shading_type st = (me.normal == "smooth") ? Gm_SMOOTH : Gm_STRICT;
					Material material(ma.albedo,lig.emission);
					scene.add_as_light(new Geom(me.path,obj.transform,&material,st));
				}
			}
		}
	}

	int p = 0;
	for(const auto &obj : root.object) {
		if(used_obj[p]){
			continue;
		}

		bool is_mesh_exist= false;
		Scene_data::Mesh me;
		for(const auto &m : root.mesh){
			if(obj.mesh == m.name) {
				me = m;
				is_mesh_exist = true;
				break;
			}
		}
		if(!is_mesh_exist){
			continue;
		}

		bool is_material_exist = false;
		Scene_data::Material ma;
		for(const auto &m : root.material) {
			if(obj.material == m.name) {
				ma = m;
				is_material_exist = true;
				break;
			}
		}

		if(me.type == Scene_data::SPHERE) {
			Vec3 cen(0.0,0.0,0.0);
			for(const auto &transform : obj.transform) {
                if(transform.type == Scene_data::TRANSLATE) {
                    cen += transform.vec3[0];
                }
                if(transform.type == Scene_data::SCALE) {
                    //cen.x *= transform.vec3[0].x;
                    //cen.y *= transform.vec3[0].y;
                    //cen.z *= transform.vec3[0].z;
                    me.radius *= transform.vec3[0].x;
                }
                if(transform.type == Scene_data::AXIS_ANGLE) {
                    cen = rotate(cen,transform.vec3[0],transform.f);
				}
			}

			if(!is_material_exist) {
				scene.add(new Sphere(cen,me.radius,Material(FColor(1.0,0.0,0.0))));
			}else if(ma.type == Scene_data::LAMBERT) {
				scene.add(new Sphere(cen,me.radius,Material(ma.albedo)));
			}else if(ma.type == Scene_data::PHONG) {
				scene.add(new Sphere(cen,me.radius,Material(ma.reflectance,ma.alpha,MT_NORMALIZED_PHONG)));
			}
		}
		if(me.type == Scene_data::OBJ) {
			if(!is_material_exist) {
				Geom_shading_type st = (me.normal == "smooth") ? Gm_SMOOTH : Gm_STRICT;
				scene.add(new Geom(me.path,obj.transform,nullptr,st));
			}else if(ma.type == Scene_data::LAMBERT) {
				Geom_shading_type st = (me.normal == "smooth") ? Gm_SMOOTH : Gm_STRICT;
				Material material(ma.albedo);
				scene.add(new Geom(me.path,obj.transform,&material,st));
			}else if(ma.type == Scene_data::PHONG) {
				Geom_shading_type st = (me.normal == "smooth") ? Gm_SMOOTH : Gm_STRICT;
				Material material(ma.reflectance,ma.alpha,MT_NORMALIZED_PHONG);
				scene.add(new Geom(me.path,obj.transform,&material,st));
			}
		}
		p++;
	}


	//scene.draw(root.renderer.samples);
	//std::cerr << scene.shapes.size() << std::endl;
	//scene.draw(std::max(1,root.renderer.samples / 16),4);
	//scene.debug(std::max(1,root.renderer.samples / 16),4);
	drawgl(argc,argv,scene);
	std::cerr << maxy << std::endl;

	return 0;
}
