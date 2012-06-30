.PHONY: clean clean_compiling_results clean_others clean_plot clean_examples examples cuda c

#vars
CUDA_FLAGS=-arch sm_20
C_FLAGS=-Wall -pedantic
STATIC_LIBS=-lglut -lGL -lGLU -lm

GENERAL_OBJECTS=main.o input.o dataset.o fiber.o output.o cylinder.o window_manager.o scene.o cylinder_collection.o

LIBRARIES_PATH=include

#default C++ version
c: $(GENERAL_OBJECTS) rk_c.o rk_kernel_c.o
	g++ $(C_FLAGS) -I$(LIBRARIES_PATH) $(GENERAL_OBJECTS) rk_c.o rk_kernel_c.o -o rk $(STATIC_LIBS)

#general objects
	
main.o: main.cpp include/input.h include/output.h include/rk.h include/dataset.h include/fiber.h
	g++ -c $(C_FLAGS) -I$(LIBRARIES_PATH) main.cpp
	
input.o: io/input.cpp include/input.h include/dataset.h
	g++ -c $(C_FLAGS) -I$(LIBRARIES_PATH) io/input.cpp
	
output.o: io/output.cpp include/output.h include/dataset.h include/fiber.h include/scene.h include/window_manager.h include/cylinder.h include/cylinder_collection.h
	g++ -c $(C_FLAGS) -I$(LIBRARIES_PATH) io/output.cpp

scene.o: io/gui/scene.cpp include/scene.h include/cylinder_collection.h include/cylinder.h include/dataset.h include/fiber.h
	g++ -c $(C_FLAGS) -I$(LIBRARIES_PATH) io/gui/scene.cpp
	
window_manager.o: io/gui/window_manager.cpp include/window_manager.h include/dataset.h include/fiber.h include/cylinder.h include/cylinder_collection.h
	g++ -c $(C_FLAGS) -I$(LIBRARIES_PATH) io/gui/window_manager.cpp
	
cylinder.o: io/gui/primitives/cylinder.cpp include/cylinder.h
	g++ -c $(C_FLAGS) -I$(LIBRARIES_PATH) io/gui/primitives/cylinder.cpp
	
cylinder_collection.o: io/gui/primitives/cylinder_collection.cpp include/cylinder_collection.h include/dataset.h include/cylinder.h
	g++ -c $(C_FLAGS) -I$(LIBRARIES_PATH) io/gui/primitives/cylinder_collection.cpp

dataset.o: core/dataset.cpp include/dataset.h
	g++ -c $(C_FLAGS) -I$(LIBRARIES_PATH) core/dataset.cpp

fiber.o: core/fiber.cpp include/fiber.h include/dataset.h
	g++ -c $(C_FLAGS) -I$(LIBRARIES_PATH) core/fiber.cpp

#C++
rk_c.o: core/c/rk.cpp include/rk.h include/dataset.h include/fiber.h include/rk_c_kernel.h
	g++ -c $(C_FLAGS) -I$(LIBRARIES_PATH) core/c/rk.cpp -o rk_c.o
	
rk_kernel_c.o: core/c/rk_kernel.cpp include/rk_c_kernel.h include/dataset.h include/fiber.h
	g++ -c  $(C_FLAGS) -I$(LIBRARIES_PATH) core/c/rk_kernel.cpp -o rk_kernel_c.o
	
#CUDA
cuda: $(GENERAL_OBJECTS) rk_cuda.o rk_cuda_kernel.o
	nvcc $(GENERAL_OBJECTS) rk_cuda.o rk_cuda_kernel.o -o rk $(CUDA_FLAGS) $(STATIC_LIBS)
	
rk_cuda_kernel.o: core/cuda/rk_kernel.cu include/rk_cuda_kernel.h include/dataset.h include/fiber.h
	nvcc -c -I$(LIBRARIES_PATH) core/cuda/rk_kernel.cu -o rk_cuda_kernel.o $(CUDA_FLAGS)

rk_cuda.o: core/cuda/rk.cpp include/rk_cuda_kernel.h include/rk.h include/dataset.h include/fiber.h
	nvcc -c -I$(LIBRARIES_PATH) core/cuda/rk.cpp -o rk_cuda.o $(CUDA_FLAGS)

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
