#!/bin/bash
rm buildMaps
g++ -Wall -lstdc++ -O3 map_builder/solver.cpp -I../src -o buildMaps
./buildMaps

