.PHONY: clean clean_compiling_results clean_others clean_plot clean_examples examples cuda c
CUDA_FLAGS=-arch sm_20
C_FLAGS=-Wall -pedantic

#default C++ version
c: main.o rk_c.o input.o dataset.o fiber.o rk_kernel_c.o output.o
	g++ $(C_FLAGS) main.o input.o rk_c.o rk_kernel_c.o dataset.o fiber.o output.o -o rk

#general objects
	
main.o: main.cpp io/input.h io/output.h core/rk.h core/dataset.h core/fiber.h
	g++ -c $(C_FLAGS) main.cpp
	
input.o: io/input.cpp io/input.h core/dataset.h
	g++ -c $(C_FLAGS) io/input.cpp
	
output.o: io/output.cpp io/output.h core/dataset.h core/fiber.h
	g++ -c $(C_FLAGS) io/output.cpp

dataset.o: core/dataset.cpp core/dataset.h
	g++ -c $(C_FLAGS) core/dataset.cpp

fiber.o: core/fiber.cpp core/fiber.h core/dataset.h
	g++ -c $(C_FLAGS) core/fiber.cpp

#C++
rk_c.o: core/c/rk.cpp core/rk.h core/dataset.h core/fiber.h core/c/rk_kernel.h
	g++ -c $(C_FLAGS) core/c/rk.cpp -o rk_c.o
	
rk_kernel_c.o: core/c/rk_kernel.cpp core/c/rk_kernel.h core/dataset.h core/fiber.h
	g++ -c  $(C_FLAGS) core/c/rk_kernel.cpp -o rk_kernel_c.o
	
#CUDA
cuda: main.o input.o dataset.o rk_cuda.o rk_cuda_kernel.o fiber.o output.o
	nvcc main.o input.o rk_cuda_kernel.o rk_cuda.o dataset.o fiber.o output.o -o rk $(CUDA_FLAGS)
	
rk_cuda_kernel.o: core/cuda/rk_kernel.cu core/cuda/rk_kernel.h core/dataset.h core/fiber.h
	nvcc -c core/cuda/rk_kernel.cu -o rk_cuda_kernel.o $(CUDA_FLAGS)

rk_cuda.o: core/cuda/rk.cpp core/cuda/rk_kernel.h core/rk.h core/dataset.h core/fiber.h
	nvcc -c core/cuda/rk.cpp -o rk_cuda.o $(CUDA_FLAGS)

#OTHER
examples:
	php example-factories/rotation.php
	php example-factories/random.php

clean_compiling_results:
	rm -f *.o rk
	
clean_plot:
	rm -f rk2.dat rk4.dat rk2-vs-rk4.p
	
clean_others:
	rm -f *~ c/*~ cuda/*~ opencl/*~
	
clean_examples:
	rm -f rotationField randomField
  
clean: clean_compiling_results clean_others clean_plot
