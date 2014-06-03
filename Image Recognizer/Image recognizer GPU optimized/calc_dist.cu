#include <float.h>
#include <stdlib.h>
#include <stdio.h>
#include <cuda_runtime.h>
#include <cutil.h>
#include "utils.h"
//PASSES MAKE CHECK

__global__ void flipKernel(float*arr, float *out, int w){
	int x = blockIdx.x*blockDim.x+threadIdx.x;
	int y = blockIdx.y*blockDim.y+threadIdx.y;
	// int x = blockIdx.x*16+threadIdx.x;
	// int y = blockIdx.y*16+threadIdx.y;
	out[y*w+w-x-1]=arr[y*w+x];
}

void flip(float *arr, float *out, int w) {
	dim3 dim_blocks(w/16,w/16);
	dim3 dim_threads(16,16,1);
	flipKernel<<<dim_blocks, dim_threads>>>(arr, out, w);
	cudaThreadSynchronize();
	CUT_CHECK_ERROR("");

}

__global__ void transposeKernel(float*arr, float *out, int w){
	int x = blockIdx.x*blockDim.x+threadIdx.x;
	int y = blockIdx.y*blockDim.y+threadIdx.y;
	//int x = blockIdx.x*16+threadIdx.x;
	//int y = blockIdx.y*16+threadIdx.y;
	//out[y*w+x]=arr[y+x*w];
	out[y+x*w]=arr[y*w+x];
}

void transpose(float *arr, float *out, int w) {
	dim3 dim_blocks(w/16,w/16);
	dim3 dim_threads(16,16,1);
	transposeKernel<<<dim_blocks, dim_threads>>>(arr, out, w);
	cudaThreadSynchronize();
	CUT_CHECK_ERROR("");

}



__global__ void reductionKernel(float* A, int len, int stride) {
	int  index = blockIdx.x*blockDim.x+threadIdx.x;
	if (index+stride<len){
		index+=blockIdx.y*len;
		A[index] += A[index+stride];
	}
}



void reductionGPU(float* A, int len) {

	int threads_per_block = 512; 
	int blocks_per_grid = (len/1024);
	if (blocks_per_grid==0){
		blocks_per_grid=1;
	}
	int stride = len/2;

	while (stride>=1) { 
		dim3 dim_blocks(blocks_per_grid,1);
		dim3 dim_threads(threads_per_block,1,1);
		reductionKernel<<<dim_blocks, dim_threads>>>(A,len,stride);
		cudaThreadSynchronize();
		CUT_CHECK_ERROR("");

		stride /=2;

		if (blocks_per_grid!=1){
			blocks_per_grid /=2;
		}
		else if(threads_per_block != 1){
			threads_per_block /= 2;
		}
	}

}



__global__ void calcdistKernel(float* out, float* img, int i_w, int i_h, float* t, int t_w, int x, int y) {
	int index0 = 	blockIdx.x*16 + threadIdx.x
					+ (blockIdx.y)*t_w
					+ t_w*t_w*threadIdx.z;	
	int index1 = 	blockIdx.x*16 + threadIdx.x + x
					+ (blockIdx.y+y)*i_w;
	//int index2 = 	index0 + t_w*t_w*threadIDx.z;	
	float diff = img[index1]-t[index0];
	out[index0] = diff*diff;
}


float calc_min_dist(float *img, int i_w, int i_h, float *t, int t_w) {

	float min_dist = FLT_MAX;
	float* dist = (float*)malloc(sizeof(float));
	int x = i_w-t_w;
	int y = i_h-t_w;

	float* t2,* t3,* t4;
	
	int t_len = t_w*t_w;
	int t_len2 = t_len*sizeof(float);
	//int len = 8*t_w*t_w*sizeof(float);
	CUDA_SAFE_CALL(cudaMalloc(&t2, t_len2));
	CUDA_SAFE_CALL(cudaMalloc(&t3, t_len2));
	float* out;
	CUDA_SAFE_CALL(cudaMalloc(&out, t_len2));

	CUDA_SAFE_CALL(cudaMemcpy(t2, t,t_len2,cudaMemcpyDeviceToDevice)); 	
	// //t_len *=4;
	// flip(t, t2+t_len2, t_w);
	// //flip(t2+t_len2, t2, t_w);
	// transpose(t2+t_len2, t2+t_len2*2, t_w);
	// flip(t2+t_len2*2, t2+t_len2*3, t_w);
	// transpose(t2+t_len2*3, t2+t_len2*4, t_w);
	// flip(t2+t_len2*4, t2+t_len2*5, t_w);
	// transpose(t2+t_len2*5, t2+t_len2*6, t_w);
	// flip(t2+t_len2*6, t2+t_len2*7, t_w);
	// //t_len/=4;

	for (int k = 1; k<9;k++){

		if (k%2==0){
			flip(t2, t3, t_w);
			t4=t2;
			t2=t3;
			t3=t4;
		}else if(k!=1){
			transpose(t2, t3, t_w);
			t4=t2;
			t2=t3;
			t3=t4;
		}


		for (int i=0; i<=x;i++){
			for (int j=0; j<=y;j++){
				//printf("k:%d/n",k);
				dim3 dim_blocks(t_w/16,t_w);
				dim3 dim_threads(16, 1, 1);
				calcdistKernel<<<dim_blocks, dim_threads>>>(out, img, i_w, i_h, t2, t_w, i, j);
				cudaThreadSynchronize();
				CUT_CHECK_ERROR("");

				reductionGPU(out, t_len);

				CUDA_SAFE_CALL(cudaMemcpy(dist, out,sizeof(float),cudaMemcpyDeviceToHost)); 
				if(*dist<min_dist) min_dist=*dist;
				//int offset=t_len;
				// CUDA_SAFE_CALL(cudaMemcpy(dist, out+offset,sizeof(float),cudaMemcpyDeviceToHost)); 
				// if(*dist<min_dist) min_dist=*dist;
				// offset+=t_len;
				// CUDA_SAFE_CALL(cudaMemcpy(dist, out+offset,sizeof(float),cudaMemcpyDeviceToHost)); 
				// if(*dist<min_dist) min_dist=*dist;
				// offset+=t_len;
				// CUDA_SAFE_CALL(cudaMemcpy(dist, out+offset,sizeof(float),cudaMemcpyDeviceToHost)); 
				// if(*dist<min_dist) min_dist=*dist;
				// offset+=t_len;
				// CUDA_SAFE_CALL(cudaMemcpy(dist, out+offset,sizeof(float),cudaMemcpyDeviceToHost)); 
				// if(*dist<min_dist) min_dist=*dist;
				// offset+=t_len;
				// CUDA_SAFE_CALL(cudaMemcpy(dist, out+offset,sizeof(float),cudaMemcpyDeviceToHost)); 
				// if(*dist<min_dist) min_dist=*dist;
				// offset+=t_len;
				// CUDA_SAFE_CALL(cudaMemcpy(dist, out+offset,sizeof(float),cudaMemcpyDeviceToHost)); 
				// if(*dist<min_dist) min_dist=*dist;
				// offset+=t_len;
				// CUDA_SAFE_CALL(cudaMemcpy(dist, out+offset,sizeof(float),cudaMemcpyDeviceToHost)); 
				// if(*dist<min_dist) min_dist=*dist;
			}
		}
	}

	CUDA_SAFE_CALL(cudaFree(t2));
	CUDA_SAFE_CALL(cudaFree(out));

	return min_dist;
}
