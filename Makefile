.PHONY: clean clean_compiling_results clean_others clean_plot clean_examples examples cuda c gtest

#vars
CUDA_FLAGS=-arch sm_20
C_FLAGS=-Wall -pedantic
C_EXTRA_FLAGS=-Wextra
C_ALL_FLAGS=$(C_FLAGS) $(C_EXTRA_FLAGS)
STATIC_LIBS=-lglut -lGL -lGLU -lm -lpthread -lX11

GENERAL_OBJECTS=main.o input.o dataset.o fiber.o output.o cylinder.o window_manager.o scene.o cylinder_collection.o cone.o cone_collection.o

LIBRARIES_PATH=include
GTEST_PATH=gtest-1.6.0

#default C++ version
c: $(GENERAL_OBJECTS) rk_c.o rk_kernel_c.o
	g++ $(C_ALL_FLAGS) -I$(LIBRARIES_PATH) $(GENERAL_OBJECTS) rk_c.o rk_kernel_c.o -o rk $(STATIC_LIBS)

#general objects

main.o: main.cpp include/input.h include/output.h include/rk.h include/dataset.h include/fiber.h
	g++ -c $(C_ALL_FLAGS) -I$(LIBRARIES_PATH) main.cpp

input.o: io/input.cpp include/input.h include/dataset.h
	g++ -c $(C_ALL_FLAGS) -I$(LIBRARIES_PATH) io/input.cpp

output.o: io/output.cpp include/output.h include/dataset.h include/fiber.h include/scene.h include/window_manager.h include/cylinder.h include/cylinder_collection.h
	g++ -c $(C_ALL_FLAGS) -I$(LIBRARIES_PATH) io/output.cpp

scene.o: io/gui/scene.cpp include/scene.h include/cylinder_collection.h include/cylinder.h include/dataset.h include/fiber.h
	g++ -c $(C_ALL_FLAGS) -I$(LIBRARIES_PATH) io/gui/scene.cpp

window_manager.o: io/gui/window_manager.cpp include/window_manager.h include/dataset.h include/fiber.h include/cylinder.h include/cylinder_collection.h
	g++ -c $(C_FLAGS) -I$(LIBRARIES_PATH) io/gui/window_manager.cpp

cylinder.o: io/gui/primitives/cylinder.cpp include/cylinder.h
	g++ -c $(C_ALL_FLAGS) -I$(LIBRARIES_PATH) io/gui/primitives/cylinder.cpp

cylinder_collection.o: io/gui/primitives/cylinder_collection.cpp include/cylinder_collection.h include/dataset.h include/cylinder.h
	g++ -c $(C_ALL_FLAGS) -I$(LIBRARIES_PATH) io/gui/primitives/cylinder_collection.cpp

cone.o: io/gui/primitives/cone.cpp include/cone.h
	g++ -c $(C_ALL_FLAGS) -I$(LIBRARIES_PATH) io/gui/primitives/cone.cpp

cone_collection.o: io/gui/primitives/cone_collection.cpp include/cone_collection.h include/dataset.h include/cone.h
	g++ -c $(C_ALL_FLAGS) -I$(LIBRARIES_PATH) io/gui/primitives/cone_collection.cpp

dataset.o: core/dataset.cpp include/dataset.h
	g++ -c $(C_ALL_FLAGS) -I$(LIBRARIES_PATH) core/dataset.cpp

fiber.o: core/fiber.cpp include/fiber.h include/dataset.h
	g++ -c $(C_ALL_FLAGS) -I$(LIBRARIES_PATH) core/fiber.cpp

#C++
rk_c.o: core/c/rk.cpp include/rk.h include/dataset.h include/fiber.h include/rk_c_kernel.h
	g++ -c $(C_ALL_FLAGS) -I$(LIBRARIES_PATH) core/c/rk.cpp -o rk_c.o

rk_kernel_c.o: core/c/rk_kernel.cpp include/rk_c_kernel.h include/dataset.h include/fiber.h
	g++ -c  $(C_ALL_FLAGS) -I$(LIBRARIES_PATH) core/c/rk_kernel.cpp -o rk_kernel_c.o

#CUDA
cuda: $(GENERAL_OBJECTS) rk_cuda.o rk_cuda_kernel.o
	nvcc $(GENERAL_OBJECTS) rk_cuda.o rk_cuda_kernel.o -o rk $(CUDA_FLAGS) $(STATIC_LIBS)

rk_cuda_kernel.o: core/cuda/rk_kernel.cu include/rk_cuda_kernel.h include/dataset.h include/fiber.h
	nvcc -c -I$(LIBRARIES_PATH) core/cuda/rk_kernel.cu -o rk_cuda_kernel.o $(CUDA_FLAGS)

rk_cuda.o: core/cuda/rk.cpp include/rk_cuda_kernel.h include/rk.h include/dataset.h include/fiber.h
	nvcc -c -I$(LIBRARIES_PATH) core/cuda/rk.cpp -o rk_cuda.o $(CUDA_FLAGS)

#TESTS
gtest:
	wget http://googletest.googlecode.com/files/gtest-1.6.0.zip
	unzip gtest-1.6.0.zip
	g++ -I$(GTEST_PATH)/include -I$(GTEST_PATH) -c $(GTEST_PATH)/src/gtest-all.cc
	ar -rv libgtest.a gtest-all.o

tests_runner: tests_main.o rk_kernel_fixture.o rk_kernel_tests.o dataset.o fiber.o rk_kernel_c.o
	g++ -I$(GTEST_PATH)/include -Itests/include -I$(LIBRARIES_PATH) tests_main.o rk_kernel_fixture.o rk_kernel_tests.o rk_kernel_c.o dataset.o fiber.o libgtest.a -lpthread -o tests_runner

tests_main.o: tests/main.cpp
	g++ -I$(GTEST_PATH)/include -Itests/include -I$(LIBRARIES_PATH) -c tests/main.cpp -o tests_main.o

rk_kernel_fixture.o: include/dataset.h tests/fixtures/rk_kernel_fixture.cpp tests/include/rk_kernel_fixture.h
	g++ -I$(GTEST_PATH)/include -Itests/include -I$(LIBRARIES_PATH) -c tests/fixtures/rk_kernel_fixture.cpp

rk_kernel_tests.o: tests/units/rk_kernel_tests.cpp tests/fixtures/rk_kernel_fixture.cpp include/dataset.h include/fiber.h include/rk_c_kernel.h
	g++ -I$(GTEST_PATH)/include -Itests/include -I$(LIBRARIES_PATH) -c tests/units/rk_kernel_tests.cpp

#OpenCL
opencl: main.o input.o vector_field.o output.o rk_opencl.o ocl.o
	gcc main.o input.o rk_opencl.o vector_field.o output.o ocl.o -o rk -lm
	
rk_opencl.o: ocl.o opencl/rk.cpp opencl/ocl.h rk.h vector_field.h
	gcc -c opencl/rk.cpp opencl/rk.cl -o rk_opencl.o -I${OPENCL_INCLUDE}

ocl.o: opencl/ocl.cpp opencl/ocl.h vector_field.h
	gcc -c opencl/ocl.cpp -o ocl.o -I${OPENCL_INCLUDE}

#OTHER
examples:
	php example-factories/rotation.php
	php example-factories/random.php
	php example-factories/lines.php
	php example-factories/gaussian.php

clean_compiling_results:
	rm -f *.o rk

clean_plot:
	rm -f rk2.dat rk4.dat rk2-vs-rk4.p

clean_others:
	rm -f *~ example-factories/*~ include/*~ core/*~ core/c/*~ core/cuda/*~ core/opencl/*~ io/*~ io/gui/*~ io/gui/primitives/*~

clean_examples:
	rm -f rotationField randomField gaussianField linesField

clean_tests:
	rm -f libgtest.a gtest-1.6.0.zip
	rm -rf gtest-1.6.0

clean: clean_compiling_results clean_others clean_plot
