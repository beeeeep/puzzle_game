#!/bin/bash
rm buildMaps
g++ -Wall -lstdc++ -g map_builder/solver.cpp -I../src -o buildMaps
gdb ./buildMaps
