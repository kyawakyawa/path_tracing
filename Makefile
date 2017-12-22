build:
	clang++ -std=c++17 -O3 -fopenmp -march=core-avx2 -lglut -lGLU -lGL -lm -o /tmp/traverse_bench traverse_bench.cpp  -ltcmalloc -lprofiler -g3 -DNDEBUG -fno-inline
fast_gcc:
	g++ -std=c++17 -Ofast -fopenmp -march=core-avx2 -lglut -lGLU -lGL -lm -o /tmp/traverse_bench traverse_bench.cpp  -ltcmalloc -lprofiler -g3 -DNDEBUG
fast_clang:
	clang++ -std=c++17 -Ofast -fopenmp -march=core-avx2 -lglut -lGLU -lGL -lm -o /tmp/traverse_bench traverse_bench.cpp  -ltcmalloc -lprofiler -g3 -DNDEBUG
S:
	clang++ -std=c++17 -Ofast -fopenmp -march=core-avx2 -lglut -lGLU -lGL -lm -o /tmp/traverse.s traverse_bench.cpp  -ltcmalloc -lprofiler -g3 -DNDEBUG -S
pgo:
	g++ -std=c++17 -O3 -fopenmp -march=core-avx2 -lglut -lGLU -lGL -lm -o /tmp/traverse_bench traverse_bench.cpp  -ltcmalloc -lprofiler -g3 -DNDEBUG -fprofile-generate
	/tmp/traverse_bench obj/buddha.obj
	g++ -std=c++17 -O3 -fopenmp -march=core-avx2 -lglut -lGLU -lGL -lm -o /tmp/traverse_bench traverse_bench.cpp  -ltcmalloc -lprofiler -g3 -DNDEBUG -fprofile-use
fast_clang_ibl:
	clang++ -std=c++17 -Ofast -fopenmp -march=core-avx2 -lglut -lGLU -lGL -lm -o /tmp/ibl_test ibl_test.cpp -ltcmalloc -lprofiler -g3 -DNDEBUG
fast_clang_mtl:
	clang++ -std=c++17 -Ofast -fopenmp -march=core-avx2 -lglut -lGLU -lGL -lm -o /tmp/mtl_test mtl_test.cpp -ltcmalloc -lprofiler -g3 -DNDEBUG
Obj:
	clang++ -std=c++17 -O3 -fopenmp -march=core-avx2 -lglut -lGLU -lGL -lm -o /tmp/obj_test obj.cpp  -ltcmalloc 
