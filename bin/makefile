#makefile
objs = main.o Load_image.o
CXX = clang++
CXXFLAGS = -std=c++17 -O3 -fopenmp -march=core-avx2

.PHONY: all
all: main

main: $(objs)
	$(CXX) $(CXXFLAGS) -o main $(objs) -lglut -lGLU -lGL -lm -ltbb -ltcmalloc

main.o: ../src/main/load_toml.cpp
	$(CXX) $(CXXFLAGS) -o main.o -c ../src/main/load_toml.cpp

Load_image.o: ../src/Load_image/Load_image.cpp
	$(CXX) $(CXXFLAGS) -c ../src/Load_image/Load_image.cpp
Load_image.o: ../src/Color/FColor.hpp ../src/ext_libs/stb_image.h


.PHONY: clean
clean:
	$(RM) main $(objs)

