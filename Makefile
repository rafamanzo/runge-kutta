#default C++ version
c: main.o rk_c.o input.o
	g++ main.o input.o rk_c.o -Wall -ansi -o rk

#general objects
main.o: main.cpp input.h rk.h first_order_function.h
	g++ main.cpp -Wall -ansi -c
	
input.o: input.cpp input.h rk.h first_order_function.h
	g++ -c -Wall -ansi input.cpp

#C++
rk_c.o: c/rk.cpp rk.h first_order_function.h
	g++ -c -Wall -ansi c/rk.cpp -o rk_c.o

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
