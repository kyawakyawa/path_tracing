build:
	clang++ -std=c++17 -O3 -fopenmp -march=core-avx2  -o traverse_bench traverse_bench.cpp
