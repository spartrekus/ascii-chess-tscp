#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defs.h"
#include "data.h"
#include "protos.h"

/* parse moves in either algebraic (e4) or verbose (e2e4) format 

   This file was added by Andrew Tridgell (tridge@samba.anu.edu.au)
   in order to support RoboFICS. It is based on some code I wrote for
   KnightCap.
*/

#define POSN(x,y) ((x) + (7-(y))*8)
#define YPOS(p) (7-((p)>>3))
#define XPOS(p) ((p)&7)

#define E1 POSN(4,0)
#define C1 POSN(2,0)
#define G1 POSN(6,0)
#define E8 POSN(4,7)
#define C8 POSN(2,7)
#define G8 POSN(6,7)


static int charpiece(char c)
{
	switch (tolower(c)) {
	case 'p': return PAWN;
	case 'n': return KNIGHT;
	case 'r': return ROOK;
	case 'b': return BISHOP;
	case 'q': return QUEEN;
	case 'k': return KING;
	}
	return 0;
}

static int isrank(char c)
{
	return (c >= '1' && c <= '8');
}

static int isfile(char c)
{
	return (c >= 'a' && c <= 'h');
}


static int parse_square(char *s, int *sq)
{
	if (s[0] < 'a' || s[0] > 'h' ||
	    s[1] < '1' || s[1] > '8') 
		return 0;

	*sq = POSN(s[0]-'a', s[1] - '1');
	return 1;
}

static int parse_algebraic(char *s)
{
	int capture = 0;
	int p = -1;
	int from = -1, to = -1, file = -1, rank = -1;
	int n, i;

	if (isdigit(s[1]) && isfile(s[0])) {
		/* it must be a pawn move */
		if (!parse_square(s, &to))
			return -1;
		file = XPOS(to);
		p = PAWN;
	} else if (isupper(s[0])) {
		/* its a piece move */
		p = charpiece(s[0]);
		if (s[1] == 'x') {
			capture = 1;
			if (!parse_square(s+2, &to))
				return -1;
		} else if (s[2] == 'x') {
			capture = 1;
			if (isfile(s[1])) {
				file = s[1] - 'a';
			} else if (isrank(s[1])) {
				rank = s[1] - '1';
			} else {
				return -1;
			}
			if (!parse_square(s+3, &to))
				return -1;
		} else if (s[3] == 'x') {
			capture = 1;
			if (!parse_square(s+1, &from))
				return -1;
			if (isupper(s[4])) {
				if (!parse_square(s+5, &to))
					return -1;			
			} else {
				if (!parse_square(s+4, &to))
					return -1;			
			}
		} else {
			/* its not a capture! */
			if (parse_square(s+3, &to)) {
				if (!parse_square(s+1, &from))
					return -1;
			} else if (parse_square(s+2, &to)) {
			if (isfile(s[1])) {
					file = s[1] - 'a';
				} else if (isrank(s[1])) {
					rank = s[1] - '1';
				} else {
					return -1;
				}
			} else if (!parse_square(s+1, &to)) {
				return -1;
			}
		}
	} else {
		/* it must be a pawn capture */
		p = PAWN;
		capture = 1;
		file = s[0] - 'a';
		if (s[1] != 'x' || !parse_square(s+2, &to))
			return -1;
	}

	for(i=0;i<gen_end[ply];i++) {
		int m_from = gen_dat[i].m.b.from;
		int m_to = gen_dat[i].m.b.to;

		if ((color[m_from] == side) &&
		    (p == piece[m_from]) &&
		    (to == -1 || m_to == to) &&
		    (from == -1 || m_from == from) &&
		    (file == -1 || XPOS(m_from) == file) &&
		    (rank == -1 || YPOS(m_from) == rank)) {
#if 0
			printf("aparsed %s -> %s\n", s, movestr(&gen_dat[i].m.b));
#endif
			return i;
		}
	}

	return -1;
}


int parse_move(char *s)
{
	int i, from, to, found=FALSE;
	
	from=s[0]-'a';
	from+=8*(8-(s[1]-'0'));
	to=s[2]-'a';
	to+=8*(8-(s[3]-'0'));

	if (strcmp(s, "o-o")==0) {
		if (side==LIGHT) {
			from = E1;
			to = G1;
		} else {
			from = E8;
			to = G8;
		}
	}

	if (strcmp(s, "o-o-o")==0) {
		if (side==LIGHT) {
			from = E1;
			to = C1;
		} else {
			from = E8;
			to = C8;
		}
	}
	
	for(i=0;i<gen_end[ply];i++) {
		if(gen_dat[i].m.b.from==from&&gen_dat[i].m.b.to==to) {
			found=TRUE;
			break;
		}
	}

	if (found) {
		/* get the promotion piece right */
		if(gen_dat[i].m.b.bits&32)
			switch(s[4]) {
			case 'N': break;
			case 'B': i+=1; break;
			case 'R': i+=2; break;
			default: i+=3; break;
			}
#if 0
		printf("parsed %s -> %s\n", s, movestr(&gen_dat[i].m.b));
#endif
		return i;
	}

	return parse_algebraic(s);
}


char *movestr(move_bytes *m)
{
	static char buf[30];
	sprintf(buf,"%c%d%c%d", 
		FILE(m->from)+'a',
		8-RANK(m->from),
		FILE(m->to)+'a',
		8-RANK(m->to));
	return buf;
}

