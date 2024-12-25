#!/bin/bash
rm buildMaps
g++ -Wall -lstdc++ map_builder/solver.cpp -o buildMaps
./buildMaps

