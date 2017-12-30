build:
	clang++ -std=c++17 -O3 -fopenmp -march=core-avx2 -lglut -lGLU -lGL -lm -o /tmp/traverse_bench traverse_bench.cpp  -ltcmalloc -lprofiler -g3 -DNDEBUG -fno-inline
fast_clang:
	clang++ -std=c++17 -Ofast -fopenmp -march=core-avx2 -lglut -lGLU -lGL -lm -o /tmp/traverse_bench traverse_bench.cpp  -ltcmalloc -lprofiler -g3 -DNDEBUG
pgo:
	g++ -std=c++17 -O3 -fopenmp -march=core-avx2 -lglut -lGLU -lGL -lm -o /tmp/traverse_bench traverse_bench.cpp  -ltcmalloc -lprofiler -g3 -DNDEBUG -fprofile-generate
	/tmp/traverse_bench obj/buddha.obj
	g++ -std=c++17 -O3 -fopenmp -march=core-avx2 -lglut -lGLU -lGL -lm -o /tmp/traverse_bench traverse_bench.cpp  -ltcmalloc -lprofiler -g3 -DNDEBUG -fprofile-use
fast_clang_ibl:
	clang++ -std=c++17 -Ofast -fopenmp -march=core-avx2 -lglut -lGLU -lGL -lm -o /tmp/ibl_test ibl_test.cpp -ltcmalloc 
fast_clang_mtl:
	clang++ -std=c++17 -Ofast -fopenmp -march=core-avx2 -lglut -lGLU -lGL -lm -o /tmp/mtl_test mtl_test.cpp -ltcmalloc
fast_clang_texture:
	clang++ -std=c++17 -Ofast -fopenmp -march=core-avx2 -lglut -lGLU -lGL -lm -o /tmp/texture_test texture_test.cpp  -ltcmalloc
Obj:
	clang++ -std=c++17 -Ofast -fopenmp -march=core-avx2 -lglut -lGLU -lGL -lm -o /tmp/obj_test obj.cpp  -ltcmalloc 
