##### sparse2dense function code #####
.text
# sparse2dense will have 2 arguments: $a0 = address of sparse matrix data, $a1 = address of dense matrix, $a2 = matrix width
# Recall that sparse matrix representation uses the following format:
# Row r<y> {int row#, Elem *node, Row *nextrow}
# Elem e<y><x> {int col#, int value, Elem *nextelem}
sparse2dense:
	### YOUR CODE HERE ###
	li	$t0, 0		#initialize column index
	li	$t1, 0		#initialize row index
	move	$t2, $ra

	#For each row
	sparserow:
	beqz	$a0, finishsparse	#finish if null pointer
	lw	$t7, 0($a0)	#load row number
	lw	$t8, 4($a0)	#load elem pointer
	sub	$t7,$t7,$t1		#t7 = row number - row index
	beqz	$t7, sparseelement	#go to sparseelement if row at row index has elements
	emptyrow:
	sub	$t7,$a2,$t0		
	blez	$t7, finishemptyrow	
	sw	$zero, 0($a1)	#populate dense matrix
	addi	$a1,$a1,4	#increment dense matrixpointer
	addi	$t0,$t0,1	#increment column index
	b	emptyrow
	#Populate each element in dense
	sparseelement:
	sub 	$t7, $a2, $t0	
	blez	$t7, finishsparserow
	li	$t5,0		#initialize content to populate as 0
	beqz	$t8, emptyelement
	lw	$t7, 0($t8)	#load	column number
	sub	$t7, $t7,$t0
	bgtz	$t7, emptyelement	#if next thing in sparse is not in index, skip to
	lw 	$t5, 4($t8)	#load value at column index
	lw	$t8, 8($t8)	#increment element pointer
	emptyelement:
	sw	$t5, 0($a1)	#populate dense matrix
	addi	$a1,$a1,4	#increment dense matrixpointer
	addi	$t0,$t0,1	#increment column index
	b	sparseelement
	finishsparserow:
	lw	$a0,8($a0)	#increment rowpointer
	finishemptyrow:
	li	$t0,0		#reset column index
	addi	$t1,$t1,1	#increment row index
	b	sparserow
	
	finishsparse:
	jr $t2
