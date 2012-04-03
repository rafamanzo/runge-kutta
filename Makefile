.PHONY: clean
COMPUTE_CAPABILITY=-arch compute_20

#default C++ version
c: main3d.o rk_c.o input3d.o vector_operations_c.o vector_field_c.o output.o
	g++ main3d.o input3d.o rk_c.o vector_operations_c.o vector_field_c.o output.o -Wall -ansi -o rk

#general objects
main.o: main.cpp input.h rk.h first_order_function.h
	g++ main.cpp -Wall -ansi -c
	
main3d.o: main3d.cpp input3d.h rk3d.h vector_field.h
	g++ main3d.cpp -Wall -ansi -pedantic -c
	
input.o: input.cpp input.h rk.h first_order_function.h
	g++ -c -Wall -ansi input.cpp
	
input3d.o: input3d.cpp input3d.h rk3d.h vector_field.h
	g++ -c -Wall -ansi -pedantic input3d.cpp
	
output.o: output.cpp output.h vector_field.h
	g++ -c -Wall -ansi -pedantic output.cpp

#C++
rk_c.o: c/rk.cpp rk3d.h vector_field.h c/vector_operations.h
	g++ -c -Wall -ansi -pedantic c/rk.cpp -o rk_c.o
	
vector_field_c.o:
	g++ -c -Wall -ansi -pedantic c/vector_field.cpp -o vector_field_c.o
	
vector_operations_c.o:
	g++ -c -Wall -ansi -pedantic c/vector_operations.cpp -o vector_operations_c.o

#CUDA
cuda: main.o rk_cuda.o rk_cuda_kernel.o input.o vector_field_cuda.o
	nvcc main.o input.o rk_cuda_kernel.o rk_cuda.o -o rk ${COMPUTE_CAPABILITY}
	
rk_cuda_kernel.o: cuda/rk.cu cuda/rk_kernel.h first_order_function.h
	nvcc -c cuda/rk.cu -o rk_cuda_kernel.o ${COMPUTE_CAPABILITY}

rk_cuda.o: cuda/rk.cpp cuda/rk_kernel.h rk.h first_order_function.h
	nvcc -c cuda/rk.cpp -o rk_cuda.o ${COMPUTE_CAPABILITY}
	
vector_field_cuda.o: cuda/vector_field.cu vector_field.h
	nvcc -c cuda/vector_field.cu -o vector_field_cuda.o ${COMPUTE_CAPABILITY}
	
vector_operations_cuda.o: cuda/vector_operations.cu cuda/vector_operations.h vector_field.h 
	nvcc -c cuda/vector_operations.cu -o vector_operations_cuda.o ${COMPUTE_CAPABILITY}
#OTHER
clean:
	rm -f *~ *.o rk
