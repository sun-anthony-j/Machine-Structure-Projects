/*
 * PROJ1-1: YOUR TASK B CODE HERE
 * 
 * Feel free to define additional helper functions.
 */

#include <stdlib.h>
#include <stdio.h>
#include "sparsify.h"
#include "utils.h"

/* Returns a NULL-terminated list of Row structs, each containing a NULL-terminated list of Elem structs.
 * See sparsify.h for descriptions of the Row/Elem structs.
 * Each Elem corresponds to an entry in dense_matrix whose value is not 255 (white).
 * This function can return NULL if the dense_matrix is entirely white.
 */
Row *dense_to_sparse(unsigned char *dense_matrix, int width, int height) {
    /* YOUR CODE HERE */
	int i = 0;
	int j = 0;
	struct Row *row = (struct Row*)malloc(sizeof(struct Row));
	if (NULL == row)
		allocation_failed();
	struct Row *start = NULL;
	struct Elem *oldelem=NULL;
	struct Row *oldrow = NULL;
	while (j<height){
		struct Elem *elem = (struct Elem*)malloc(sizeof(struct Elem));
		if (NULL == elem)
			allocation_failed();
		struct Elem *start2 = NULL;
		while (i<width){
			if (dense_matrix[j*width+i] != 255){	
				if (NULL == start2)
					start2 = elem;
				elem->x = i;
				elem->value = dense_matrix[j*width+i];
				struct Elem *newelem = (struct Elem*)malloc(sizeof(struct Elem));
				elem->next = newelem;
				oldelem=elem;
				elem= newelem;
			}
			++i;
		}
		if (start2 != NULL){
			if (NULL == start)
				start = row;
			row->y = j;
			row->elems = start2;
			struct Row *newrow = (struct Row*)malloc(sizeof(struct Row));
			if (NULL == row)
				allocation_failed();
			row->next = newrow;
			oldrow=row;
			row=newrow;
		}
		++j;
		i = 0;
		free(elem);
		if (oldelem)
			oldelem->next=0;
	}
	free(row);
	if (oldrow)
		oldrow->next=0;
	return start;

}

void free_elem(Elem *elem){
	if (elem == NULL){
		return;
	}
	free_elem(elem->next);
	free(elem);
	return;
}

/* Frees all memory associated with SPARSE. SPARSE may be NULL. */
void free_sparse(Row *sparse) {
    /* YOUR CODE HERE */
	if (sparse == NULL){
		return;
	}
	free_elem(sparse->elems);
	free_sparse(sparse->next);
	free(sparse);
	return;
}