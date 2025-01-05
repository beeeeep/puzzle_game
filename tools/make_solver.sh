#!/bin/bash
rm buildMaps
g++ -Wall -lstdc++ -O3 map_builder/solver.cpp -o buildMaps
./buildMaps
mv *_final.in ../data/
