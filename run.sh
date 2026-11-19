#!/usr/bin/env bash

clear

rm *.o propagator

echo "Compiling propagator.cpp"
g++ -c -I /opt/cuda/include/ propagator.cpp -o propagator.o

echo "Compiling kernels.cu"
nvcc -arch=sm_120 --compile kernels.cu -o kernelscu.o

echo "Compiling propagator.cu"
nvcc -arch=sm_120 --compile  propagator.cu -o propagatorcu.o

echo "Compiling io.cpp"
g++ -c io.cpp -o io.o

echo "Compiling helpers.cpp"
g++ -c -I /opt/cuda/include/ helpers.cpp -o helpers.o

echo "Compiling and linking propagator"
g++ propagator.o propagatorcu.o kernelscu.o io.o helpers.o -L /opt/cuda/lib/ -lcudart -lm -o propagator

time ./propagator
