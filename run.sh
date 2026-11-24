#!/usr/bin/env bash

clear

rm build/*.o build/test_propagator

echo "Compiling propagator.cpp"
g++ -c -I /opt/cuda/include/ -I include/ -I src/ src/propagator.cpp -o build/propagator.o

echo "Compiling kernels.cu"
nvcc -arch=sm_120 --compile -I include/ -I src/ src/kernels.cu -o build/kernelscu.o

echo "Compiling propagator.cu"
nvcc -arch=sm_120 --compile -I include/ -I src/ src/propagator.cu -o build/propagatorcu.o

echo "Compiling io.cpp"
g++ -c -I include/ -I src/ src/io.cpp -o build/io.o

echo "Compiling helpers.cpp"
g++ -c -I /opt/cuda/include/ -I include -I src/ src/helpers.cpp -o build/helpers.o

echo "Compiling test_propagator.cpp"
g++ -c -I include/ -I src/ tests/test_propagator.cpp -o build/test_propagator.o

echo "Compiling and linking test_propagator"
g++ build/test_propagator.o build/propagator.o build/propagatorcu.o build/kernelscu.o build/io.o build/helpers.o -L /opt/cuda/lib/ -lcudart -lm -o build/test_propagator

time build/test_propagator ./tests/custom_config.txt
