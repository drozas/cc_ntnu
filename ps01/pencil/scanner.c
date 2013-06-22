#include <stdlib.h>
#include <inttypes.h>
#include <ctype.h>

#include "pencil.h"

#define	STATES	13
#define NCHARS	128 //It is supposed that the input can be any ascii character
#define INITIAL	0
#define FINAL_STATE_DRAW 10
#define FINAL_STATE_TURN 11
#define FINAL_STATE_MOVE 12

uint8_t table[STATES][NCHARS];
uint8_t state = INITIAL;


/*
 * This function is called before anything else, to initialize the
 * state machine. It is certainly possible to create implementations
 * which don't require any initialization, so just leave this blank if
 * you don't need it.
 */
void
init_transtab ( void )
{
	uint8_t i = 0;
	uint8_t j = 0;
	
	//Initialization
	for(i=0;i<STATES;i++)
		for(j=0;j<NCHARS;j++)
			table[i][j] = INITIAL;
	
	
	//Transitions 
	table[0]['D'-'A'] = 1;	
	table[1]['R'-'A'] = 2;
	table[2]['A'-'A'] = 3;
	table[3]['W'-'A'] = 10;

	table[0]['T'-'A'] = 4;	
	table[4]['U'-'A'] = 5;
	table[5]['R'-'A'] = 6;
	table[6]['N'-'A'] = 11;

	table[0]['M'-'A'] = 7;	
	table[7]['O'-'A'] = 8;
	table[8]['V'-'A'] = 9;
	table[9]['E'-'A'] = 12;
		
}


/*
 * Return the next token from reading the given input stream.
 * The words to be recognized are 'turn', 'draw' and 'move',
 * while the returned tokens may be TURN, DRAW, MOVE or END (as
 * enumerated in 'pencil.h').
 */
command_t
next ( FILE *input )
{
	char c;
	
	//Loop until EOF or any final state
	do
	{
		c = fgetc(input);
		
		if ((c!=EOF))
		{
			//next state
			c = toupper(c);
			state = table [state][c-'A'];
		}
		
	}while((c!=EOF) && (state != FINAL_STATE_TURN) &&(state != FINAL_STATE_MOVE) &&(state != FINAL_STATE_DRAW));
	

	if(state==FINAL_STATE_DRAW)
		return DRAW;
	else if (state==FINAL_STATE_MOVE)
		return MOVE;
	else if (state==FINAL_STATE_TURN)
		return TURN;
	else
		return END;
	
}
