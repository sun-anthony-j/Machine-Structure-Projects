# CS61C Sp14 Project 1-2
# Task A: The Collatz conjecture

.globl main
.include "collatz_common.s"

main:
	jal read_input			# Get the number we wish to try the Collatz conjecture on
	move $a0, $v0			# Set $a0 to the value read
	la $a1, collatz_iterative	# Set $a1 as ptr to the function we want to execute
	jal execute_function		# Execute the function
	li $v0, 10			# Exit
	syscall
	
# --------------------- DO NOT MODIFY ANYTHING ABOVE THIS POINT ---------------------

# Returns the stopping time of the Collatz function (the number of steps taken till the number reaches one)
# using an ITERATIVE approach. This means that if the input is 1, your function should return 0.
#
# The initial value is stored in $a0, and you may assume that it is a positive number.
# 
# Make sure to follow all function call conventions.
collatz_iterative:
	# YOUR CODE HERE
	li $v0, 0
	loop:   sub     $t7,$a0,1       # t7 = a0-1
        beqz    $t7,done	#loop done if t7 =0
        rem     $t6,$a0, 2     # remainder of a0/2
        addi    $v0,$v0,1       # increment counter
        bnez    $t6,odd
	even:   div     $a0,$a0,2     # if rem is even, divide a0 by 2:
	b	loop
	odd:    mul     $a0,$a0,3     # if rem is odd, mult a0 by 3 and add 1:
        addi    $a0,$a0,1
        b       loop
	done:
	jr	$ra
