.PHONY: clean clean_compiling_results clean_others clean_plot clean_examples examples cuda c

#vars
CUDA_FLAGS=-arch sm_20
C_FLAGS=-Wall -pedantic -g
STATIC_LIBS=-lglut -lGL -lGLU -lm

GENERAL_OBJECTS=main.o input.o dataset.o fiber.o output.o cylinder.o window_manager.o scene.o cylinder_collection.o

#default C++ version
c: $(GENERAL_OBJECTS) rk_c.o rk_kernel_c.o
	g++ $(C_FLAGS) $(GENERAL_OBJECTS) rk_c.o rk_kernel_c.o -o rk $(STATIC_LIBS)

#general objects
	
main.o: main.cpp io/input.h io/output.h core/rk.h core/dataset.h core/fiber.h
	g++ -c $(C_FLAGS) main.cpp
	
input.o: io/input.cpp io/input.h core/dataset.h
	g++ -c $(C_FLAGS) io/input.cpp
	
output.o: io/output.cpp io/output.h core/dataset.h core/fiber.h io/gui/scene.h io/gui/window_manager.h
	g++ -c $(C_FLAGS) io/output.cpp

dataset.o: core/dataset.cpp core/dataset.h
	g++ -c $(C_FLAGS) core/dataset.cpp

fiber.o: core/fiber.cpp core/fiber.h core/dataset.h
	g++ -c $(C_FLAGS) core/fiber.cpp
	
cylinder.o: io/gui/primitives/cylinder.cpp io/gui/primitives/cylinder.h
	g++ -c $(C_FLAGS) io/gui/primitives/cylinder.cpp
	
window_manager.o: io/gui/window_manager.cpp io/gui/window_manager.h core/dataset.h core/fiber.h io/gui/primitives/cylinder.h
	g++ -c $(C_FLAGS) io/gui/window_manager.cpp
	
cylinder_collection.o: io/gui/primitives/cylinder_collection.cpp io/gui/primitives/cylinder_collection.h core/dataset.h io/gui/primitives/cylinder.h
	g++ -c $(C_FLAGS) io/gui/primitives/cylinder_collection.cpp

scene.o: io/gui/scene.cpp io/gui/scene.h io/gui/primitives/cylinder_collection.h io/gui/primitives/cylinder.h core/dataset.h core/fiber.h
	g++ -c $(C_FLAGS) io/gui/scene.cpp

#C++
rk_c.o: core/c/rk.cpp core/rk.h core/dataset.h core/fiber.h core/c/rk_kernel.h
	g++ -c $(C_FLAGS) core/c/rk.cpp -o rk_c.o
	
rk_kernel_c.o: core/c/rk_kernel.cpp core/c/rk_kernel.h core/dataset.h core/fiber.h
	g++ -c  $(C_FLAGS) core/c/rk_kernel.cpp -o rk_kernel_c.o
	
#CUDA
cuda: $(GENERAL_OBJECTS) rk_cuda.o rk_cuda_kernel.o
	nvcc $(GENERAL_OBJECTS) rk_cuda.o rk_cuda_kernel.o -o rk $(CUDA_FLAGS) $(STATIC_LIBS)
	
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
