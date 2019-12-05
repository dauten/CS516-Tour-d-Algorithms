all:
	nvcc test.cu -o test -w
	nvcc thrust.cu -o thrust -w
	nvcc singlethread.cu -o singlethread -w
	nvcc -arch=sm_50 multithread.cu -o multithread -w
