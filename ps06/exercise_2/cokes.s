.data
OUTPUT_STRING:
.string "%d bottles of Coke on the wall.\n%d bottles of beer.\nTake one down, pass it around.\n"
.text
.globl main
			
main:			/∗ Create a label called main in the object code */
	movl $99,%eax	/* Setting number of bottles*/
begin_loop:		/* Create a label where we will come back if %eax is greater than zero*/
	pushl %eax	/* First parameter referred to in the string*/
	pushl %eax	/* First parameter referred to in the string*/
	pushl $OUTPUT_STRING	/* Push the address of our string*/
	call printf	/* Give control to printf*/
	addl $8,%esp	/* Restore the stack pointer*/
	popl %eax	/* Get the old eax bakc*/
	decl %eax	/* Decrease the number of bottles*/
	cmpl $0,%eax	/* Compare the counter with 0*/
	jne begin_loop	 /*Jump to the begin of the loop if it is not equal*/
	ret		/* Return control wherever main was called from */

