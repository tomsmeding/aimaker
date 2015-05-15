CXXFLAGS=-Wall -O2 -std=c++11
CXX=g++

CXX_FILES=*.cpp **/*.cpp
H_FILES=*.h **/*.h

.PHONY: all

all: main

main: $(CXX_FILES) $(H_FILES)
	$(CXX) $(CXXFLAGS) -o main *.cpp **/*.cpp
