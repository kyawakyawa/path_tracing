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
	//const int HEIGHT = 480;//縦のピクセル
	//const int WIDTH = 480;//横のピクセル
	//const R INF = 1000000000.0;
	//const FColor back;//物体がないときの色
	//const Infinity_light_source back;
	const int threads = 4; //スレッドの数
	//FColor *img;//ピクセルごとの色を保持
	Camera *camera = nullptr;

	//inline Scene() : Scene(480,480) {} ;
	//inline Scene(int w,int h) :HEIGHT(h),WIDTH(w),img(new FColor[HEIGHT * WIDTH]),camera(WIDTH,HEIGHT),radiance(new Normal_render()){};
	//inline Scene(Camera c,Radiance *rad) : HEIGHT(c.picH),WIDTH(c.picW),img(new FColor[HEIGHT * WIDTH]),camera(c),radiance(rad) {};

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

	/*inline void init_img(){
		#pragma omp parallel for schedule(dynamic, 1) num_threads(threads)
		for(int i = 0;i < HEIGHT * WIDTH;i++)
			img[i] = FColor(0,0,0);
	}*/

	/*void compute(int n,int N,int supersamples){//サンプル回数とすでに何回サンプリングしているか
		if(N == 0){
			init_img();
			radiance->set(&shapes,&lights);
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
							//img[i * WIDTH + j] += pathtracing_direct(ray,0,true);
							//img[i * WIDTH + j] += next_event_estimation(ray);
							img[i * WIDTH + j] += radiance->radiance(ray);
							FColor g = img[i * WIDTH + j];
							if(std::isinf(g.red) || std::isinf(g.blue) || std::isinf(g.green)) {
								std::cerr << "inf!" << std::endl; 
							}
							if(std::isnan(g.red) || std::isnan(g.blue) || std::isnan(g.green)) {
								std::cerr << "nan!" << std::endl; 
							}
						}
					}
				}
				img[i * WIDTH + j] *= r;
			}
		}
	}*/

	void draw(int sample) {

		/*printf("P3\n%d %d\n255\n", WIDTH,HEIGHT);

		compute(sample,0,supersamples);

		for(int i = 0;i < HEIGHT * WIDTH;i++)
			img[i].print255();
			*/

		camera->sensitization(&shapes,&lights,sample,threads);

		camera->out_ppm_stdio();
	}
	/*void debug(int sample,int supersamples) {
		compute(sample,0,supersamples);
	}*/


	~Scene(){
		for(Shape *shape : shapes)
			delete shape;
		for(Light_source *light : lights)
			delete light;
		//delete[] img;
		//delete radiance;
		delete camera;
	}
};
