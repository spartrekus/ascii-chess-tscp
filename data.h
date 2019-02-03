/* this is basically a copy of data.c that's included by most
   of the source files so they can use the data.c variables */

extern int color[64];
extern int piece[64];
extern int side;
extern int xside;
extern int castle;
extern int ep;
extern int fifty;
extern int ply;
extern gen_rec gen_dat[MOVE_STACK];
extern int gen_begin[HIST_STACK],gen_end[HIST_STACK];
extern int history[64][64];
extern hist_rec hist_dat[HIST_STACK];
extern int nodes;
extern move pv[HIST_STACK][HIST_STACK];
extern int pv_length[HIST_STACK];
extern BOOL follow_pv;
extern int mailbox[120];
extern int mailbox64[64];
extern BOOL slide[6];
extern int offsets[6];
extern int offset[6][8];
extern int castle_mask[64];
extern int value[6];
extern char piece_char[6];
extern int init_color[64];
extern int init_piece[64];