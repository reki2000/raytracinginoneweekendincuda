CUDA_PATH     ?= /usr/local/cuda
HOST_COMPILER  = clang
NVCC           = $(CUDA_PATH)/bin/nvcc -ccbin $(HOST_COMPILER)
CC             = clang

# select one of these for Debug vs. Release
#NVCC_DBG       = -g -G
NVCC_DBG       =

NVCCFLAGS      = $(NVCC_DBG) -m64 -O3 -Xptxas -O3 -Xcompiler -march=x86-64-v3
GENCODE_FLAGS  = -gencode arch=compute_89,code=sm_89

SRCS = main.cu
INCS = vec3.h ray.h hitable.h hitable_list.h sphere.h camera.h material.h

cudart: cudart.o main.o
	$(NVCC) $(NVCCFLAGS) $(GENCODE_FLAGS) -o cudart cudart.o main.o -lX11

cudart.o: $(SRCS) $(INCS)
	$(NVCC) $(NVCCFLAGS) $(GENCODE_FLAGS) -o cudart.o -c main.cu

main.o: main.c
	$(CC) -std=c2x -o main.o -c main.c

out.ppm: cudart
	rm -f out.ppm
	./cudart > out.ppm

out.jpg: out.ppm
	rm -f out.jpg
	ppmtojpeg out.ppm > out.jpg

x: cudart
	./cudart

profile_basic: cudart
	nvprof ./cudart > out.ppm

# use nvprof --query-metrics
profile_metrics: cudart
	nvprof --metrics achieved_occupancy,inst_executed,inst_fp_32,inst_fp_64,inst_integer ./cudart > out.ppm

clean:
	rm -f cudart cudart.o out.ppm out.jpg main.o
