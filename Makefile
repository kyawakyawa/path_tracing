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

