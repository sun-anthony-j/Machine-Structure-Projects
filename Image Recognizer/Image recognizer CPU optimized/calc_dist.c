/* CS61C Sp14 Project 3 Part 1: YOUR CODE HERE
 *
 * You MUST implement the calc_min_dist() function in this file.
 *
 * You do not need to implement/use the swap(), flip_horizontal(), transpose(),
 * or rotate_ccw_90() functions, but you may find them useful. Feel free to
 * define additional helper functions.
 */

#include <float.h>
#include <stdlib.h>
#include <stdio.h>
#include "digit_rec.h"
#include "utils.h"
#include <emmintrin.h> 
#include <omp.h>



void flip_horizontal_parallel(float *arr, int width) {

	#pragma omp parallel for num_threads(8)
	for (int j=0; j<width; j++){
		float temp;
		int ind1,ind2;
		for (int i=0;i<width/2;i+=8 ){
			// swap(arr+j*width+i,arr+j*width+width-i-1);
			ind1 = j*width+i;
			ind2 = j*width+width-i-1;
			temp = arr[ind1];
			arr[ind1]=arr[ind2];
			arr[ind2]=temp;
			ind1++;
			ind2--;
			temp = arr[ind1];
			arr[ind1]=arr[ind2];
			arr[ind2]=temp;
			ind1++;
			ind2--;
			temp = arr[ind1];
			arr[ind1]=arr[ind2];
			arr[ind2]=temp;
			ind1++;
			ind2--;
			temp = arr[ind1];
			arr[ind1]=arr[ind2];
			arr[ind2]=temp;
			ind1++;
			ind2--;
			temp = arr[ind1];
			arr[ind1]=arr[ind2];
			arr[ind2]=temp;
			ind1++;
			ind2--;
			temp = arr[ind1];
			arr[ind1]=arr[ind2];
			arr[ind2]=temp;
			ind1++;
			ind2--;
			temp = arr[ind1];
			arr[ind1]=arr[ind2];
			arr[ind2]=temp;
			ind1++;
			ind2--;
			temp = arr[ind1];
			arr[ind1]=arr[ind2];
			arr[ind2]=temp;

		}
		for (int i=width/16*8;i<width/2;i++ ){
			temp = arr[j*width+i];
			arr[j*width+i]=arr[j*width+width-i-1];
			arr[j*width+width-i-1]=temp;
		}
	}
	
}

void flip_horizontal(float *arr, int width) {
	// flip_horizontal_parallel(arr, width);
	for (int j=0; j<width; j++){
		float temp;
		int ind1,ind2;
		for (int i=0;i<width/2;i+=4 ){
			// swap(arr+j*width+i,arr+j*width+width-i-1);
			ind1 = j*width+i;
			ind2 = j*width+width-i-1;
			temp = arr[ind1];
			arr[ind1]=arr[ind2];
			arr[ind2]=temp;
			ind1++;
			ind2--;
			temp = arr[ind1];
			arr[ind1]=arr[ind2];
			arr[ind2]=temp;
			ind1++;
			ind2--;
			temp = arr[ind1];
			arr[ind1]=arr[ind2];
			arr[ind2]=temp;
			ind1++;
			ind2--;
			temp = arr[ind1];
			arr[ind1]=arr[ind2];
			arr[ind2]=temp;
		}
		for (int i=width/8*4;i<width/2;i++ ){
			temp = arr[j*width+i];
			arr[j*width+i]=arr[j*width+width-i-1];
			arr[j*width+width-i-1]=temp;
		}
	}
	
}
void transpose_parallel(float *arr, int width) {

	float temp;
	#pragma omp parallel for private(temp) num_threads(8)
	for (int j=1; j<width; j++){
		
		for (int i=0;i<j;i++){
			// swap(arr+j*width+i, arr+i*width+j);
			temp = arr[j*width+i];
			arr[j*width+i]=arr[i*width+j];
			arr[i*width+j]=temp;
		}
	} 
	// #pragma omp parallel for private(temp)
	// for(int k=0; k<=(width/128);k++){
 //        for(int l=0; l<=(width/128);l++){
 //            for(int j = l*128; (j<(l+1)*128)&&(j < width); j++){
 //                for(int i = k*128; (i<(k+1)*128)&&(i < j); i++){
	// 				temp = arr[j*width+i];
	// 				arr[j*width+i]=arr[i*width+j];
	// 				arr[i*width+j]=temp;
	// 			}
	// 		}
	// 	}
	// }
}
/* Transposes the square array ARR. */
void transpose(float *arr, int width) {

	// transpose_parallel(arr,width);
	float temp;
	for (int j=1; j<width; j++){
		for (int i=0;i<j;i++){
			// swap(arr+j*width+i, arr+i*width+j);
			temp = arr[j*width+i];
			arr[j*width+i]=arr[i*width+j];
			arr[i*width+j]=temp;
		}
	} 
	
}

float calc_distance(float *image, int i_width, int x, int y, float *template, int t_width){
	float sum = 0;
	int rounded = (t_width/16)*16;
	float lsum;
	float diff;
	int img_ind;
	int temp_ind;
	for (int j=0; j<t_width; j++){

		lsum=0;
		__m128 image2;
		__m128 template2;
		__m128 difference;
		__m128 output =  _mm_setzero_ps();;


		for (int i=0; i<rounded; i+=16){
			img_ind=(x+i)+i_width*(y+j);
			temp_ind=i+t_width*j;

			image2 = _mm_loadu_ps(image+img_ind);
			template2 =  _mm_loadu_ps(template+temp_ind);
			difference = _mm_sub_ps(image2, template2);
			difference = _mm_mul_ps(difference, difference);
			output = _mm_add_ps(output, difference);

			temp_ind+=4;
			img_ind+=4;

			image2 = _mm_loadu_ps(image+img_ind);
			template2 =  _mm_loadu_ps(template+temp_ind);
			difference = _mm_sub_ps(image2, template2);
			difference = _mm_mul_ps(difference, difference);
			output = _mm_add_ps(output, difference);

			temp_ind+=4;
			img_ind+=4;

			image2 = _mm_loadu_ps(image+img_ind);
			template2 =  _mm_loadu_ps(template+temp_ind);
			difference = _mm_sub_ps(image2, template2);
			difference = _mm_mul_ps(difference, difference);
			output = _mm_add_ps(output, difference);

			temp_ind+=4;
			img_ind+=4;

			image2 = _mm_loadu_ps(image+img_ind);
			template2 =  _mm_loadu_ps(template+temp_ind);
			difference = _mm_sub_ps(image2, template2);
			difference = _mm_mul_ps(difference, difference);
			output = _mm_add_ps(output, difference);



		}
		float A[4];
		_mm_storeu_ps(A, output);
		lsum += (A[0] + A[1] +A[2]+A[3]);
		
		for (int i = rounded;i<t_width;i++){
			diff = image[(x+i)+i_width*(y+j)]-template[i+t_width*j];
			lsum += diff*diff;
		}
		sum+=lsum;
		
	}

	return sum;
}

/* Returns the squared Euclidean distance between TEMPLATE and IMAGE. The size of IMAGE
 * is I_WIDTH * I_HEIGHT, while TEMPLATE is square with side length T_WIDTH. The template
 * image should be flipped, rotated, and translated across IMAGE.
 */
float calc_min_dist(float *image, int i_width, int i_height, float *template, int t_width) {
    float min_dist = FLT_MAX;
    /* YOUR CODE HERE */
    // if (i_width == 128 && i_height == 128 && t_width == 64){
    // 	return calc_min_dist_bench(image, i_width, i_height, template, t_width);
    // }
	float temp;
	int i;
	
	for (int k = 1; k<9;k++){
		if (t_width<200){
			if (k%2==0){
				flip_horizontal(template, t_width);
			}else if(k!=1){
				transpose(template, t_width);
			}
		}else{
			if (k%2==0){
				flip_horizontal_parallel(template, t_width);
			}else if(k!=1){
				transpose_parallel(template, t_width);
			}
		}



		int j = 0;
		do{
			
			#pragma omp parallel num_threads(8)
			{
				float local_min=min_dist;
				#pragma omp for
				for (i = 0; i<=i_width-t_width; i++){
					temp = calc_distance(image, i_width, i, j, template, t_width);
					if (temp < min_dist)
						local_min = temp;			
				}
				if (local_min<min_dist){
					#pragma omp critical
					{
					if (local_min<min_dist){
					min_dist=local_min;
					}
					}
				}

			}
		} while (++j<=i_height-t_width);
	}
    return min_dist;
}   