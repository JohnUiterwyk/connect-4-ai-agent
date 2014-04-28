/***************************************************************************
**                                                                        **
**                          Connect-4 Algorithm                           **
**                                                                        **
****************************************************************************
**  Adaptation of a Connect4 program with author's permission.            **
***************************************************************************/

#ifndef C4_DEFINED
#define C4_DEFINED

#include <time.h>

#ifndef Boolean
#define Boolean char
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define C4_NONE      2
#define C4_MAX_LEVEL 20

/* Some macros for convenience. */

#define other(x)        ((x) ^ 1)
#define real_player(x)  ((x) & 1)



/* A struct which defines the state of a game. */
typedef struct {

    int width;              /* The width of the board                      */
    int height;             /* Thenum height of the board                     */
    int num_to_connect;     /* The number to connect in order to win       */
   
    char **board;           /* The board configuration of the game state.  */
                            /* board[x][y] specifies the position of the   */
                            /* xth column and the yth row of the board,    */
                            /* where column and row numbering starts at 0. */
                            /* (The 0th row is the bottom row.)            */
                            /* A value of 0 specifies that the position is */
                            /* occupied by a piece owned by player 0, a    */
                            /* value of 1 specifies that the position is   */
                            /* occupied by a piece owned by player 1, and  */
                            /* a value of C4_NONE specifies that the       */
                            /* position is unoccupied.                     */

    int *(score_array[2]);  /* An array specifying statistics on both      */
                            /* players.  score_array[0] specifies the      */
                            /* statistics for player 0, while              */
                            /* score_array[1] specifies the statistics for */
                            /* player 1.                                   */
                            /* Each of score_array[0] and [1] is an array  */
                            /* holding how many pieces the corresponding   */
                            /* player has on each possible winning line as */
                            /* defined in map. Eg: score_array[1][24] = 2  */
                            /* means that player 1 has 2 tokens on line 24 */
                            /* which runs vertically up the left hand side */
                            /* of the board (see map diagram in c4.txt)    */ 
    
    struct agent_struct *agents[2]; /* The computer players of this game   */

    double score[2];        /* The actual scores of each player, as        */
                            /* evaluated by the agent. This field is used  */
                            /* internally by the game and your agent       */
                            /* should not modify it (it is set from        */
                            /* what your agent returns anyway). It does    */
                            /* not contain any information useful to your  */
                            /* agent                                       */

    short int winner;       /* The winner of the game - either 0, 1 or     */
                            /* C4_NONE.  Deducible from score_array, but   */
                            /* kept separately for efficiency.             */

    int num_of_pieces;      /* The number of pieces currently occupying    */
                            /* board spaces.  Deducible from board, but    */
                            /* kept separately for efficiency.             */
    
    int ***map;             /* The winning lines map. See c4.txt for       */
                            /* details                                     */

} Game_state;

/* A struct which defines a computer opponent */
typedef struct agent_struct
{
   char *name;
   double (*agentFunction) (Game_state *current_state, int player, int x, int y);   
} Agent;

int count_agents();
Agent **get_agents();

/* See the file "c4.c" for documentation on the following functions. */

extern void    c4_poll(void (*poll_func)(void), clock_t interval);
extern void    c4_new_game(int width, int height, int num, Agent *agents[]);
extern void    c4_build_map(int ***map, int width, int height, int num_to_connect);
extern Boolean c4_make_move(int player, int column, int *row);
extern Boolean c4_auto_move(int player, int level, int *column, int *row);
extern char ** c4_board(void);
extern int     c4_score_of_player(int player);
extern Boolean c4_is_winner(int player);
extern Boolean c4_is_tie(void);
extern void    c4_end_game(void);
extern void    c4_reset(void);
extern int     num_of_win_places(int width, int height, int num_to_connect);
extern const char *c4_get_version(void);
extern void *emalloc(unsigned int n);

extern void build_map(Game_state *current_state);
extern int game_won(Game_state *current_state);


#endif /* C4_DEFINED */
