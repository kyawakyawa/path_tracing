#参考
#http://auewe.hatenablog.com/entry/2017/02/09/070446
#http://auewe.hatenablog.com/entry/2017/02/08/195625
# root/Makefile

CXX     := clang++
CXXFLAGS := -std=c++17 -O3 -fopenmp -march=core-avx2

ALL_C  := $(wildcard src/*/*.c src/*/*.cpp)                   
ALL_O_1  := $(patsubst src/%.cpp,obj/%.o,$(ALL_C)) 
ALL_O_2  := $(notdir $(ALL_O_1)) 
ALL_O := $(patsubst %,obj/%,$(ALL_O_2))
ALL_CH := $(wildcard src/*/*.c src/*/*cpp inc/*/*.hpp inc/*/*.h)           

exec.out: $(ALL_CH)
	cd obj && $(MAKE) "CXX=$(CXX)" # obj/Makefile を実行する (ALL_Oが作成される)
	$(CXX) $(CXXFLAGS) $(ALL_O) -lglut -lGLU -lGL -lm -ltbb -o $@
test:
	$(ALL_CH)

.PHONY: clean
clean:
	@rm -rf *.out obj/*.o obj/*.d
