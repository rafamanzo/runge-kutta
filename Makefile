.PHONY: clean clean_compiling_results clean_others clean_plot clean_examples examples cuda c
COMPUTE_CAPABILITY=-arch sm_20

#default C++ version
c: main.o rk_c.o input.o vector_operations_c.o vector_field.o output.o
	g++ main.o input.o rk_c.o vector_operations_c.o vector_field.o output.o -o rk

#general objects
	
main.o: main.cpp input.h output.h rk.h vector_field.h
	g++ main.cpp -Wall -ansi -pedantic -c
	
input.o: input.cpp input.h vector_field.h
	g++ -c -Wall -ansi -pedantic input.cpp
	
output.o: output.cpp output.h vector_field.h
	g++ -c -Wall -ansi -pedantic output.cpp
	
vector_field.o: vector_field.cpp vector_field.h
	g++ -c -Wall -ansi -pedantic vector_field.cpp -o vector_field.o

#C++
rk_c.o: c/rk.cpp rk.h vector_field.h c/vector_operations.h
	g++ -c -Wall -ansi -pedantic c/rk.cpp -o rk_c.o
	
vector_operations_c.o: c/vector_operations.cpp c/vector_operations.h vector_field.h
	g++ -c -Wall -ansi -pedantic c/vector_operations.cpp -o vector_operations_c.o

#CUDA
cuda: main.o input.o vector_field.o output.o rk_cuda.o rk_cuda_kernel.o
	nvcc main.o input.o rk_cuda_kernel.o rk_cuda.o vector_field.o output.o -o rk ${COMPUTE_CAPABILITY}
	
rk_cuda_kernel.o: cuda/rk.cu cuda/rk_kernel.h vector_field.h
	nvcc -c cuda/rk.cu -o rk_cuda_kernel.o ${COMPUTE_CAPABILITY}

rk_cuda.o: cuda/rk.cpp cuda/rk_kernel.h rk.h vector_field.h
	nvcc -c cuda/rk.cpp -o rk_cuda.o ${COMPUTE_CAPABILITY}

#OpenCL
opencl: main.o input.o vector_field.o output.o rk_opencl.o ocl.o
	gcc main.o input.o rk_opencl.o vector_field.o output.o ocl.o -o rk -lm
	
rk_opencl.o: opencl/rk.cpp opencl/ocl.h rk.h vector_field.h
	gcc -c opencl/rk.cpp -o rk_opencl.o

ocl.o: opencl/ocl.cpp opencl/ocl.h vector_field.h
	gcc -c opencl/rk.cpp -o ocl.o 

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
  
clean: clean_compiling_results clean_others clean_plot clean_examples
