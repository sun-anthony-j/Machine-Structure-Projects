##### Variables #####
.data
# Header for dense matrix
head:		.asciiz	"  -----0----------1----------2----------3----------4----------5----------6----------7----------8----------9-----\n"

##### print_dense function code #####
.text
# print_dense will have 3 arguments: $a0 = address of dense matrix, $a1 = matrix width, $a2 = matrix height
print_dense:
	### YOUR CODE HERE ###
	move	$t0, $a0	#move matrix address
	move	$t1, $a1	#move matrix width
	move	$t2, $a2	#move matrix height
	li	$t3, 0		#initialize column index
	li	$t4, 0		#initialize row index
	move	$t5,$ra		#store return address
	la	$a0, head	# load header
	jal	print_str	# print start message
	
	#For each row
	rowloop:
	sub 	$t7, $t2, $t4
	blez	$t7, finish
	#Print row number
	move	$a0, $t4
	jal	print_int
	jal	print_space
	#Print each element
	elementloop:
	sub 	$t6, $t1, $t3
	blez 	$t6, finishloop
	lw 	$a0, 0($t0)	#load matrix content
	addi	$t0,$t0,4	#increment pointer
	jal	print_intx
	jal	print_space
	addi	$t3,$t3,1	#increment column index
	b	elementloop
	finishloop:
	jal	print_newline
	li	$t3,0		#reset column index
	addi	$t4,$t4,1	#increment row index
	b	rowloop
	
	finish:
	jr $t5
