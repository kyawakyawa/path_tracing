# root/Makefile

CXX     := clang++
CXXFLAGS := -std=c++17 -O3 -fopenmp -march=core-avx2

ALL_C  := $(wildcard ../src/*/*.c ../src/*/*.cpp)                   
ALL_O_1  := $(patsubst ../src/%.cpp,../obj/%.o,$(ALL_C)) 
ALL_O  := $(notdir $(ALL_O_1)) 
ALL_D_1 := $(patsubst ../src/%.cpp,%.d,$(ALL_C))
ALL_D  := $(notdir $(ALL_D_1))
ALL_H  := $(wildcard ../inc/*/*.hpp ../inc/*/*.h)

.PHONY: dummy
dummy: $(ALL_O)

%.o: ../src/*/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.d: ../src/*/%.cpp $(ALL_H)  # 下の -include 命令から呼ばれる
	$(CXX) -MM $< -MF $@       # A.c の依存関係をMakefile形式で書いた A.d を生成

-include $(ALL_D)
