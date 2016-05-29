#! /bin/bash

mkdir -p includes || exit
clang++ -std=c++14 src/generate.cpp -o gen || exit
./gen > includes/variant.h || exit
clang++ -std=c++14 -I includes test/test.cpp -o variant
