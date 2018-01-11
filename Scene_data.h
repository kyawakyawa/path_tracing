#pragma once
#include <cmath>
#include <vector>
#include "Vec3.h"
#include "FColor.h"

#include "toml.h"//https://github.com/mayah/tinytoml

namespace Scene_data{
	enum Transform_type {
		TRANSLATE,
		SCALE,
		AXIS_ANGLE,
		LOOK_AT
	} type;
	struct Transform{
		Transform_type  type = TRANSLATE;
		Vec3 vec3[3] = { Vec3(0,0,0) , Vec3(0,0,0) ,Vec3(0,0,0) };
		float f = 0.0;
		Transform() = default;
		Transform(const toml::Value& transform){

			if(transform.type() != 7) {//テーブルじゃない
				std::cerr << "error transform1" << std::endl;
				return;
			}

			const toml::Value* ty = transform.find("type");
			if(ty == nullptr || !ty->is<std::string>()) {
				std::cerr << "error transform2" << std::endl;
				//delete ty;
				return;
			}

			if(ty->as<std::string>() == "axis-angle") {
				const toml::Value* axi = transform.find("axis");
				if(axi != nullptr && axi->is<toml::Array>()){
					float ax[3] = {0.0 , 1.0 , 0.0};int p = 0;
					for(const auto &a : axi->as<toml::Array>()) {
						if(p >= 3) {
							break;
						}
						if(a.is<double>()) {
							ax[p] = a.as<double>();
						}
						if(a.is<int>()) {
							ax[p] = a.as<int>();
						}
						p++;
					}
					vec3[0] = Vec3(ax[0],ax[1],ax[2]);
				}
				//delete axi;
				const toml::Value* ang = transform.find("angle");
				if(ang != nullptr && ang->is<double>()) {
					f = ang->as<double>();
				}
				if(ang != nullptr && ang->is<int>()) {
					f = ang->as<int>();
				}
				type = AXIS_ANGLE;
				//delete ang;
			}

			if(ty->as<std::string>() == "scale") {
				const toml::Value* vec = transform.find("vector");
				if(vec != nullptr && vec->is<toml::Array>()){
					float ve[3] = {0.0 , 1.0 , 0.0};int p = 0;
					for(const auto &v : vec->as<toml::Array>()) {
						if(p >= 3) {
							break;
						}
						if(v.is<double>()) {
							ve[p] = v.as<double>();
						}
						if(v.is<int>()) {
							ve[p] = v.as<int>();
						}
						p++;
					}
					vec3[0] = Vec3(ve[0],ve[1],ve[2]);
				}
				type = SCALE;
				//delete vec;
			}

			if(ty->as<std::string>() == "translate") {
				const toml::Value* vec = transform.find("vector");
				if(vec != nullptr && vec->is<toml::Array>()){
					float ve[3] = {0.0 , 1.0 , 0.0};int p = 0;
					for(const auto &v : vec->as<toml::Array>()) {
						if(p >= 3) {
							break;
						}
						if(v.is<double>()) {
							ve[p] = v.as<double>();
						}
						if(v.is<int>()) {
							ve[p] = v.as<int>();
						}
						p++;
					}
					vec3[0] = Vec3(ve[0],ve[1],ve[2]);
				}
				//delete vec;
				type = TRANSLATE;
			}

			if(ty->as<std::string>() == "look-at") {
				const toml::Value* vec_o = transform.find("origin");
				if(vec_o != nullptr && vec_o->is<toml::Array>()){
					float ve[3] = {0.0 , 1.0 , 0.0};int p = 0;
					for(const auto &v : vec_o->as<toml::Array>()) {
						if(p >= 3) {
							break;
						}
						if(v.is<double>()) {
							ve[p] = v.as<double>();
						}
						if(v.is<int>()) {
							ve[p] = v.as<int>();
						}
						p++;
					}
					vec3[0] = Vec3(ve[0],ve[1],ve[2]);
				}
				//delete vec_o;
				const toml::Value* vec_ta = transform.find("target");
				if(vec_ta != nullptr && vec_ta->is<toml::Array>()){
					float ve[3] = {0.0 , 1.0 , 0.0};int p = 0;
					for(const auto &v : vec_ta->as<toml::Array>()) {
						if(p >= 3) {
							break;
						}
						if(v.is<double>()) {
							ve[p] = v.as<double>();
						}
						if(v.is<int>()) {
							ve[p] = v.as<int>();
						}
						p++;
					}
					vec3[1] = Vec3(ve[0],ve[1],ve[2]);
				}
				//delete vec_ta;
				const toml::Value* vec_u = transform.find("up");
				if(vec_u != nullptr && vec_u->is<toml::Array>()){
					float ve[3] = {0.0 , 1.0 , 0.0};int p = 0;
					for(const auto &v : vec_u->as<toml::Array>()) {
						if(p >= 3) {
							break;
						}
						if(v.is<double>()) {
							ve[p] = v.as<double>();
						}
						if(v.is<int>()) {
							ve[p] = v.as<int>();
						}
						p++;
					}
					vec3[2] = Vec3(ve[0],ve[1],ve[2]);
				}
				//delete vec_u;
				type = LOOK_AT;
			}
			//delete ty;
		}
	};

	enum Mesh_type{
		SPHERE,
		TIRANGLE,
		OBJ
	};
	struct Mesh {
		Mesh_type type = SPHERE;
		std::string name = "";
		float radius = 0;
		std::string path = "";
		std::string normal = "strict";
		Mesh() = default;
		Mesh(const toml::Value &mesh) {
			if(mesh.type() != 7) {//テーブルじゃない
				std::cerr << "error mesh" << std::endl;
				return;
			}

			const  toml::Value* na = mesh.find("name");
			if(na == nullptr || !na->is<std::string>()) {
				std::cerr << "error mesh" << std::endl;
				//delete na;
				return;
			}
			name = na->as<std::string>();
			//delete na;

			const toml::Value* ty = mesh.find("type");
			if(ty == nullptr || !ty->is<std::string>()) {
				std::cerr << "error mesh" << std::endl;
				//delete ty;
				return;
			}

			if(ty->as<std::string>() == "sphere") {
				const toml::Value* rad = mesh.find("radius");
				if(rad != nullptr && rad->is<double>()){
					radius = rad->as<double>();
				}
				if(rad != nullptr && rad->is<int>()){
					radius = rad->as<int>();
				}
				type = SPHERE;
				//delete rad;
				type = SPHERE;
			}
			if(ty->as<std::string>() == "triangle") {
				type = TIRANGLE;
			}
			if(ty->as<std::string>() == "obj") {
				const toml::Value* pt = mesh.find("path");
				if(pt != nullptr && pt->is<std::string>()) {
					path = pt->as<std::string>();
				}
				//delete pt;
				const toml::Value* no = mesh.find("normal");
				if(no != nullptr && no->is<std::string>()) {
					normal = no->as<std::string>();
				}
				//delete no;
				type = OBJ;
			}
			//delete ty;
		}
	};

	enum Material_type{
		LAMBERT,
		PHONG,
	};
	struct Material {
		Material_type type = LAMBERT;
		std::string name = "";
		FColor albedo = FColor(0,0,0);
		FColor emission = FColor(0,0,0);
		Material() = default;
		Material(const toml::Value& material) {
			if(material.type() != 7){//テーブルじゃない
				std::cerr << "error material" << std::endl;
			}

			const toml::Value* na = material.find("name");
			if(na == nullptr || !na->is<std::string>()) {
				std::cerr << "error material" << std::endl;
				//delete na;
				return;
			}
			name = na->as<std::string>();
			//delete na;


			const toml::Value* ty = material.find("type");
			if(ty == nullptr || !ty->is<std::string>()) {
				std::cerr << "error material" << std::endl;
				//delete ty;
				return;
			}

			if(ty->as<std::string>() == "lambert"){
				const toml::Value* vec_a = material.find("albedo");
				if(vec_a != nullptr && vec_a->is<toml::Array>()){
					float ve[3] = {0.0 , 0.0 , 0.0};int p = 0;
					for(const auto &v : vec_a->as<toml::Array>()) {
						if(p >= 3) {
							break;
						}
						if(v.is<double>()) {
							ve[p] = v.as<double>();
						}
						if(v.is<int>()) {
							ve[p] = v.as<int>();
						}
						p++;
					}
					albedo = FColor(ve[0],ve[1],ve[2]);
				}
				//delete vec_a;
				const toml::Value* vec_e = material.find("emission");
				if(vec_e != nullptr && vec_e->is<toml::Array>()){
					float ve[3] = {0.0 , 0.0 , 0.0};int p = 0;
					for(const auto &v : vec_e->as<toml::Array>()) {
						if(v.is<double>()) {
							ve[p] = v.as<double>();
						}
						if(v.is<int>()) {
							ve[p] = v.as<int>();
						}
						p++;
					}
					emission = FColor(ve[0],ve[1],ve[2]);
				}
				//delete vec_e;
			}
			if(ty->as<std::string>() == "phong"){

			}
			//delete ty;
		}
	};

	enum Camera_type {
		IDEAL_PINHOLE,
		PINHOLE,
		THIN_LENS
	};
	struct Camera {
		Camera_type type = IDEAL_PINHOLE;
		float fov = 30;;
		std::vector<Transform> transform;
		Camera() {
			//Transformのデフォルトを入力;
			transform.push_back(Transform());
		}
		Camera(const toml::Value& camera) :Camera(){

			if(camera.type() != 7) {//テーブルじゃない
				std::cerr << "error camera" << std::endl;
				return;
			}

			const toml::Value* ty = camera.find("type");
			if(ty == nullptr || !ty->is<std::string>()) {
				std::cerr << "error camera" << std::endl;
				//delete ty;
				return;
			}

			if(ty->as<std::string>() == "ideal-pinhole"){
				const toml::Value* fo = camera.find("fov");
				if(fo != nullptr && fo->is<double>()){
					fov = fo->as<double>();
				}
				//delete fo;
				
				const toml::Value* trans = camera.find("transform");
				if(trans != nullptr && trans->is<toml::Array>()) {
					transform.clear();
					for(const auto &tran : trans->as<toml::Array>()) {
						transform.push_back(Transform(tran));
					}
				}
				type = IDEAL_PINHOLE;
				//delete trans;
				
			}
			//delete ty;
		}
	};

	struct Object{
		std::string name = "";
		std::string mesh = "";
		std::string material = "";
		std::vector<Transform> transform;
		Object() {
		}
		Object(const toml::Value& object) : Object() {

			if(object.type() != 7) {//デーブルじゃない
				std::cerr << "error object " << std::endl;
				return;
			}

			const toml::Value* na = object.find("name");
			if(na != nullptr && na->is<std::string>()){
				name = na->as<std::string>();
			}
			//delete na;

			const toml::Value* me = object.find("mesh");
			if(me == nullptr || !me->is<std::string>()){
				std::cerr << "error object" << std::endl;
				//delete me;
				return;
			}
			mesh = me->as<std::string>();
			//delete me;

			const toml::Value* ma = object.find("material");
			if(ma != nullptr && ma->is<std::string>()) {
				material = ma->as<std::string>();
			}
			//delete ma;

			const toml::Value* trans = object.find("transform");
			if(trans != nullptr && trans->is<toml::Array>()) {
				for(const auto &tran : trans->as<toml::Array>()) {
					transform.push_back(Transform(tran));
				}
			}
			//delete trans;
		}
	};

	enum Light_type {
		AREA,
		DIRECTIONAL,
		POINT
	};
	struct Light {
		Light_type type = AREA;
		FColor emission = FColor(0,0,0);
		std::string object = "";
		Light() = default;
		Light(const toml::Value& light) {

			if(light.type() != 7) {//デーブルじゃない
				std::cerr << "error light" << std::endl;
				return;
			}
			const toml::Value* vec_e = light.find("emission");
			if(vec_e == nullptr || !vec_e->is<toml::Array>()){
				std::cerr << "error light" << std::endl;
				//delete vec_e;
				return ;
			}
			float ve[3] = {0.0 , 0.0 , 0.0};int p = 0;
			for(const auto &v : vec_e->as<toml::Array>()) {
				if(p >= 3) {
					break;
				}
				if(v.is<double>()) {
					ve[p] = v.as<double>();
				}else if(v.is<int>()) {
					ve[p] = v.as<int>();
				}else{
					std::cerr << "error light" << std::endl;
					//delete vec_e;
					return;
				}
				p++;
			}
			emission = FColor(ve[0],ve[1],ve[2]);
			//delete vec_e;

			const toml::Value* ty = light.find("type");
			if(ty == nullptr || !ty->is<std::string>()){
				std::cerr << "error light" << std::endl;
				//delete ty;
				return;
			}

			if(ty->as<std::string>() == "area") {
				const toml::Value* ob = light.find("object");
				if(ob == nullptr || !ob->is<std::string>()){
					std::cerr << "error area light" << std::endl;
					//delete ty;
					//delete ob;
					return;
				}
				object = ob->as<std::string>();
				type = AREA;
			}
			//delete ty;
		}
	};

	enum Sky_type {
		UNIFORM,
		IBL
	};
	struct Sky {
		Sky_type type = UNIFORM;
		FColor color = FColor(0.75,0.75,0.75);
		std::string path = "";
		float theta_offset = 0;
		float phi_offset = 0;
		Sky() = default;
		Sky(const toml::Value& sky) {
			if(sky.type() != 7) {//デーブルじゃない
				std::cerr << "error light" << std::endl;
				return;
			}

			const toml::Value* ty = sky.find("type");
			if(ty == nullptr || !ty->is<std::string>()){
				std::cerr << "error light" << std::endl;
				//delete ty;
				return;
			}

			if(ty->as<std::string>() == "uniform") {
				const toml::Value* vec_c = sky.find("color");
				if(vec_c == nullptr || !vec_c->is<toml::Array>()){
					std::cerr << "error uniform sky" << std::endl;
					//delete vec_c;
					//delete ty;
					return ;
				}
				float ve[3] = {0.0 , 0.0 , 0.0};int p = 0;
				for(const auto &v : vec_c->as<toml::Array>()) {
					if(p >= 3) {
						break;
					}
					if(v.is<double>()) {
						ve[p] = v.as<double>();
					}else if(v.is<int>()) {
						ve[p] = v.as<int>();
					}else{
						std::cerr << "error uniform sky" << std::endl;
						//delete vec_c;
						//delete ty;
						return;
					}
					p++;
				}
				color = FColor(ve[0],ve[1],ve[2]);
				type = UNIFORM;
				//delete vec_c;
			}
			if(ty->as<std::string>() == "ibl") {
				const toml::Value* pt = sky.find("path");
				if(pt == nullptr || !pt->is<std::string>()) {
					std::cerr << "error ibl sky" << std::endl;
					//delete pt;
					//delete ty;
					return;
				}
				path = pt->as<std::string>();
				//delete pt;

				const toml::Value* to = sky.find("theta-offset");
				if(to != nullptr && to->is<double>()) {
					theta_offset = to->as<double>();
				}
				if(to != nullptr && to->is<int>()) {
					theta_offset = to->as<int>();
				}
				//delete to;

				const toml::Value* po = sky.find("phi-offset");
				if(po != nullptr && po->is<double>()) {
					phi_offset = po->as<double>();
				}
				if(po != nullptr && po->is<int>()) {
					phi_offset = po->as<int>();
				}
				//delete po;
				type = IBL;
			}
			//delete ty;
		}
	};

	struct Film {
		float resolution[2] = {480,480};
		std::string output = "ppm";
		float gamma = 2.2;
		FColor sensitivity = FColor(1.0,1.0,1.0);
		Film() = default;
		Film(const toml::Value& film) {
			if(film.type() != 7) {//デーブルじゃない
				std::cerr << "error film " << std::endl;
				return;
			}
			const toml::Value* re = film.find("resolution");
			if(re == nullptr || !re->is<toml::Array>()) {
				std::cerr << "error film" << std::endl;
				//delete re;
				return ;
			}
			{
				int p = 0;
				for(const auto &v : re->as<toml::Array>()) {
					if(p >= 2) {
						break;
					}
					if(v.is<double>()) {
						resolution[p] = v.as<double>();
					}else if(v.is<int>()) {
						resolution[p] = v.as<int>();
					}else{
						std::cerr << "error film" << std::endl;
						//delete re;
						return;
					}
					p++;
				}
				//delete re;
				if(p < 2) {
						std::cerr << "error film" << std::endl;
						return;
				}
			}

			const toml::Value* ou = film.find("output");
			if(ou == nullptr || !ou->is<std::string>()) {
				std::cerr << "error film" << std::endl;
				//delete ou;
				return;
			}
			output = ou->as<std::string>();

			const toml::Value* ga = film.find("gamma");
			if(ga != nullptr && ga->is<double>()) {
				gamma = ga->as<double>();
			}
			if(ga != nullptr && ga->is<int>()) {
				gamma = ga->as<int>();
			}
			//delete ga;

			const toml::Value* vec_s = film.find("sensitivity");
			if(vec_s != nullptr || vec_s->is<toml::Array>()){
			
				float ve[3] = {1.0 , 1.0 , 1.0};int p = 0;
				for(const auto &v : vec_s->as<toml::Array>()) {
					if(p >= 3) {
						break;
					}
					if(v.is<double>()) {
						ve[p] = v.as<double>();
					}else if(v.is<int>()) {
						ve[p] = v.as<int>();
					}
					p++;
				}
				sensitivity = FColor(ve[0],ve[1],ve[2]);
			}
			//delete vec_s;
		}
	};

	struct Renderer {
		int samples = 64;
		int depth = 4;
		int depth_limit = 8;
		bool no_direct_emitter = false;
		int threads = 8;
		std::string integrator = "pt";
		Renderer() = default;
		Renderer(const toml::Value& renderer) {

			if(renderer.type() != 7) {//デーブルじゃない
				std::cerr << "error renderer " << std::endl;
				return;
			}

			const toml::Value* sa = renderer.find("samples");
			if(sa == nullptr || !sa->is<int>()) {
				std::cerr << "error renderer" << std::endl;
				//delete sa;
				return;
			}
			samples = sa->as<int>();
			//delete sa;

			const toml::Value* de = renderer.find("depth");
			if(de != nullptr && de->is<int>()) {
				depth = de->as<int> ();
			}
			//delete de;

			const toml::Value* dl = renderer.find("depth-limit");
			if(dl != nullptr && dl->is<int>()) {
				depth_limit = dl->as<int> ();
			}
			//delete dl;

			const toml::Value* nd = renderer.find("no-direct-emitter");
			if(nd != nullptr && nd->is<bool>()) {
				no_direct_emitter = nd->as<bool> ();
			}
			//delete nd;

			const toml::Value* th = renderer.find("threads");
			if(th != nullptr && th->is<int>()) {
				threads = th->as<int> ();
			}
			//delete th;

			const toml::Value* it = renderer.find("integrator");
			if(it != nullptr && it->is<std::string>()) {
				integrator = it->as<std::string> ();
			}
			//delete it;
		}
	};

	struct Root {
		Renderer renderer;
		Sky sky;
		Film film;
		Camera camera;
		std::vector<Light> light;
		std::vector<Object> object;
		std::vector<Material> material;
		std::vector<Mesh> mesh;
		Root() = default;
		Root(const toml::Value& root) {
			
			const toml::Value* re = root.find("renderer");
			if(re == nullptr){
				std::cerr << "error renderer" << std::endl;
				return;
			}
			renderer = Renderer(*re);
			//delete re;

			const toml::Value* sk = root.find("sky");
			if(sk == nullptr){
				std::cerr << "error sky" << std::endl;
				return;
			}
			sky = Sky(*sk);
			//delete sk;

			const toml::Value* fi = root.find("film");
			if(fi == nullptr){
				std::cerr << "error film" << std::endl;
				return;
			}
			film = Film(*fi);
			//delete fi;

			const toml::Value* ca =root.find("camera");
			if(ca == nullptr){
				std::cerr << "error camera" << std::endl;
				return;
			}
			camera = Camera(*ca);
			//delete ca;

			const toml::Value* li = root.find("light");
			if(li != nullptr && li->is<toml::Array>()) {
				for(const auto & l : li->as<toml::Array>()) {
					light.push_back(Light(l));
				}
			}
			//delete li;

			const toml::Value* ob = root.find("object");
			if(ob != nullptr && ob->is<toml::Array>()) {
				for(const auto & o : ob->as<toml::Array>()) {
					object.push_back(Object(o));
				}
			}
			//delete ob;

			const toml::Value* ma = root.find("material");
			if(ma != nullptr && ma->is<toml::Array>()) {
				for(const auto &m : ma->as<toml::Array>()) {
					material.push_back(Material(m));
				}
			}
			//delete ma;

			const toml::Value* me = root.find("mesh");
			if(me != nullptr && me->is<toml::Array>()) {
				for(const auto & m : me->as<toml::Array>()) {
					mesh.push_back(Mesh(m));
				}
			}
			//delete me;
		}

		void out() {
			std::cout << "[renderer]\n" << std::endl;
			std::cout << renderer.samples << std::endl;
			std::cout << renderer.depth << std::endl;
			std::cout << renderer.depth_limit << std::endl;
			std::cout << renderer.no_direct_emitter << std::endl;
			std::cout << renderer.threads << std::endl;
			std::cout << renderer.integrator << "\n\n" << std::endl;

			std::cout << "[sky]\n" << std::endl;
			std::cout << sky.type << std::endl;
			std::cout << sky.color << std::endl;
			std::cout << sky.path << std::endl;
			std::cout << sky.theta_offset << std::endl;
			std::cout << sky.phi_offset << "\n\n" << std::endl;

			std::cout << "[film]\n" << std::endl;
			std::cout << film.resolution[0] << " " << film.resolution[1] << std::endl;
			std::cout << film.output << std::endl;
			std::cout << film.gamma << std::endl;
			std::cout << film.sensitivity << "\n\n" << std::endl;

			std::cout << "[camera]\n" << std::endl;
			std::cout << camera.type << std::endl;
			std::cout << camera.fov << std::endl;
			for(const auto &transform : camera.transform){
				std::cout << transform.type << std::endl;
				for(int i = 0;i < 3;i++){
					std::cout << transform.vec3[i] << std::endl;
				}
				std::cout << transform.f << std::endl;
			}
			std::cout << "\n\n" << std::endl;

			std::cout << "[[light]]\n" << std::endl;
			for(const auto &li : light){
				std::cout << li.emission << std::endl;
				std::cout << li.type << std::endl;
				std::cout << li.object << std::endl;
			}
			std::cout << "\n\n" << std::endl;

			std::cout << "[[object]]\n" << std::endl;
			for(const auto &ob : object){
				std::cout << ob.name << std::endl;
				std::cout << ob.mesh << std::endl;
				std::cout << ob.material << std::endl;
				for(const auto &transform : ob.transform){
					std::cout << transform.type << std::endl;
					for(int i = 0;i < 3;i++){
						std::cout << transform.vec3[i] << std::endl;
					}
					std::cout << transform.f << std::endl;
				}
			}
			std::cout << "\n\n" << std::endl;

			std::cout << "[[material]]\n" << std::endl;
			for(const auto &ma : material){
				std::cout << ma.name<< std::endl;
				std::cout << ma.type << std::endl;
				std::cout << ma.albedo<< std::endl;
				std::cout << ma.emission << std::endl;
			}
			std::cout << "\n\n" << std::endl;

			std::cout << "[[mesh]]\n" << std::endl;
			for(const auto &me : mesh){
				std::cout << me.name<< std::endl;
				std::cout << me.type << std::endl;
				std::cout << me.radius << std::endl;
				std::cout << me.path << std::endl;
				std::cout << me.normal << std::endl;
			}
		}
	};
}
