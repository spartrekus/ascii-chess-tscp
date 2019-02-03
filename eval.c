#include <string.h>
#include "defs.h"
#include "data.h"
#include "protos.h"


/* pcsq stands for piece/square table. It's indexed by the piece's color,
   type, and square. The value of pcsq[LIGHT][KNIGHT][e5] might be 310
   instead of just 300 because a knight on e5 is better than one on
   one of the outer squares. */
int pcsq[2][6][64];

int eval(void)
{
	int i;
	int c;  /* color */
	int xc;  /* not c */
	int f;  /* file */
	int score[2];  /* the score for each side */
	int pawns[2][10];  /* the number of pawns of each color on each file */

	score[LIGHT]=score[DARK]=0;
	memset(pawns,0,sizeof(pawns));

	/* loop through to set up the pawns array and to add up the
	   piece/square table values for each piece. */
	for(i=0;i<64;i++) {
		if(color[i]==EMPTY)
			continue;
		if(piece[i]==PAWN)
			pawns[color[i]][FILE(i)+1]++;
		score[color[i]]+=pcsq[color[i]][piece[i]][i];
	}

	/* now that we have the pawns array set up, evaluate the pawns
	   and rooks */
	for(i=0;i<64;i++) {
		if(color[i]==EMPTY)
			continue;
		c=color[i];  /* set up c, xc, and f so we don't have to type a lot */
		xc=c^1;
		f=FILE(i)+1;
		if(piece[i]==PAWN) {
			if(pawns[c][f]>1)  /* this pawn is doubled */
				score[c]-=5;
			if((!pawns[c][f-1])&&
					(!pawns[c][f+1])) {  /* isolated */
				score[c]-=20;
				if(!pawns[xc][f])
					score[c]-=10;
			}
			if((!pawns[xc][f-1])&&  /* passed */
					(!pawns[xc][f])&&
					(!pawns[xc][f+1]))
				score[c]+=2*pcsq[c][PAWN][i];
		}
		if(piece[i]==ROOK)
			if(!pawns[c][f]) {  /* the rook is on a half-open file */
				score[c]+=10;
				if(!pawns[xc][f])  /* actually, it's totally open */
					score[c]+=5;
			}
	}

	/* return the score relative to the side to move (i.e.,
	   a positive score means the side to move is winning) */
	if(side==LIGHT)
		return(score[LIGHT]-score[DARK]);
	return(score[DARK]-score[LIGHT]);
}


int flip[64]= {
	 56,  57,  58,  59,  60,  61,  62,  63,
	 48,  49,  50,  51,  52,  53,  54,  55,
	 40,  41,  42,  43,  44,  45,  46,  47,
	 32,  33,  34,  35,  36,  37,  38,  39,
	 24,  25,  26,  27,  28,  29,  30,  31,
	 16,  17,  18,  19,  20,  21,  22,  23,
	  8,   9,  10,  11,  12,  13,  14,  15,
	  0,   1,   2,   3,   4,   5,   6,   7
};

int pawn_pcsq[64]= {
	  0,   0,   0,   0,   0,   0,   0,   0,
	  5,  10,  15,  20,  20,  15,  10,   5,
	  4,   8,  12,  16,  16,  12,   8,   4,
	  3,   6,   9,  12,  12,   9,   6,   3,
	  2,   4,   6,   8,   8,   6,   4,   2,
	  1,   2,   3,   4,   4,   3,   2,   1,
	  0,   0,   0, -20, -20,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0
};

int kingside_pawn_pcsq[64]= {
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   5,   5,   5,
	  0,   0,   0,   0,   0,  10,  10,  10,
	  0,   0,   0,   0,   0,  20,  20,  20,
	  0,   0,   0,   0,   0,   0,   0,   0	
};

int queenside_pawn_pcsq[64]= {
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,
	  5,   5,   5,   0,   0,   0,   0,   0,
	 10,  10,  10,   0,   0,   0,   0,   0,
	 20,  20,  20,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0	
};

int minor_pcsq[64]= {
	-10, -10, -10, -10, -10, -10, -10, -10,
	-10,   0,   0,   0,   0,   0,   0, -10,
	-10,   0,   5,   5,   5,   5,   0, -10,
	-10,   0,   5,  10,  10,   5,   0, -10,
	-10,   0,   5,  10,  10,   5,   0, -10,
	-10,   0,   5,   5,   5,   5,   0, -10,
	-10,   0,   0,   0,   0,   0,   0, -10,
	-20, -20, -20, -20, -20, -20, -20, -20,
};

int king_pcsq[64]= {
	-40, -40, -40, -40, -40, -40, -40, -40,
	-40, -40, -40, -40, -40, -40, -40, -40,
	-40, -40, -40, -40, -40, -40, -40, -40,
	-40, -40, -40, -40, -40, -40, -40, -40,
	-40, -40, -40, -40, -40, -40, -40, -40,
	-40, -40, -40, -40, -40, -40, -40, -40,
	-20, -20, -20, -20, -20, -20, -20, -20,
	-10,   0,  10, -20,   0, -20,  10,   0
};

int endgame_king_pcsq[64]= {
	 -5,  -5,  -5,  -5,  -5,  -5,  -5,  -5,
	 -5,   0,   0,   0,   0,   0,   0,  -5,
	 -5,   0,   5,   5,   5,   5,   0,  -5,
	 -5,   0,   5,  10,  10,   5,   0,  -5,
	 -5,   0,   5,  10,  10,   5,   0,  -5,
	 -5,   0,   5,   5,   5,   5,   0,  -5,
	 -5,   0,   0,   0,   0,   0,   0,  -5,
	 -5,  -5,  -5,  -5,  -5,  -5,  -5,  -5
};

void init_eval(void)
{
	int i,material,king_sq;

	memset(pcsq,0,sizeof(pcsq));

	/* initialize the no-brainer piece/square tables */
	for(i=0;i<64;i++) {
		pcsq[LIGHT][PAWN][i]=value[PAWN]+pawn_pcsq[i];
		pcsq[LIGHT][KNIGHT][i]=value[KNIGHT]+minor_pcsq[i];
		pcsq[LIGHT][BISHOP][i]=value[BISHOP]+minor_pcsq[i];
		pcsq[LIGHT][ROOK][i]=value[ROOK];
		pcsq[LIGHT][QUEEN][i]=value[QUEEN];
		pcsq[DARK][PAWN][i]=value[PAWN]+pawn_pcsq[flip[i]];
		pcsq[DARK][KNIGHT][i]=value[KNIGHT]+minor_pcsq[flip[i]];
		pcsq[DARK][BISHOP][i]=value[BISHOP]+minor_pcsq[flip[i]];
		pcsq[DARK][ROOK][i]=value[ROOK];
		pcsq[DARK][QUEEN][i]=value[QUEEN];
	}

	/* now scan the board to see how much piece material the
	   enemy has and figure out what side of the board the
	   king is on */
	material=0;
	for(i=0;i<64;i++) {
		if(color[i]==DARK&&piece[i]!=PAWN)
			material+=value[piece[i]];
		if(color[i]==LIGHT&&piece[i]==KING)
			king_sq=i;
	}
	if(material>1400) {  /* use the middlegame tables */
		for(i=0;i<64;i++)
			pcsq[LIGHT][KING][i]=king_pcsq[i];
		if(FILE(king_sq)>=5)
			for(i=0;i<64;i++)
				pcsq[LIGHT][PAWN][i]+=kingside_pawn_pcsq[i];
		if(FILE(king_sq)<=2)
			for(i=0;i<64;i++)
				pcsq[LIGHT][PAWN][i]+=queenside_pawn_pcsq[i];
	}
	else
		for(i=0;i<64;i++)
			pcsq[LIGHT][KING][i]=endgame_king_pcsq[i];

	/* do the same for black */
	material=0;
	for(i=0;i<64;i++) {
		if(color[i]==LIGHT&&piece[i]!=PAWN)
			material+=value[piece[i]];
		if(color[i]==DARK&&piece[i]==KING)
			king_sq=i;
	}
	if(material>1400) {
		for(i=0;i<64;i++)
			pcsq[DARK][KING][i]=king_pcsq[flip[i]];
		if(FILE(king_sq)>=5)
			for(i=0;i<64;i++)
				pcsq[DARK][PAWN][i]+=kingside_pawn_pcsq[flip[i]];
		if(FILE(king_sq)<=2)
			for(i=0;i<64;i++)
				pcsq[DARK][PAWN][i]+=queenside_pawn_pcsq[flip[i]];
	}
	else
		for(i=0;i<64;i++)
			pcsq[DARK][KING][i]=endgame_king_pcsq[flip[i]];
}