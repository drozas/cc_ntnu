/*
 * asm_macros.h:
 * This file contains various convenience macros for generating an assembly
 * program. None of these macros create any significant program logic, but
 * they can be used for setup/takedown to get the code running.
 * To read the code in the longer ones, you are advised to shove them through
 * printf and read the result instead - it's a little prettier that way.
 */


/*
 * DATA_HEAD:
 * This starts a data section, and includes a static string to support
 * debugging and testing using printf; this is covered in the PRINTSTACKTOP
 * macro, below.
 */
#define DATA_HEAD \
    ".data\n"\
    "EXPR_FORMAT:\n.string \"%%d\"\n"


/*
 * PRINTSTACKTOP:
 * This macro is purely for debugging purposes, and will not be needed
 * to compile programs in the end. It pushes the EXPR_FORMAT string defined
 * in the head of the data section, calls printf, and removes EXPR_FORMAT
 * from the stack again. This will effectively print the value on the top
 * of the stack as an integer on standard output, as the format string
 * specifies that printf should read one integer down the stack from the
 * format string, making the top value a parameter. Since the system libs
 * make the callee responsible for handling parameters on the stack, we
 * leave the stack unchanged by removing only the string.
 * (Mind that calls to printf clobbers registers ECX and EDX, though...)
 */
#define PRINTSTACKTOP \
    "pushl $EXPR_FORMAT\n"\
    "call printf\n"\
    "popl %%ecx\n"


/*
 * TEXT_HEAD:
 * This defines the beginning of the program text with some code to set up
 * any arguments from the command line as a list of 32-bit integers at the
 * top of the stack, and performs a call to the entry point (first function)
 * generated from what we find in the vsl program.
 * Since we can't generally know the name of the first function, and VSL does
 * not force any particular name (like the C requirement that the entry point
 * is called 'main'), this macro must be printed with a string argument to
 * give the name of the first function (note the '%s' in the call instruction,
 * below). The reference implementation prints TEXT_HEAD thus:
 *
 * printf ( TEXT_HEAD, root->children[0]->children[0]->entry->label );
 *
 * (root is PROGRAM_N, child is FUNCTION_LIST_N, 1st grandchild is
 * FUNCTION_N, and it's symtab entry contains the name of the first function).
 * If your tree simplification does something to the FUNCTION_LIST, you
 * obviously need to alter the path to this string correspondingly.
 */
#define TEXT_HEAD \
".text\n.globl main\n"\
"main:\n"\
"\tpushl\t%%ebp\t/* Store the base of the caller (shell) stack frame */\n"\
"\tmovl\t%%esp,%%ebp\t/* Make my own stack frame */\n"\
"\tmovl\t8(%%esp),%%esi\t/* Store the first parameter (argc) in ESI */\n"\
"\tdecl\t%%esi\t/* argc--; argv[0] is not interesting to us */\n"\
"\tjz\tnoargs\t/* Skip argument setup if there are none */\n"\
"\n"\
"\tmovl\t12(%%ebp),%%ebx\t/* Store the base addr. of argv in EBX */\n"\
"pusharg:\t/* Loop over the arguments */\n"\
"\taddl\t$4,%%ebx\t/* Look at the next argument (disregarding argv[0]) */\n"\
"\tpushl\t$10\t/* strtol arg 3: our number base is 10 */\n"\
"\tpushl\t$0\t/* strtol arg 2: there is no error pointer */\n"\
"\tpushl\t(%%ebx)\t/* strtol arg 1: Addr. of string containing integer */\n"\
"\tcall\tstrtol\t/* Call strtol, to convert the string to a 32-bit int */\n"\
"\taddl\t$12,%%esp/* Restore the stack pointer to before strtol-params */\n"\
"\tpushl\t%%eax\t/* Push return value from strtol (our new argument) */\n"\
"\tdecl\t%%esi\t/* Decrement the number of arguments to go */\n"\
"\tjnz\tpusharg\t/* Loop if there are any arguments left */\n"\
"noargs:\n"\
"\tcall\t%s\t/* Args now on stack, call entry */\n"\
"\tleave\t/* Give the stack frame back to the calling shell */\n"\
"\tret\t/* Return value from vsl program is still in EAX */\n"\
"\n"


/*
 * FUNCTION_HEAD:
 * This is the code for beginning a function, which is where we take care of
 * calling conventions for our programs. The code given here follows the
 * standard convention used by most compilers for IA32.
 *
 * The first line is a label, corresponding to the function name. This is only
 * given as '%s' here, so the printf which outputs this string will need
 * another argument, containing the function name.
 * Pushing EBP stores a copy of the base of caller's stack frame
 * Setting EBP to ESP creates a base for our own stack frame from the present
 * stack pointer.
 */
#define FUNCTION_HEAD \
    "%s:\n"\
    "\tpushl\t%%ebp\n"\
    "\tmovl\t%%esp,%%ebp\n"


/*
 * FUNCTION_TAIL:
 * This is the code for ending every function:
 *
 * 'leave' hands the stack frame back to caller (i.e. restores old EBP, ESP
 * using our EBP, set up when the function begins)
 *
 * 'ret' returns control
 */
#define FUNCTION_TAIL\
    "\tleave\n"\
    "\tret\n"
