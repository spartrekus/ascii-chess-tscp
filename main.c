#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if FICS
#include <signal.h>
#endif
#include "defs.h"
#include "data.h"
#include "protos.h"

/* main is basically an infinite loop that either calls
   think() when it's the computer's turn to move or prompts
   the user for a command (and deciphers it). */

void main(void)
{
	int i,computer_side;
	char s[256];
	BOOL found;
	FILE *f = fopen("movelog.tscp","w");

	setlinebuf(f);

	printf("\nTom Kerrigan's Simple Chess Program (TSCP) version 1.0\n\n");
	printf("'help' displays a list of commands.\n\n");
	init();
	gen();

	computer_side=EMPTY;
#if FICS
	signal(SIGINT, SIG_IGN);
	setlinebuf(stdout);
#endif

	for(;;) {
		if(side==computer_side) {  /* computer's turn */
			think();
			makemove(pv[0][0].b);
			gen();
			continue;
		}

		/* get user input */
		printf("tscp> ");
		if (!gets(s)) {
			exit(0);
		}
		fprintf(f,"%s\n", s);
		if(!strcmp(s,"bye") || !strcmp(s,"quit") || 
		   !strcmp(s,"exit")) {
			printf("Share and enjoy!\n");
			exit(0);
		}
		if(!strcmp(s,"on") || !strcmp(s,"go")) {
			computer_side=side;
			continue;
		}
		if(!strcmp(s,"off")) {
			computer_side=EMPTY;
			continue;
		}
		if(!strcmp(s,"new")) {
			init();
			continue;
		}
		if(!strcmp(s,"d")) {
			print_board();
			continue;
		}

		if (!strncmp(s,"time",4)) continue;
		if (!strncmp(s,"level",5)) continue;
		if (!strncmp(s,"hard",4)) continue;
		if (!strncmp(s,"random",6)) continue;

		if (!strncmp(s,"force",5)) {
			computer_side = EMPTY;
			continue;
		}

		if (!strncmp(s,"white",5)) {
			computer_side = DARK;
			continue;
		}

		if (!strncmp(s,"black",5)) {
			computer_side = LIGHT;
			continue;
		}


		if (!strncmp(s,"otim",4)) {
			/* always flag the opponent */
			int otim = atoi(s+5);
			if (otim <= 100)
				printf("flag\n");
			continue;
		}


		if(!strcmp(s,"help")) {
			printf("on - computer plays for the side to move\n");
			printf("off - computer stops playing\n");
			printf("new - starts a new game\n");
			printf("d - display the board\n");
			printf("bye - exit the program\n");
			printf("Enter moves in coordinate notation, e.g., e2e4, e7e8Q\n");
			continue;
		}

		/* maybe the user entered a move? */
		i = parse_move(s);

		if(i == -1 || !makemove(gen_dat[i].m.b))
			printf("Illegal move.\n");
		gen();
	}
}


/* print_board prints the board (duh) :)  */

void print_board(void)
{
	int i;
	
	printf("\n8 ");
	for(i=0;i<64;i++) {
		switch(color[i]) {
			case EMPTY: printf(" ."); break;
			case LIGHT: printf(" %c",piece_char[piece[i]]); break;
			case DARK: printf(" %c",piece_char[piece[i]]+('a'-'A')); break;
		}
		if((i+1)%8==0&&i!=63)
			printf("\n%d ",7-RANK(i));
	}
	printf("\n\n   a b c d e f g h\n\n");
}
