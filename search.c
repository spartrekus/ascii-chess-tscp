#include <stdio.h>
#include <string.h>
#include "defs.h"
#include "data.h"
#include "protos.h"


/* sort_pv() is called when the search function is following
   the PV (Principal Variation). It looks through the current
   ply's move list to see if the PV move is there. If so,
   it adds 10,000,000 to the move's score so it's played first
   by the search function. If not, follow_pv remains FALSE and
   search() stops calling sort_pv(). */

void sort_pv(void)
{
	int i;

	follow_pv=FALSE;
	for(i=gen_begin[ply];i<gen_end[ply];i++)
		if(gen_dat[i].m.i==pv[0][ply].i) {
			follow_pv=TRUE;
			gen_dat[i].score+=10000000;
			return;
		}
}


/* sort() searches the current ply's move list from 'from'
   to the end to find the move with the highest score. Then it
   swaps that move and the 'from' move so the move with the
   highest score gets searched next, and hopefully produces
   a cutoff. */

void sort(int from)
{
	int i;
	int bs;  /* best score */
	int bi;  /* best i */
	gen_rec g;

	bs=-1;
	for(i=from;i<gen_end[ply];i++)
		if(gen_dat[i].score>bs) {
			bs=gen_dat[i].score;
			bi=i;
		}
	g=gen_dat[from];
	gen_dat[from]=gen_dat[bi];
	gen_dat[bi]=g;
}


/* quiesce() is a recursive minimax search function with
   alpha-beta cutoffs. In other words, negamax. It basically
   only searches capture sequences and allows the evaluation
   function to cut the search off (and set alpha). The idea
   is to find a position where there isn't a lot going on
   so the static evaluation function will work. */

int quiesce(int alpha,int beta)
{
	int i,j,x;
	BOOL c;  /* in check */
	BOOL f;  /* legal move found */

	nodes++;
	pv_length[ply]=ply;
	c=in_check(side);

	/* if we're in check, try all moves to get out. (not
	   necessary, just something I decided to do) */
	if(c)
		gen();

	/* otherwise, use the evaluation function. */
	else {
		x=eval();
		if(x>=beta)
			return(beta);
		if(x>alpha)
			alpha=x;
		gen_caps();
	}
	if(follow_pv)  /* are we following the PV? */
		sort_pv();
	f=FALSE;

	/* loop through the moves */
	for(i=gen_begin[ply];i<gen_end[ply];i++) {
		sort(i);
		if(!makemove(gen_dat[i].m.b))
			continue;
		f=TRUE;  /* we found a legal move! */
		x=-quiesce(-beta,-alpha);
		takeback();
		if(x>alpha) {
			if(x>=beta)
				return(beta);
			alpha=x;

			/* update the PV */
			pv[ply][ply]=gen_dat[i].m;
			for(j=ply+1;j<pv_length[ply+1];j++)
				pv[ply][j]=pv[ply+1][j];
			pv_length[ply]=pv_length[ply+1];
		}
	}

	/* if we're in check and there aren't any legal moves,
	   well, we lost */
	if((!f)&&c)
		return(-10000+ply);
	return(alpha);
}


/* search() does just that, in negamax fashion */

int search(int alpha,int beta,int depth)
{
	int i,j,x;
	BOOL c,f;
	
	/* we're as deep as we want to be; call quiesce() to get
	   a reasonable score and return it. */
	if(!depth)
		return(quiesce(alpha,beta));
	nodes++;
	pv_length[ply]=ply;

	/* are we in check? if so, we want to search deeper */
	c=in_check(side);
	if(c)
		depth++;
	gen();
	if(follow_pv)  /* are we following the PV? */
		sort_pv();
	f=FALSE;

	/* loop through the moves */
	for(i=gen_begin[ply];i<gen_end[ply];i++) {
		sort(i);
		if(!makemove(gen_dat[i].m.b))
			continue;
		f=TRUE;
		x=-search(-beta,-alpha,depth-1);
		takeback();
		if(x>alpha) {

			/* this move caused a cutoff, so increase the history
			   value so it gets ordered high next time we can
			   search it */
			history[gen_dat[i].m.b.from][gen_dat[i].m.b.to]+=depth;
			if(x>=beta)
				return(beta);
			alpha=x;

			/* update the PV */
			pv[ply][ply]=gen_dat[i].m;
			for(j=ply+1;j<pv_length[ply+1];j++)
				pv[ply][j]=pv[ply+1][j];
			pv_length[ply]=pv_length[ply+1];
		}
	}

	/* no legal moves? then we're in checkmate or stalemate */
	if(!f) {
		if(c)
			return(-10000+ply);
		else
			return(0);
	}

	/* fifty move draw rule */
	if(fifty>100)
		return(0);
	return(alpha);
}


/* think() calls search() iteratively and prints the results
   after every iteration. */

void think(void)
{
	int i,j,x;

	ply=0;
	nodes=0;
	init_eval();
	memset(history,0,sizeof(history));
	printf("ply      nodes  score  pv\n");
	for(i=1;i<=4;i++) {
		follow_pv=TRUE;
		x=search(-10000,10000,i);

		printf("%3d  %9d  %5d ",i,nodes,x);
		for(j=0;j<pv_length[0];j++)
			printf(" %s", movestr(&pv[0][j].b));
		printf("\n");
	}

	printf("1 ... %s\n", movestr(&pv[0][0].b));
}
