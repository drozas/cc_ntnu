.data
EXPR_FORMAT:
.string "%d"
.text
.globl main
main:
	pushl	%ebp	/* Store the base of the caller (shell) stack frame */
	movl	%esp,%ebp	/* Make my own stack frame */
	movl	8(%esp),%esi	/* Store the first parameter (argc) in ESI */
	decl	%esi	/* argc--; argv[0] is not interesting to us */
	jz	noargs	/* Skip argument setup if there are none */

	movl	12(%ebp),%ebx	/* Store the base addr. of argv in EBX */
pusharg:	/* Loop over the arguments */
	addl	$4,%ebx	/* Look at the next argument (disregarding argv[0]) */
	pushl	$10	/* strtol arg 3: our number base is 10 */
	pushl	$0	/* strtol arg 2: there is no error pointer */
	pushl	(%ebx)	/* strtol arg 1: Addr. of string containing integer */
	call	strtol	/* Call strtol, to convert the string to a 32-bit int */
	addl	$12,%esp/* Restore the stack pointer to before strtol-params */
	pushl	%eax	/* Push return value from strtol (our new argument) */
	decl	%esi	/* Decrement the number of arguments to go */
	jnz	pusharg	/* Loop if there are any arguments left */
noargs:
	call	_main	/* Args now on stack, call entry */
	leave	/* Give the stack frame back to the calling shell */
	ret	/* Return value from vsl program is still in EAX */

_main: 	
	 pushl -4(%ebp) 	/* Var declaration*/
	 pushl -8(%ebp) 	/* Var declaration*/
	 pushl $16 	/*Push of an integer value*/
	 popl %eax 	/*Pop with the final result*/
	 movl %eax,-4(%ebp) 	 /*Moving the result to the var*/
	 pushl $5 	/*Push of an integer value*/
	 popl %eax 	/*Pop with the final result*/
	 movl %eax,-8(%ebp) 	 /*Moving the result to the var*/
	 pushl $2 	/*Push of an integer value  in the first operand*/
	 pushl -4(%ebp) 	/*Push of a variable in the second operand*/
	 movl $0,%edx 	/*Initialize %edx*/ 
	 popl %eax 	/*Pop of the first expression value*/
	 popl %ecx 	/*Pop of the second expression value*/
	 imull %ecx 	/*Multiplication, result saved in %eax*/
	 pushl %eax 	/*Push the result at the top of the stack*/
	 popl %eax 	/*Pop with the final result*/
	 movl %eax,-4(%ebp) 	 /*Moving the result to the var*/
	 pushl -4(%ebp) 	/*Push of a variable in the first operand*/
	 pushl -8(%ebp) 	/*Push of a variable in the second operand*/
	 movl $0,%edx 	/*Initialize %edx*/ 
	 popl %eax 	/*Pop of the first expression value*/
	 popl %ecx 	/*Pop of the second expression value*/
	 idivl %ecx 	/*Division, result saved in %eax*/
	 pushl %eax 	/*Push the result at the top of the stack*/
	 popl %eax 	/*Pop with the final result*/
	 movl %eax,-8(%ebp) 	 /*Moving the result to the var*/
	 pushl -8(%ebp) 	/*Push of a variable*/
	 pushl %eax 	 /*Push of the final result*/
	 ret 	 /*Return control*/
