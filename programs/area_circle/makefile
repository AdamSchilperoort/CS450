

all:
	nvcc -arch=compute_60 -code=sm_60 -lcuda  -Xcompiler -fopenmp  area_circle.cu -o area_circle
	
psum:
	nvcc -arch=compute_60 -code=sm_60 -lcuda  -Xcompiler -fopenmp  area_circle_psum.cu -o area_circle_psum


