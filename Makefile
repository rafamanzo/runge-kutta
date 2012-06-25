.PHONY: clean clean_compiling_results clean_others clean_plot clean_examples examples cuda c
CUDA_FLAGS=-arch sm_20
C_FLAGS=-Wall -pedantic

#default C++ version
c: main.o rk_c.o input.o dataset.o fiber.o rk_kernel_c.o output.o
	g++ $(C_FLAGS) main.o input.o rk_c.o rk_kernel_c.o dataset.o fiber.o output.o -o rk

#general objects
	
main.o: main.cpp input.h output.h rk.h dataset.h fiber.h
	g++ -c $(C_FLAGS) main.cpp
	
input.o: input.cpp input.h dataset.h
	g++ -c $(C_FLAGS) input.cpp
	
output.o: output.cpp output.h dataset.h fiber.h
	g++ -c $(C_FLAGS) output.cpp

dataset.o: dataset.cpp dataset.h
	g++ -c $(C_FLAGS) dataset.cpp

fiber.o: fiber.cpp fiber.h dataset.h
	g++ -c $(C_FLAGS) fiber.cpp

#C++
rk_c.o: c/rk.cpp rk.h dataset.h fiber.h c/rk_kernel.h
	g++ -c $(C_FLAGS) c/rk.cpp -o rk_c.o
	
rk_kernel_c.o: c/rk_kernel.cpp c/rk_kernel.h dataset.h fiber.h
	g++ -c  $(C_FLAGS) c/rk_kernel.cpp -o rk_kernel_c.o
	
#CUDA
cuda: main.o input.o dataset.o rk_cuda.o rk_cuda_kernel.o fiber.o output.o
	nvcc main.o input.o rk_cuda_kernel.o rk_cuda.o dataset.o fiber.o output.o -o rk $(CUDA_FLAGS)
	
rk_cuda_kernel.o: cuda/rk_kernel.cu cuda/rk_kernel.h dataset.h fiber.h
	nvcc -c cuda/rk_kernel.cu -o rk_cuda_kernel.o $(CUDA_FLAGS)

rk_cuda.o: cuda/rk.cpp cuda/rk_kernel.h rk.h dataset.h fiber.h
	nvcc -c cuda/rk.cpp -o rk_cuda.o $(CUDA_FLAGS)

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
