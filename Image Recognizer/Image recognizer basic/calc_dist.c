/*
 * PROJ1-1: YOUR TASK A CODE HERE
 *
 * You MUST implement the calc_min_dist() function in this file.
 *
 * You do not need to implement/use the swap(), flip_horizontal(), transpose(), or rotate_ccw_90()
 * functions, but you may find them useful. Feel free to define additional helper functions.
 */

#include <stdlib.h>
#include <stdio.h>
#include "digit_rec.h"
#include "utils.h"
#include "limits.h"

/* Swaps the values pointed to by the pointers X and Y. */
void swap(unsigned char *x, unsigned char *y) {
	unsigned char temp = *x;
	*x = *y;
	*y = temp;
}

/* Flips the elements of a square array ARR across the y-axis. */
void flip_horizontal(unsigned char *arr, int width) {
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
void transpose(unsigned char *arr, int width) {
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
void rotate_ccw_90(unsigned char *arr, int width) {
    /* Optional function */
	flip_horizontal(arr, width);
	transpose(arr, width);
	
}

unsigned int calc_distance(unsigned char *image, int i_width, int x, int y, unsigned char *template, int t_width){
	int sum = 0;
	int i = 0;
	int j = 0;
	do{
		do{
			sum = sum + (image[(x+i)+i_width*(y+j)]-template[i+t_width*j])*(image[(x+i)+i_width*(y+j)]-template[i+t_width*j]);
		}while(++i<t_width);
		i = 0;
	} while (++j<t_width);
	return sum;
}

/* Returns the squared Euclidean distance between TEMPLATE and IMAGE. The size of IMAGE
 * is I_WIDTH * I_HEIGHT, while TEMPLATE is square with side length T_WIDTH. The template
 * image should be flipped, rotated, and translated across IMAGE.
 */
unsigned int calc_min_dist(unsigned char *image, int i_width, int i_height, unsigned char *template, int t_width) {
    unsigned int min_dist = UINT_MAX;
    /* YOUR CODE HERE */
	unsigned int temp;
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