build:
	clang++ -std=c++17 -O3 -fopenmp -march=core-avx2 -lglut -lGLU -lGL -lm -o /tmp/traverse_bench traverse_bench.cpp  -ltcmalloc -lprofiler -g3 -DNDEBUG -fno-inline -ltbb
fast_clang:
	clang++ -std=c++17 -O3 -fopenmp -march=core-avx2 -lglut -lGLU -lGL -lm -o /tmp/traverse_bench traverse_bench.cpp -ltcmalloc -lprofiler -g3 -DNDEBUG -ltbb
pgo:
	g++ -std=c++17 -O3 -fopenmp -march=core-avx2 -lglut -lGLU -lGL -lm -o /tmp/traverse_bench traverse_bench.cpp  -ltcmalloc -lprofiler -g3 -DNDEBUG -fprofile-generate
	/tmp/traverse_bench obj/buddha.obj
	g++ -std=c++17 -O3 -fopenmp -march=core-avx2 -lglut -lGLU -lGL -lm -o /tmp/traverse_bench traverse_bench.cpp  -ltcmalloc -lprofiler -g3 -DNDEBUG -fprofile-use
fast_clang_ibl:
	clang++ -std=c++17 -O3 -fopenmp -march=core-avx2 -lglut -lGLU -lGL -lm -o /tmp/ibl_test ibl_test.cpp -ltcmalloc 
fast_clang_mtl:
	clang++ -std=c++17 -O3 -fopenmp -march=core-avx2 -lglut -lGLU -lGL -lm -o /tmp/mtl_test mtl_test.cpp -ltcmalloc
fast_clang_texture:
	clang++ -std=c++17 -O3 -fopenmp -march=core-avx2 -lglut -lGLU -lGL -lm -o /tmp/texture_test texture_test.cpp  -ltcmalloc -ltbb
Obj:
	clang++ -std=c++17 -O3 -fopenmp -march=core-avx2 -lglut -lGLU -lGL -lm -o /tmp/obj_test obj.cpp  -ltcmalloc 
geom_test:
	clang++ -std=c++17 -O3 -fopenmp -march=core-avx2 -lglut -lGLU -lGL -lm -o /tmp/geom_test ./geom_test.cpp -ltbb -ltcmalloc
geom_bench:
	clang++ -std=c++17 -O3 -fopenmp -march=core-avx2 -lglut -lGLU -lGL -lm -o /tmp/geom_bench ./geom_bench.cpp -ltbb -ltcmalloc -lprofiler
like_edupt:
	clang++ -std=c++17 -O3 -fopenmp -march=core-avx2 -lglut -lGLU -lGL -lm -o /tmp/sample ./sample.cpp -ltbb -ltcmalloc
toml_test:
	clang++ -std=c++17 -O3 -fopenmp -march=core-avx2 -lglut -lGLU -lGL -lm -o /tmp/toml_test toml_test.cpp -ltbb -ltcmalloc -g
load_toml:
	clang++ -std=c++17 -O3 -fopenmp -march=core-avx2 -lglut -lGLU -lGL -lm -o /tmp/load_toml load_toml.cpp -ltbb -ltcmalloc 
