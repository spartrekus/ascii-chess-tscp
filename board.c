#include "defs.h"
#include "data.h"
#include "protos.h"


/* init() sets the board to the initial game state */

void init(void)
{
	int i;

	for(i=0;i<64;i++) {
		color[i]=init_color[i];
		piece[i]=init_piece[i];
	}
	side=LIGHT;
	xside=DARK;
	castle=15;
	ep=-1;
	fifty=0;
	ply=0;
	gen_begin[0]=0;
}


/* in_check() returns TRUE if side s is in check and FALSE
   otherwise. It just scans the board to find side s's king
   and calls attack() to see if it's being attacked. */

BOOL in_check(int s)
{
	int i;

	for(i=0;i<64;i++)
		if(color[i]==s&&piece[i]==KING)
			return(attack(i,s^1));
}


/* attack returns TRUE if square sq is being attacked by side
   s and FALSE otherwise. */

BOOL attack(int sq,int s)
{
	int i,j,n;

	for(i=0;i<64;i++)
		if(color[i]==s) {
			if(piece[i]==PAWN) {
				if(s==LIGHT) {
					if(FILE(i)!=0&&i-9==sq)
						return(TRUE);
					if(FILE(i)!=7&&i-7==sq)
						return(TRUE);
				}
				else {
					if(FILE(i)!=0&&i+7==sq)
						return(TRUE);
					if(FILE(i)!=7&&i+9==sq)
						return(TRUE);
				}
			}
			else
				for(j=0;j<offsets[piece[i]];j++)
					for(n=i;;) {
						n=mailbox[mailbox64[n]+offset[piece[i]][j]];
						if(n==-1)
							break;
						if(n==sq)
							return(TRUE);
						if(color[n]!=EMPTY)
							break;
						if(!slide[piece[i]])
							break;
					}
		}
	return(FALSE);
}


/* gen() generates pseudo-legal moves for the current position.
   It scans the board to find friendly pieces and then determines
   what squares they attack. When it finds a piece/square
   combination, it calls gen_push to put the move on the "move
   stack." */

void gen(void)
{
	int i,j,n;

	/* so far, we have no moves for the current ply */
	gen_end[ply]=gen_begin[ply];
	for(i=0;i<64;i++)
		if(color[i]==side) {
			if(piece[i]==PAWN) {
				if(side==LIGHT) {
					if(FILE(i)!=0&&color[i-9]==DARK)
						gen_push(i,i-9,17);
					if(FILE(i)!=7&&color[i-7]==DARK)
						gen_push(i,i-7,17);
					if(color[i-8]==EMPTY) {
						gen_push(i,i-8,16);
						if(i>=48&&color[i-16]==EMPTY)
							gen_push(i,i-16,24);
					}
				}
				else {
					if(FILE(i)!=0&&color[i+7]==LIGHT)
						gen_push(i,i+7,17);
					if(FILE(i)!=7&&color[i+9]==LIGHT)
						gen_push(i,i+9,17);
					if(color[i+8]==EMPTY) {
						gen_push(i,i+8,16);
						if(i<=15&&color[i+16]==EMPTY)
							gen_push(i,i+16,24);
					}
				}
			}
			else
				for(j=0;j<offsets[piece[i]];j++)
					for(n=i;;) {
						n=mailbox[mailbox64[n]+offset[piece[i]][j]];
						if(n==-1)
							break;
						if(color[n]!=EMPTY) {
							if(color[n]==xside)
								gen_push(i,n,1);
							break;
						}
						gen_push(i,n,0);
						if(!slide[piece[i]])
							break;
					}
		}

	/* generate castle moves */
	if(side==LIGHT) {
		if(castle&1)
			gen_push(60,62,2);
		if(castle&2)
			gen_push(60,58,2);
	}
	else {
		if(castle&4)
			gen_push(4,6,2);
		if(castle&8)
			gen_push(4,2,2);
	}
	
	/* generate en passant moves */
	if(ep!=-1) {
		if(side==LIGHT) {
			if(FILE(ep)!=0&&color[ep+7]==LIGHT&&piece[ep+7]==PAWN)
				gen_push(ep+7,ep,21);
			if(FILE(ep)!=7&&color[ep+9]==LIGHT&&piece[ep+9]==PAWN)
				gen_push(ep+9,ep,21);
		}
		else {
			if(FILE(ep)!=0&&color[ep-9]==DARK&&piece[ep-9]==PAWN)
				gen_push(ep-9,ep,21);
			if(FILE(ep)!=7&&color[ep-7]==DARK&&piece[ep-7]==PAWN)
				gen_push(ep-7,ep,21);
		}
	}

	/* the next ply's moves need to start where the current
	   ply's end */
	gen_begin[ply+1]=gen_end[ply];
}


/* gen_caps() is basically a copy of gen() that's modified to
   only generate capture and promote moves. It's used by the
   quiescence search. */

void gen_caps(void)
{
	int i,j,n;

	gen_end[ply]=gen_begin[ply];
	for(i=0;i<64;i++)
		if(color[i]==side) {
			if(piece[i]==PAWN) {
				if(side==LIGHT) {
					if(FILE(i)!=0&&color[i-9]==DARK)
						gen_push(i,i-9,17);
					if(FILE(i)!=7&&color[i-7]==DARK)
						gen_push(i,i-7,17);
					if(i<=15&&color[i-8]==EMPTY)
						gen_push(i,i-8,16);
				}
				if(side==DARK) {
					if(FILE(i)!=0&&color[i+7]==LIGHT)
						gen_push(i,i+7,17);
					if(FILE(i)!=7&&color[i+9]==LIGHT)
						gen_push(i,i+9,17);
					if(i>=48&&color[i+8]==EMPTY)
						gen_push(i,i+8,16);
				}
			}
			else
				for(j=0;j<offsets[piece[i]];j++)
					for(n=i;;) {
						n=mailbox[mailbox64[n]+offset[piece[i]][j]];
						if(n==-1)
							break;
						if(color[n]!=EMPTY) {
							if(color[n]==xside)
								gen_push(i,n,1);
							break;
						}
						if(!slide[piece[i]])
							break;
					}
		}
	if(ep!=-1) {
		if(side==LIGHT) {
			if(FILE(ep)!=0&&color[ep+7]==LIGHT&&piece[ep+7]==PAWN)
				gen_push(ep+7,ep,21);
			if(FILE(ep)!=7&&color[ep+9]==LIGHT&&piece[ep+9]==PAWN)
				gen_push(ep+9,ep,21);
		}
		else {
			if(FILE(ep)!=0&&color[ep-9]==DARK&&piece[ep-9]==PAWN)
				gen_push(ep-9,ep,21);
			if(FILE(ep)!=7&&color[ep-7]==DARK&&piece[ep-7]==PAWN)
				gen_push(ep-7,ep,21);
		}
	}
	gen_begin[ply+1]=gen_end[ply];
}


/* gen_push() puts a move on the move stack, unless it's a
   pawn promotion that needs to be handled by gen_promote().
   It also assigns a score to the move for alpha-beta move
   ordering. If the move is a capture, it uses MVV/LVA
   (Most Valuable Victim/Least Valuable Attacker). Otherwise,
   it uses the move's history heuristic value. Note that
   1,000,000 is added to a capture move's score, so it
   always gets ordered above a "normal" move. */

void gen_push(int from,int to,int bits)
{
	gen_rec *g;
	
	if(bits&16) {
		if(side==LIGHT) {
			if(to<=8) {
				gen_promote(from,to,bits);
				return;
			}
		}
		else {
			if(to>=56) {
				gen_promote(from,to,bits);
				return;
			}
		}
	}
	g=&gen_dat[gen_end[ply]];
	gen_end[ply]++;
	g->m.b.from=from;
	g->m.b.to=to;
	g->m.b.promote=0;
	g->m.b.bits=bits;
	if(bits&1)
		g->score=1000000+(piece[to]*10)-piece[from];
	else
		g->score=history[from][to];
}


/* gen_promote() is just like gen_push(), only it puts 4 moves
   on the move stack, one for each possible promotion piece */

void gen_promote(int from,int to,int bits)
{
	int i;
	gen_rec *g;
	
	for(i=KNIGHT;i<=QUEEN;i++) {
		g=&gen_dat[gen_end[ply]];
		gen_end[ply]++;
		g->m.b.from=from;
		g->m.b.to=to;
		g->m.b.promote=i;
		g->m.b.bits=bits|32;
		g->score=1000000+(i*10);
	}
}


/* makemove() makes a move. If the move is illegal, it
   undoes whatever it did and returns FALSE. Otherwise, it
   returns TRUE. */

BOOL makemove(move_bytes m)
{
	
	/* test to see if a castle move is legal and move the rook
	   (the king is moved with the usual move code later) */
	if(m.bits&2) {
		if(in_check(side))
			return(FALSE);
		switch(m.to) {
			case 62:
				if(color[61]!=EMPTY||color[62]!=EMPTY||
						attack(61,xside)||attack(62,xside))
					return(FALSE);
				color[61]=LIGHT;
				piece[61]=ROOK;
				color[63]=EMPTY;
				piece[63]=EMPTY;
				break;
			case 58:
				if(color[57]!=EMPTY||color[58]!=EMPTY||color[59]!=EMPTY||
						attack(58,xside)||attack(59,xside))
					return(FALSE);
				color[59]=LIGHT;
				piece[59]=ROOK;
				color[56]=EMPTY;
				piece[56]=EMPTY;
				break;
			case 6:
				if(color[5]!=EMPTY||color[6]!=EMPTY||
						attack(5,xside)||attack(6,xside))
					return(FALSE);
				color[5]=DARK;
				piece[5]=ROOK;
				color[7]=EMPTY;
				piece[7]=EMPTY;
				break;
			case 2:
				if(color[1]!=EMPTY||color[2]!=EMPTY||color[3]!=EMPTY||
						attack(2,xside)||attack(3,xside))
					return(FALSE);
				color[3]=DARK;
				piece[3]=ROOK;
				color[0]=EMPTY;
				piece[0]=EMPTY;
				break;
		}
	}

	/* back up information so we can take the move back later. */
	hist_dat[ply].m.b=m;
	hist_dat[ply].capture=piece[m.to];
	hist_dat[ply].castle=castle;
	hist_dat[ply].ep=ep;
	hist_dat[ply].fifty=fifty;
	ply++;

	/* update the castle, en passant, and
	   fifty-move-draw variables */
	castle&=castle_mask[m.from]&castle_mask[m.to];
	if(m.bits&8) {
		if(side==LIGHT)
			ep=m.to+8;
		else
			ep=m.to-8;
	}
	else
		ep=-1;
	if(m.bits&17)
		fifty=0;
	else
		fifty++;

	/* move the piece */
	color[m.to]=side;
	if(m.bits&32)
		piece[m.to]=m.promote;
	else
		piece[m.to]=piece[m.from];
	color[m.from]=EMPTY;
	piece[m.from]=EMPTY;

	/* erase the pawn if this is an en passant move */
	if(m.bits&4) {
		if(side==LIGHT) {
			color[m.to+8]=EMPTY;
			piece[m.to+8]=EMPTY;
		}
		else {
			color[m.to-8]=EMPTY;
			piece[m.to-8]=EMPTY;
		}
	}

	/* switch sides and test for legality (if we can capture
	   the other guy's king, it's an illegal position and
	   we need to take the move back) */
	side^=1;
	xside^=1;
	if(in_check(xside)) {
		takeback();
		return(FALSE);
	}
	return(TRUE);
}


/* takeback() is very similar to makemove(), only backwards :)  */

void takeback(void)
{
	move_bytes m;

	side^=1;
	xside^=1;
	ply--;
	m=hist_dat[ply].m.b;
	castle=hist_dat[ply].castle;
	ep=hist_dat[ply].ep;
	fifty=hist_dat[ply].fifty;
	color[m.from]=side;
	if(m.bits&32)
		piece[m.from]=PAWN;
	else
		piece[m.from]=piece[m.to];
	if(hist_dat[ply].capture==EMPTY) {
		color[m.to]=EMPTY;
		piece[m.to]=EMPTY;
	}
	else {
		color[m.to]=xside;
		piece[m.to]=hist_dat[ply].capture;
	}
	if(m.bits&2) {
		int from,to;

		switch(m.to) {
			case 62: from=61; to=63; break;
			case 58: from=59; to=56; break;
			case 6: from=5; to=7; break;
			case 2: from=3; to=0; break;
		}
		color[to]=side;
		piece[to]=ROOK;
		color[from]=EMPTY;
		piece[from]=EMPTY;
	}
	if(m.bits&4) {
		if(side==LIGHT) {
			color[m.to+8]=xside;
			piece[m.to+8]=PAWN;
		}
		else {
			color[m.to-8]=xside;
			piece[m.to-8]=PAWN;
		}
	}
}