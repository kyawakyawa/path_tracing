#pragma once

#include<climits>
#include <vector>
#include "Shape.h"
#include "Intersection_info.h"
#include "Camera.h"
#include <GL/glut.h>

unsigned long long  xor128(void) {
    // x, y, z, w が乱数のシード
    static unsigned long long x=123456789,y=362436069,z=521288629,w=88675123;
    unsigned long long t;
    t=(x^(x<<11));x=y;y=z;z=w; return( w=(w^(w>>19))^(t^(t>>8)) );
}

R rando(){
	unsigned long long n = xor128();

	R M = (R)ULLONG_MAX;
	M += 1.0;
	return (R)n / M;
}

struct Scene{
	std::vector<Shape*> shapes;//物体
	const int HEIGHT = 480;//縦のピクセル
	const int WIDTH = 480;//横のピクセル
	const R INF = 1000000000.0;
	const FColor back;//物体がないときの色
	const int MAX_DEPTH = 3;//再帰の深さの最大値
	FColor *img;//ピクセルごとの色を保持

	inline Scene(): back(FColor(100.0 / 255,149.0 / 255,237.0 / 255)),img(new FColor[HEIGHT * WIDTH]){};

	inline void add(Shape *shape){//物体を追加する
		shapes.push_back(shape);
	}

	inline void init_img(){
		#pragma omp parallel for schedule(dynamic, 1) num_threads(4)
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
	inline bool is_shadow(const Ray &ray,const R max_t) const {//光源までの間に物体がないか調べる
		for(Shape *shape : shapes){
			Intersection_point *intersection = shape->get_intersection(ray);
			if(intersection != nullptr && max_t > intersection->distance){
				delete intersection;
				return true;
			}
			delete intersection;
		}
		return false;
	}

	inline FColor pathtracing(const Ray &ray,const int depth) const{
		const Intersection_info *intersection_info = get_intersection_of_nearest(ray);
			
		if(intersection_info == nullptr){//物体が存在しない
			return back;
		}
		
		const Intersection_point *intersection = intersection_info->intersection_point;
		const Shape *intersection_shape = intersection_info->shape;
		const Material material = intersection_shape->get_material(intersection->position);
		const Vec3 normal = ((ray.direction * intersection->normal < 0.0) ? 1.0 : -1.0) * intersection->normal;
		
		FColor L = material.Le;

		R P = std::max(std::max(material.kd.red,material.kd.green),material.kd.blue);

		if(depth < MAX_DEPTH)
			P = 1.0;
		
		if (depth > 32)
			P *= pow(0.5, depth - 32);
		
		

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
		}

		delete intersection_info;
		return L;
	}

	void compute(int n,int N){//サンプル回数とすでに何回サンプリングしているか
		if(N == 0)
			init_img();
		Camera camera(WIDTH,HEIGHT);
		#pragma omp parallel for schedule(dynamic, 1) num_threads(4)
		for(int i = 0;i < HEIGHT;i++){
			for(int j = 0;j < WIDTH;j++){
				//const Ray ray(Vec3(0,0,-5),Vec3(2.0 * j / (WIDTH - 1) - 1,-2.0 * i / (HEIGHT - 1) + 1,5.0));
				const Ray ray = camera.get_ray(i,j);
				img[i * WIDTH + j] *= N;
				for(int k = 0;k < n;k++){
					img[i * WIDTH + j] += pathtracing(ray,0);
				}
				img[i * WIDTH + j] *= (1.0 / (n + N));
			}
		}
	}

	void draw() {

		printf("P3\n%d %d\n255\n", WIDTH,HEIGHT);

		compute(8192,0);

		for(int i = 0;i < HEIGHT * WIDTH;i++)
			img[i].print255();
	}

	~Scene(){
		for(Shape *shape : shapes)
			delete shape;
		delete[] img;
	}
};
