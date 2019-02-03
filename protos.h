/* prototypes */

/* board.c */
void init(void);
BOOL in_check(int s);
BOOL attack(int sq,int s);
void gen(void);
void gen_caps(void);
void gen_push(int from,int to,int bits);
void gen_promote(int from,int to,int bits);
BOOL makemove(move_bytes m);
void takeback(void);

/* search.c */
void sort_pv(void);
void sort(int from);
int quiesce(int alpha,int beta);
int search(int alpha,int beta,int depth);
void think(void);

/* eval.c */
int eval(void);
void init_eval(void);

/* main.c */
void main(void);
void print_board(void);

/* parse.c */
char *movestr(move_bytes *m);
int parse_move(char *s);
