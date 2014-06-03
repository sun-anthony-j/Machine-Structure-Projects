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


/* Swaps the values pointed to by the pointers X and Y. */
void swap(float *x, float *y) {
	float temp = *x;
	*x = *y;
	*y = temp;
}

/* Flips the elements of a square array ARR across the y-axis. */
void flip_horizontal(float *arr, int width) {
	int i = 0;
	int j = 0;
	do{
		do{
			swap(arr+j*width+i,arr+j*width+width-i-1);
			}while(++i<width/2);
		i = 0;
	} while (++j<width);
}

/* Transposes the square array ARR. */
void transpose(float *arr, int width) {
	int i = 0;
	int j = 0;
	while (++j<width){
		do{
			swap(arr+j*width+i, arr+i*width+j);
		} while (++i<j);
		i = 0;
	} 
	
}

/* Rotates the square array ARR by 90 degrees counterclockwise. */
void rotate_ccw_90(float *arr, int width) {
    /* Optional function */
	flip_horizontal(arr, width);
	transpose(arr, width);
	
}

float calc_distance(float *image, int i_width, int x, int y, float *template, int t_width){
	float sum = 0;
	
	for (int j=0; j<t_width; j++){
		#pragma omp parallel reduction(+:sum)
		{
			int n = omp_get_num_threads();
			int m = omp_get_thread_num();
			int rounded = t_width/(n*4)*(n*4);
			__m128 image2;
			__m128 template2;
			__m128 difference;
			__m128 output =  _mm_setzero_ps();;
			float w;
			float x;
			float y;
			float z;
			int i;
			
			for (int index=0; index<rounded; index++){
				i= (index*n+m)*4;
				w = template[i+t_width*j]; 
				x = template[i+t_width*j+2];
				y = template[i+t_width*j+3];
				z = template[i+t_width*j+4];
				template2 = _mm_set_ps(w,x,y,z);
				int offset = (x+i)+i_width*(y+j);
				image2 = _mm_loadu_ps(image+offset);
				difference = _mm_sub_ps(image2, template2);
				difference = _mm_mul_ps(difference, difference);
				output = _mm_add_ps(output, difference);
			}
			float A[4];
			_mm_storeu_ps(A, output);
			float sum2 = (A[0] + A[1] +A[2]+A[3]);
			sum += sum2;
		}	
		for( int i = t_width/32*32; i < t_width; i++ ) {
			sum +=(image[(x+i)+i_width*(y+j)]-template[i+t_width*j])*(image[(x+i)+i_width*(y+j)]-template[i+t_width*j]);
		}
		
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
	float temp;
	int i;
	int j = 0;
	do{
		i = 0;
		do{
			//1
			temp = calc_distance(image, i_width, i, j, template, t_width);
			if (temp < min_dist)
				min_dist = temp;
			//2
			rotate_ccw_90(template, t_width);
			temp = calc_distance(image, i_width, i, j, template, t_width);
			if (temp < min_dist)
				min_dist = temp;
			//3
			rotate_ccw_90(template, t_width);
			temp = calc_distance(image, i_width, i, j, template, t_width);
			if (temp < min_dist)
				min_dist = temp;
			//4
			rotate_ccw_90(template, t_width);
			temp = calc_distance(image, i_width, i, j, template, t_width);
			if (temp < min_dist)
				min_dist = temp;
			//5
			rotate_ccw_90(template, t_width);
			flip_horizontal(template, t_width);
			temp = calc_distance(image, i_width, i, j, template, t_width);
			if (temp < min_dist)
				min_dist = temp;
			//6
			rotate_ccw_90(template, t_width);
			temp = calc_distance(image, i_width, i, j, template, t_width);
			if (temp < min_dist)
				min_dist = temp;
			//7
			rotate_ccw_90(template, t_width);
			temp = calc_distance(image, i_width, i, j, template, t_width);
			if (temp < min_dist)
				min_dist = temp;
			//8
			rotate_ccw_90(template, t_width);
			temp = calc_distance(image, i_width, i, j, template, t_width);
			if (temp < min_dist)
				min_dist = temp;
		}while(++i<=i_width-t_width);
	} while (++j<=i_height-t_width);
    return min_dist;
}   