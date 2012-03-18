#default C++ version
c: main3d.o rk_c.o input3d.o vector_operations_c.o vector_field.o
	g++ main3d.o input3d.o rk_c.o vector_operations_c.o vector_field.o -Wall -ansi -o rk

#general objects
main.o: main.cpp input.h rk.h first_order_function.h
	g++ main.cpp -Wall -ansi -c
	
main3d.o: main3d.cpp input3d.h rk3d.h vector_field.h
	g++ main3d.cpp -Wall -ansi -pedantic -c
	
input.o: input.cpp input.h rk.h first_order_function.h
	g++ -c -Wall -ansi input.cpp
	
input3d.o: input3d.cpp input3d.h rk3d.h vector_field.h
	g++ -c -Wall -ansi -pedantic input3d.cpp
	
vector_field.o:
	g++ -c -Wall -ansi -pedantic vector_field.cpp

#C++
rk_c.o: c/rk.cpp rk3d.h vector_field.h c/vector_operations.h
	g++ -c -Wall -ansi -pedantic c/rk.cpp -o rk_c.o
	
vector_operations_c.o:
	g++ -c -Wall -ansi -pedantic c/vector_operations.cpp -o vector_operations_c.o

#CUDA
cuda: main.o rk_cuda.o rk_cuda_kernel.o input.o
	nvcc main.o input.o rk_cuda_kernel.o rk_cuda.o -o rk -arch compute_20
	
rk_cuda_kernel.o: cuda/rk.cu cuda/rk_kernel.h first_order_function.h
	nvcc -c cuda/rk.cu -o rk_cuda_kernel.o -arch compute_20

rk_cuda.o: cuda/rk.cpp cuda/rk_kernel.h rk.h first_order_function.h
	nvcc -c cuda/rk.cpp -o rk_cuda.o -arch compute_20

#OTHER
clean:
	rm -f *~ *.o rk
