#pragma once

#include <vector>
#include <chrono>
#include "Shape.h"
#include "Area_light_source.h"
#include "Intersection_info.h"
#include "Light_info.h"
#include "Camera.h"
#include "Infinity_light_source.h"
#include "Random.h"

struct Scene{
	std::vector<Shape*> shapes;//物体
	std::vector<Light_source*> lights;//光源
	R *light_P = nullptr;
	const int HEIGHT = 480;//縦のピクセル
	const int WIDTH = 480;//横のピクセル
	const R INF = 1000000000.0;
	//const FColor back;//物体がないときの色
	const Infinity_light_source back;
	const int MIN_DEPTH = 4;//再帰の深さの最小値(オブジェクトに当たらなかった時は別)
	const int MAX_DEPTH = 32;//再帰の深さの最大値
	const int threads = 4; //スレッドの数
	FColor *img;//ピクセルごとの色を保持
	const Camera camera;

	inline Scene(): back(/*FColor(100.0 / 255,149.0 / 255,237.0 / 255)*/"hdr/PaperMill_E_3k.hdr"),img(new FColor[HEIGHT * WIDTH]),camera(480,480){}
	inline Scene(int w,int h) :HEIGHT(h),WIDTH(w),back(/*FColor(100.0 / 255,149.0 / 255,237.0 / 255)*/"hdr/PaperMill_E_3k.hdr"),img(new FColor[HEIGHT * WIDTH]),camera(WIDTH,HEIGHT){};
	inline Scene(Camera c) :HEIGHT(c.picH),WIDTH(c.picW),back(/*FColor(100.0 / 255,149.0 / 255,237.0 / 255)*/"hdr/PaperMill_E_3k.hdr"),img(new FColor[HEIGHT * WIDTH]),camera(c){};
	inline Scene(Camera c,int min_depth,int max_depth,int th,std::string ibl,const FColor &cl) :HEIGHT(c.picH),WIDTH(c.picW),back(/*FColor(100.0 / 255,149.0 / 255,237.0 / 255)*/ibl,cl),MIN_DEPTH(min_depth),MAX_DEPTH(max_depth),threads(th),img(new FColor[HEIGHT * WIDTH]),camera(c){};
	inline void add(Shape *shape){//物体を追加する
		shapes.push_back(shape);
	}

	inline void add_as_light(Shape *shape){
		add(shape);

		lights.push_back(new Area_light_source(shape));
	}

	inline void set_light() {
		if(lights.size() < 1)
			return;
		light_P = new R[lights.size() + 1];

		light_P[0] = 0.0;

		for(int i = 0;i < lights.size();i++) {
			light_P[i + 1] = light_P[i] + lights[i]->get_weight();
		}
		const R d = 1.0 / light_P[lights.size()];
		for(int i = 1;i < lights.size() + 1;i++) {
			light_P[i] *= d;
		}
	}

	inline void init_img(){
		#pragma omp parallel for schedule(dynamic, 1) num_threads(threads)
		for(int i = 0;i < HEIGHT * WIDTH;i++)
			img[i] = FColor(0,0,0);
	}

	inline Intersection_info* get_intersection_of_nearest(const Ray &ray) const{//一番近い物体の情報を取得する　どちらもnullptrなら物体は存在しない
		R min_t = INF;
		Intersection_info *intersection_info = new Intersection_info();

		for(Shape *shape : shapes){
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

	inline bool is_shadow(const Ray &ray,const Vec3 light_point) const {//光源までの間に物体がないか調べる
		const R max_t = (light_point - ray.start).abs() - 0.1;
		for(Shape *shape : shapes){
			Intersection_point *intersection = shape->get_intersection(ray);
			if(intersection != nullptr && max_t > intersection->distance ){
				//if((light_point - intersection->position).abs() < EPS) {
					//std::cerr << "aa" << std::endl;
				//}
				delete intersection;
				return true;
			}
			delete intersection;
		}
		return false;
	}

	inline void nee(const Vec3 &position,const Vec3 &normal,FColor &lgp,Vec3 &omegal) const {

		const R u = rando();

		for(int i = 0;i < lights.size();i++) {
			if(u < light_P[i + 1] || i - 1 == lights.size()){
				Light_info *light_info = lights[i]->light_point();

				const Vec3 path = light_info->point - position;
				omegal = path.normalized();

				//if(is_shadow(Ray(position,omegal),path.abs(),lights[i]->get_shape())) {
				if(is_shadow(Ray(position,omegal),light_info->point)) {
					delete light_info;
					break;
				}

				const R wl_dot_nl = (-omegal) * light_info->normal;
				if(wl_dot_nl < EPS){
					delete light_info;
					break;
				}

				const R w1_dot_n1 = omegal * normal;
				if(w1_dot_n1  < EPS){
					delete light_info;
					break;
				}

				lgp = light_info->emission
						* w1_dot_n1 * wl_dot_nl
						* (1.0 / (path.abs_square() * (light_P[i + 1] - light_P[i]) * light_info->pdf));
				//std::cerr << (light_P[i + 1] - light_P[i]) * light_info->pdf << std::endl;
				delete light_info;
				return;
			}
		}
		lgp = FColor(0,0,0);
	}

	inline FColor pathtracing(const Ray &ray,const int depth) const{
		const Intersection_info *intersection_info = get_intersection_of_nearest(ray);
			
		if(intersection_info == nullptr){//物体が存在しない
			return back.get_radiance(ray.direction);
		}
		
		const Intersection_point *intersection = intersection_info->intersection_point;
		//const Shape *intersection_shape = intersection_info->shape;
		const Material material = intersection->material;//intersection_shape->get_material(intersection->position);
		const Vec3 normal = ((ray.direction * intersection->normal < 0.0) ? 1.0 : -1.0) * intersection->normal;
		
		FColor L = (ray.direction * intersection->normal < 0.0) ? material.Le : FColor(0,0,0);

		R P = std::max(std::max(material.kd.red,material.kd.green),material.kd.blue);

		if(depth < MIN_DEPTH)
			P = 1.0;
		
		if (depth >= MAX_DEPTH)//最大値で大幅に確率を下げる
			P *= pow(0.5, depth - MAX_DEPTH);
		
		

		if(P < rando()){//再帰が深すぎ
			delete intersection_info;
			return L;
		}

		switch (material.type){

		case MT_DEFAULT: {
			Vec3 u;
			if (std::abs(normal.x) > 1e-6) // ベクトルwと直交するベクトルを作る。w.xが0に近い場合とそうでない場合とで使うベクトルを変える。
				u = (cross(Vec3(0.0, 1.0, 0.0), normal)).normalized();
			else
				u = (cross(Vec3(1.0, 0.0, 0.0), normal)).normalized();
			Vec3 v = cross(normal,u);

			R u1 = rando() * 2.0 * 3.1415926535,u2 = rando(),u3 = std::sqrt(u2);

			Vec3 omega = 
				u * std::cos(u1) * u3 +
				v * std::sin(u1) * u3 +
				normal * std::sqrt(1 - u2);

			L += material.kd * pathtracing(Ray(intersection->position /*+ epsilon * omega*/,omega),depth + 1) * (1.0 / P);
		} break;
		case MT_PERFECT_REF: {
			const Vec3 r = -2.0 * (normal * ray.direction) * normal + ray.direction;
			const Vec3 p = intersection->position;
			L += material.kd * pathtracing(Ray(p,r),depth + 1) * (1.0 / P);
		} break;
		case MT_REFRACTION: {
			const R n = ((ray.direction * intersection->normal < 0.0) ? 1.0 / material.n : material.n);
			const R dn = ray.direction * normal;
			const R cos2t = 1.0 - n * n * (1.0 - dn * dn);

			const Vec3 p = intersection->position;
			const Vec3 r = -2.0 * (normal * ray.direction) * normal + ray.direction;

			if(cos2t < 0.0){
				L += material.kd * pathtracing(Ray(p,r),depth + 1) * (1.0 / P);
				break;
			}

			Vec3 T = (n * ray.direction - (n * dn + std::sqrt(cos2t) ) * normal).normalized();
			const R a = 1.0 - material.n,b = 1.0 + material.n;
			const R F0 = (a * a) / (b * b);

			const R Fr = F0 + (1.0 - F0) * std::pow(1.0 + ((ray.direction * intersection->normal < 0.0) ? dn : T * normal),5);
			const R Tr = (1.0 - Fr) * n * n;

			const R probability = 0.25 * 0.5 * Fr;
			if(depth > 2){
				if(rando() < probability){
					L += material.kd * pathtracing(Ray(p,r),depth + 1) * (1.0 / P / probability) * Fr;
				}else{
					L += material.kd * pathtracing(Ray(p,T),depth + 1) * (1.0 / P / (1.0 - probability)) * Tr;
				}
			}else {
				L += material.kd * (pathtracing(Ray(p,r),depth + 1) * Fr + pathtracing(Ray(p,T),depth + 1) * Tr) * (1.0 / P);
			}
		} break;
		case MT_NORMALIZED_PHONG: {
			Vec3 u;
			if (std::abs(normal.x) > 1e-6) // ベクトルwと直交するベクトルを作る。w.xが0に近い場合とそうでない場合とで使うベクトルを変える。
				u = (cross(Vec3(0.0, 1.0, 0.0), normal)).normalized();
			else
				u = (cross(Vec3(1.0, 0.0, 0.0), normal)).normalized();
			const Vec3 v = cross(normal,u);

			const R u1 = rando() * 2.0 * 3.1415926535,t = std::pow(rando(),1.0 / (2 + material.n));//alpha = 5
			const R t2 = std::sqrt(1.0 - t * t);

			const Vec3 omega = 
				u * std::cos(u1) * t2 +
				v * std::sin(u1) * t2 +
				normal * t;

			const R nome = normal * omega;
			const R f = (omega - 2 * nome * normal) * ray.direction / nome;

			L += material.kd * std::pow(f,material.n) * pathtracing(Ray(intersection->position /*+ epsilon * omega*/,omega),depth + 1) * (1.0 / P);
		}
		}

		delete intersection_info;
		return L;
	}

	inline FColor pathtracing_direct(const Ray &ray,const int depth,const bool flag) const{
		const Intersection_info *intersection_info = get_intersection_of_nearest(ray);
			
		if(intersection_info == nullptr){//物体が存在しない
			return (flag) ? back.get_radiance(ray.direction) : FColor(0,0,0);
		}
		
		const Intersection_point *intersection = intersection_info->intersection_point;
		//const Shape *intersection_shape = intersection_info->shape;
		const Material material = intersection->material;//intersection_shape->get_material(intersection->position);
		const Vec3 normal = ((ray.direction * intersection->normal < 0.0) ? 1.0 : -1.0) * intersection->normal;
		
		FColor L = (flag && ray.direction * intersection->normal < 0.0) ? material.Le : FColor(0,0,0);

		R P = std::max(std::max(material.kd.red,material.kd.green),material.kd.blue);

		if(depth < MIN_DEPTH)
			P = 1.0;
		
		if (depth >= MAX_DEPTH)//最大値で大幅に確率を下げる
			P *= pow(0.5, depth - MAX_DEPTH);
		
		

		if(P < rando()){//再帰が深すぎ
			delete intersection_info;
			return L;
		}

		switch (material.type){

		case MT_DEFAULT: {
			FColor lgp;Vec3 omegal;
			nee(intersection->position,normal,lgp,omegal);
			L += material.kd * M_1_PI * lgp;


			Vec3 u;
			if (std::abs(normal.x) > 1e-6) // ベクトルwと直交するベクトルを作る。w.xが0に近い場合とそうでない場合とで使うベクトルを変える。
				u = (cross(Vec3(0.0, 1.0, 0.0), normal)).normalized();
			else
				u = (cross(Vec3(1.0, 0.0, 0.0), normal)).normalized();
			Vec3 v = cross(normal,u);

			R u1 = rando() * 2.0 * 3.1415926535,u2 = rando(),u3 = std::sqrt(u2);

			Vec3 omega = 
				u * std::cos(u1) * u3 +
				v * std::sin(u1) * u3 +
				normal * std::sqrt(1 - u2);

			L += material.kd * pathtracing_direct(Ray(intersection->position /*+ epsilon * omega*/,omega),depth + 1,false) * (1.0 / P);
		} break;
		case MT_PERFECT_REF: {
			const Vec3 r = -2.0 * (normal * ray.direction) * normal + ray.direction;
			const Vec3 p = intersection->position;
			L += material.kd * pathtracing_direct(Ray(p,r),depth + 1,true) * (1.0 / P);
		} break;
		case MT_REFRACTION: {
			const R n = ((ray.direction * intersection->normal < 0.0) ? 1.0 / material.n : material.n);
			const R dn = ray.direction * normal;
			const R cos2t = 1.0 - n * n * (1.0 - dn * dn);

			const Vec3 p = intersection->position;
			const Vec3 r = -2.0 * (normal * ray.direction) * normal + ray.direction;

			if(cos2t < 0.0){
				L += material.kd * pathtracing_direct(Ray(p,r),depth + 1,true) * (1.0 / P);
				break;
			}

			Vec3 T = (n * ray.direction - (n * dn + std::sqrt(cos2t) ) * normal).normalized();
			const R a = 1.0 - material.n,b = 1.0 + material.n;
			const R F0 = (a * a) / (b * b);

			const R Fr = F0 + (1.0 - F0) * std::pow(1.0 + ((ray.direction * intersection->normal < 0.0) ? dn : T * normal),5);
			const R Tr = (1.0 - Fr) * n * n;

			const R probability = 0.25 * 0.5 * Fr;
			if(depth > 2){
				if(rando() < probability){
					L += material.kd * pathtracing_direct(Ray(p,r),depth + 1,true) * (1.0 / P / probability) * Fr;
				}else{
					L += material.kd * pathtracing_direct(Ray(p,T),depth + 1,true) * (1.0 / P / (1.0 - probability)) * Tr;
				}
			}else {
				L += material.kd * (pathtracing_direct(Ray(p,r),depth + 1,true) * Fr + pathtracing_direct(Ray(p,T),depth + 1,true) * Tr) * (1.0 / P);
			}
		} break;
		case MT_NORMALIZED_PHONG: {
			FColor lgp;Vec3 omegal;
			nee(intersection->position,normal,lgp,omegal);
			L += material.kd * ((material.n + 2.0) * 0.5 * M_1_PI) * std::pow((omegal - 2.0 * (normal * omegal) * normal) * ray.direction,material.n) * lgp;

			Vec3 u;
			if (std::abs(normal.x) > 1e-6) // ベクトルwと直交するベクトルを作る。w.xが0に近い場合とそうでない場合とで使うベクトルを変える。
				u = (cross(Vec3(0.0, 1.0, 0.0), normal)).normalized();
			else
				u = (cross(Vec3(1.0, 0.0, 0.0), normal)).normalized();
			const Vec3 v = cross(normal,u);

			const R u1 = rando() * 2.0 * 3.1415926535,t = std::pow(rando(),1.0 / (2 + material.n));//alpha = 5
			const R t2 = std::sqrt(1.0 - t * t);

			const Vec3 omega = 
				u * std::cos(u1) * t2 +
				v * std::sin(u1) * t2 +
				normal * t;

			const R nome = normal * omega;
			const R f = (omega - 2 * nome * normal) * ray.direction / nome;

			L += material.kd * std::pow(f,material.n) * pathtracing_direct(Ray(intersection->position /*+ epsilon * omega*/,omega),depth + 1,false) * (1.0 / P);
		}
		}

		delete intersection_info;
		return L;
	}

	inline FColor next_event_estimation(const Ray &input_ray) const{
		FColor L(0,0,0);
		FColor weight(1,1,1);
		Ray ray(input_ray.start,input_ray.direction);
		int depth = 0;

		bool flag = true;
		while(true){
			const Intersection_info *intersection_info = get_intersection_of_nearest(ray);
			
			if(intersection_info == nullptr){//物体が存在しない
				if(flag)
					L += weight * back.get_radiance(ray.direction);
				break;
				//return back.get_radiance(ray.direction);
			}

			const Intersection_point *intersection = intersection_info->intersection_point;
			//const Shape *intersection_shape = intersection_info->shape;
			const Material material = intersection->material;//intersection_shape->get_material(intersection->position);
			const Vec3 normal = ((ray.direction * intersection->normal < 0.0) ? 1.0 : -1.0) * intersection->normal;
		
			L += weight * ((flag && ray.direction * intersection->normal < 0.0) ? material.Le : FColor(0,0,0));

			R P = std::max(std::max(material.kd.red,material.kd.green),material.kd.blue);

			if(depth < MIN_DEPTH)
				P = 1.0;
		
			if (depth >= MAX_DEPTH)//最大値で大幅に確率を下げる
				P *= pow(0.5, depth - MAX_DEPTH);
		

			if(P < rando()){//再帰が深すぎ
				delete intersection_info;
				//return L;
				break;
			}

			switch (material.type){

			case MT_DEFAULT: {

				FColor lgp;Vec3 omega1;
				nee(intersection->position,normal,lgp,omega1);

				L += weight * material.kd * M_1_PI * lgp;

				Vec3 u;
				if (std::abs(normal.x) > 1e-6) // ベクトルwと直交するベクトルを作る。w.xが0に近い場合とそうでない場合とで使うベクトルを変える。
					u = (cross(Vec3(0.0, 1.0, 0.0), normal)).normalized();
				else
					u = (cross(Vec3(1.0, 0.0, 0.0), normal)).normalized();
				Vec3 v = cross(normal,u);

				R u1 = rando() * 2.0 * 3.1415926535,u2 = rando(),u3 = std::sqrt(u2);

				Vec3 omega = 
					u * std::cos(u1) * u3 +
					v * std::sin(u1) * u3 +
					normal * std::sqrt(1 - u2);

				ray = Ray(intersection->position /*+ epsilon * omega*/,omega);
				weight *= material.kd * (1.0 / P);
				flag = false;
			} break;
			case MT_PERFECT_REF: {
				const Vec3 r = -2.0 * (normal * ray.direction) * normal + ray.direction;
				const Vec3 p = intersection->position;
				ray = Ray(p,r);
				weight *= material.kd * (1.0 / P);
				flag = true;
			} break;
			case MT_REFRACTION: {
				const R n = ((ray.direction * intersection->normal < 0.0) ? 1.0 / material.n : material.n);
				const R dn = ray.direction * normal;
				const R cos2t = 1.0 - n * n * (1.0 - dn * dn);

				const Vec3 p = intersection->position;
				const Vec3 r = -2.0 * (normal * ray.direction) * normal + ray.direction;

				if(cos2t < 0.0){
					ray = Ray(p,r);
					weight *= material.kd * (1.0 / P);
					break;
				}

				Vec3 T = (n * ray.direction - (n * dn + std::sqrt(cos2t) ) * normal).normalized();
				const R a = 1.0 - material.n,b = 1.0 + material.n;
				const R F0 = (a * a) / (b * b);

				const R Fr = F0 + (1.0 - F0) * std::pow(1.0 + ((ray.direction * intersection->normal < 0.0) ? dn : T * normal),5);
				const R Tr = (1.0 - Fr) * n * n;

				const R probability = 0.25 * 0.5 * Fr;
				//if(depth > 2){
					if(rando() < probability){
						ray = Ray(p,r);
						weight *= material.kd * (1.0 / P / probability) * Fr;
					}else{
						ray = Ray(p,T);
						weight *= material.kd * (1.0 / P / (1.0 - probability)) * Tr;
					}
				//}else {
					//L += material.kd * (pathtracing(Ray(p,r),depth + 1) * Fr + pathtracing(Ray(p,T),depth + 1) * Tr) * (1.0 / P);
				//}
				flag = true;
			} break;
			case MT_NORMALIZED_PHONG: {
				Vec3 u;
				if (std::abs(normal.x) > 1e-6) // ベクトルwと直交するベクトルを作る。w.xが0に近い場合とそうでない場合とで使うベクトルを変える。
					u = (cross(Vec3(0.0, 1.0, 0.0), normal)).normalized();
				else
					u = (cross(Vec3(1.0, 0.0, 0.0), normal)).normalized();
				const Vec3 v = cross(normal,u);

				const R u1 = rando() * 2.0 * 3.1415926535,t = std::pow(rando(),1.0 / (2 + material.n));//alpha = 5
				const R t2 = std::sqrt(1.0 - t * t);

				const Vec3 omega = 
					u * std::cos(u1) * t2 +
					v * std::sin(u1) * t2 +
					normal * t;

				const R nome = normal * omega;
				const R f = (omega - 2 * nome * normal) * ray.direction / nome;

				ray = Ray(intersection->position /*+ epsilon * omega*/,omega);
				weight *= material.kd * std::pow(f,material.n) * (1.0 / P);
				flag = false;
			}
			}
			delete intersection_info;
			depth++;
		}
		return L;
	}	


	void compute(int n,int N,int supersamples){//サンプル回数とすでに何回サンプリングしているか
		if(N == 0){
			init_img();
			set_light();
		}
		const R r = 1.0 / (n * supersamples * supersamples + N);
		#pragma omp parallel for schedule(dynamic, 1) num_threads(threads)
		for(int i = 0;i < HEIGHT;i++){
			for(int j = 0;j < WIDTH;j++){
				img[i * WIDTH + j] *= N;
				for(int sy = 0;sy < supersamples;sy++){
					for(int sx = 0;sx < supersamples;sx++){
						const Ray ray = camera.get_ray(sy,sx,supersamples,i,j);
						for(int k = 0;k < n;k++){
							img[i * WIDTH + j] += pathtracing_direct(ray,0,true);
							//img[i * WIDTH + j] += next_event_estimation(ray);
							//img[i * WIDTH + j] += pathtracing(ray,0);
						}
					}
				}
				img[i * WIDTH + j] *= r;
			}
		}
	}

	void draw(int sample,int supersamples) {

		printf("P3\n%d %d\n255\n", WIDTH,HEIGHT);

		compute(sample,0,supersamples);

		for(int i = 0;i < HEIGHT * WIDTH;i++)
			img[i].print255();
	}
	void debug(int sample,int supersamples) {
		compute(sample,0,supersamples);
	}

	void normal_render(){
		init_img();
		printf("P3\n%d %d\n255\n", WIDTH,HEIGHT);
	    std::chrono::system_clock::time_point start,end;
        start = std::chrono::system_clock::now();
		for(int i = 0;i < HEIGHT;i++){
			//std::cerr << i << std::endl;
			#pragma omp parallel for schedule(dynamic, 1) num_threads(threads)
			for(int j = 0;j < WIDTH;j++){
				const Ray ray = camera.get_ray(i,j);
				Intersection_info *intersection_info = get_intersection_of_nearest(ray);
				if(intersection_info == nullptr){
					img[i * WIDTH + j] = FColor(0,0,0);
					continue;
				}
				const Vec3 &normal = intersection_info->intersection_point->normal;
				img[i * WIDTH + j] = FColor((normal.x + 1.0) * 0.5,(normal.y + 1.0) * 0.5,(normal.z + 1.0) * 0.5);
				delete intersection_info;
			}
		}
	    end = std::chrono::system_clock::now();

        auto elapsed = std::chrono::duration_cast< std::chrono::nanoseconds >(end - start).count();
        std::cerr <<"rays " << ((R)(WIDTH * HEIGHT) / (elapsed * 1e-9)) * 10e-6 << " M ray / s" << std::endl;
        std::cerr <<"rays " << elapsed << " nano seconds" << std::endl;
		for(int i = 0 ;i < WIDTH * HEIGHT;i++){
			R r = img[i].red;
        	R g = img[i].green;
        	R b = img[i].blue;
        	r = std::max((R)0.0,r);r = std::min((R)1.0,r);
        	g = std::max((R)0.0,g);g = std::min((R)1.0,g);
        	b = std::max((R)0.0,b);b = std::min((R)1.0,b);
        	int ir = r * 255 + 0.5;
        	int ig = g * 255 + 0.5;
        	int ib = b * 255 + 0.5;

        	printf("%d %d %d\n",ir,ig,ib);
		}
	}

	void neraiuti(int n,int y,int x,int Y,int X){
		init_img();
		set_light();
		printf("P3\n%d %d\n255\n", X - x,Y - y);
		//#pragma omp parallel for schedule(dynamic, 1) num_threads(threads)
		for(int i = y;i < Y;i++){
			for(int j = x;j < X;j++){
				const Ray ray = camera.get_ray(i,j);
				Intersection_info *intersection_info = get_intersection_of_nearest(ray);
				if(intersection_info == nullptr){
					img[i * WIDTH + j] = FColor(0,0,0);
					continue;
				}
				const Vec3 &normal = intersection_info->intersection_point->normal;
				//std::cerr << normal << std::endl;
				img[i * WIDTH + j] = FColor((normal.x + 1.0) * 0.5,(normal.y + 1.0) * 0.5,(normal.z + 1.0) * 0.5);
				delete intersection_info;
				/*const Ray ray = camera.get_ray(i,j);
				for(int k = 0;k < n;k++){
					img[i * WIDTH + j] += pathtracing(ray,0);
				}
				img[i * WIDTH + j] *= (1.0 / n);*/
			}
		}
		for(int i = y;i < Y;i++){
			for(int j = x;j < X;j++){
				R r = img[i * WIDTH + j].red;
	        	R g = img[i * WIDTH + j].green;
	        	R b = img[i * WIDTH + j].blue;
	        	r = std::max((R)0.0,r);r = std::min((R)1.0,r);
	        	g = std::max((R)0.0,g);g = std::min((R)1.0,g);
	        	b = std::max((R)0.0,b);b = std::min((R)1.0,b);
	        	int ir = r * 255 + 0.5;
	        	int ig = g * 255 + 0.5;
	        	int ib = b * 255 + 0.5;

	        	printf("%d %d %d\n",ir,ig,ib);
	
			}
		}
	}
	~Scene(){
		for(Shape *shape : shapes)
			delete shape;
		for(Light_source *light : lights)
			delete light;
		delete light_P;
		delete[] img;
	}
};
