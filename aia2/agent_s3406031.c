/***************************************************************************
 **                                                                        **
 **                          Connect-4 Agent                               **
 **                          RMIT AI Assignment 2                          **
 **                                                                        **
 **                          John ;                                 **
 **                          s3406031                                      **
 **                                                                        **
 **                                                                        **
 ****************************************************************************
 ***************************************************************************/


#include <stdlib.h>
#include <sys/time.h>
#include "c4.h"


/* JNU_EVEN and JNU_ODD  defines used in checking if a row is even/odd */
#ifndef JNU_ODD
#define JNU_ODD 0
#endif

#ifndef JNU_EVEN
#define JNU_EVEN 1
#endif

/* JNU_Threat is a struct that stores info about a threat on the board */
typedef struct {
    int x;
    int y;
    int is_playable;
    int even_odd;
    
}JNU_Threat;

typedef struct{
    JNU_Threat * threats;
    int even_threat_count;
    int odd_threat_count;
    int total_threat_count;
    int playable_threat_count;
}Player_Threats;



double  agent_s3406031(Game_state *current_state,
                       int player,
                       int x,
                       int y);

double  JNU_heuristic_claim_lines(Game_state *current_state,
                                  int player);

double  JNU_heuristic_unblocked_lines(Game_state *current_state,
                                      int player,
                                      int chip_count);

void    JNU_get_threats(Game_state *current_state,
                        int player,
                        Player_Threats * player_threats);

int     JNU_smart_win_checker(Game_state *current_state,
                          int player,
                          Player_Threats player_threats[2]);

int JNU_check_forcable_win(Game_state *current_state,
                            Player_Threats player_threats[2]);

int     JNU_total_lines;

double  JNU_get_random_double();

/* init_random_seed() is a fix for faulty test_agent script 
 * as per blackboard post by James Alan NGUYEN
 * titled Test-script-proof solution
 */
void init_random_seed()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    srand(tv.tv_usec);
}

double agent_s3406031(Game_state *current_state, int player, int x, int y)
{
    double score = 0;
    int winner;
    int enemy;
    Player_Threats player_threats[2];
    
    enemy = other(player);
    
    /* populate threat arrays */
    JNU_get_threats(current_state,player,&player_threats[player]);
    JNU_get_threats(current_state,enemy,&player_threats[enemy]);
    
    /* save total possible line count */
    JNU_total_lines = num_of_win_places(current_state->width,
                                        current_state->height,
                                        current_state->num_to_connect);
    
    /* check to see if we can see a garunteed win ahead */
    winner = JNU_smart_win_checker(current_state,player,player_threats);
    
    /* reset random seed */
    init_random_seed();
    
    if (winner == player)
    {
        score = 1;
    }
    else if (winner == enemy)
    {
        score = -1;
    }
    else
    {
        
        /* more lines has a little value, helps claim the centre early on */
        score += .05 * JNU_heuristic_claim_lines(current_state, player);
        
        /*  having unblocked lines of length 2 has some value */
        score += .2 * JNU_heuristic_unblocked_lines(current_state, player, 2);
        
        /*  having unblocked lines of length 3 has more value */
        score += .5 * JNU_heuristic_unblocked_lines(current_state, player, 3);
        
        /*  having enemy unblocked lines of length 3 is bad */
        score += -.1 * JNU_heuristic_unblocked_lines(current_state, enemy, 3);
        
        /* player 0 is more likely to finish odd threats, player 1 even. */
        if(player == 0)
        {
            score += .1 * player_threats[0].odd_threat_count / JNU_total_lines;
        }else if(player == 1)
        {
            score += .1 * player_threats[1].even_threat_count / JNU_total_lines;
            
        }
        
        
        
        /* random is included for tie breaking*/
        score += 0.0001 * JNU_get_random_double();
        
    }
    
    /* free the threat arrays */
    free(player_threats[0].threats);
    free(player_threats[1].threats);
    return score;
}



/* claim lines values having a chip in as many lines as possible 
 * this is used early on to increase the possibilities for creating 
 * winning lines. It also reduces the number of options for the enemy;
 * this has the effect of prefering center squares over outer squares
 */
double JNU_heuristic_claim_lines(Game_state *current_state, int player)
{
    int i;
    int lines = 0;
    for(i =0;i<JNU_total_lines;i++)
    {
        if(current_state->score_array[player][i] > 0)
        {
            lines++;
        }
    }
    return (double)lines / (double)JNU_total_lines;
}

/*
 * heuristic_lines counts the lines of length 'chip_count'
 */
double JNU_heuristic_unblocked_lines(Game_state *current_state,
                                     int player,
                                     int chip_count)
{
    int i;
    int lines = 0;
    int enemy = other(player);
    for(i =0;i<JNU_total_lines;i++)
    {
        if(current_state->score_array[player][i] == chip_count
           && current_state->score_array[enemy][i] == 0)
        {
            lines++;
        }
    }
    return (double)lines / (double)JNU_total_lines;
    
}

/* get_threats looks at the board and builds arrays of threats
 * a threat is a square that completes a line
 */
void JNU_get_threats(Game_state *current_state,int player, Player_Threats * player_threats)
{
    int i,j,k;
    int *list_of_lines;
    int line_num;
    int total_threat_count = 0;
    int even_threat_count = 0;
    int odd_threat_count = 0;
    int playable_threat_count = 0;
    JNU_Threat * threat;
    
    player_threats->threats = (JNU_Threat *)emalloc(current_state->width
                                                      * current_state->height
                                                      * sizeof(JNU_Threat));
    for(i=0;i<current_state->width;i++)
    {
        for (j=0; j<current_state->height; j++)
        {
            /* find empty spots */
            if(current_state->board[i][j] == C4_NONE)
            {
                /* get lines for spot */
                list_of_lines =current_state->map[i][j];
                k = 0;
                line_num = list_of_lines[k];
                while(line_num != -1)
                {
                    /* check each player for a threat line at the spot */
                    if(current_state->score_array[player][line_num] == 3)
                    {
                        threat = &player_threats->threats[total_threat_count];
                        threat->x = i;
                        threat->y = j;
                        threat->even_odd = j%2;
                        if(threat->even_odd == JNU_EVEN)
                        {
                            even_threat_count++;
                        }else
                        {
                            odd_threat_count++;
                        }
                        if(j == 0)
                        {
                            playable_threat_count++;
                            threat->is_playable = TRUE;
                        }else if(current_state->board[i][j-1] != C4_NONE)
                        {
                            playable_threat_count++;
                            threat->is_playable = TRUE;
                        }else
                        {
                            threat->is_playable = FALSE;
                        }
                        total_threat_count++;
                        break;
                    }
                    k++;
                    line_num = list_of_lines[k];
                    
                }
                
                
            }
        }
    }
                                                
    player_threats->total_threat_count = total_threat_count;
    player_threats->even_threat_count = even_threat_count;
    player_threats->odd_threat_count = odd_threat_count;
    player_threats->playable_threat_count = playable_threat_count;
}

/* JNU_smart_win_checker looks for game states that are certain to 
 * lead to a win for one player or the other
 */

int JNU_smart_win_checker(Game_state *current_state,
                          int player,
                          Player_Threats player_threats[2])
{
    /* first check for 4 in a row */
    int winner = game_won(current_state);
    int enemy = other(player);
    
    if(winner == C4_NONE)
    {
        /*check if the enemy has a playable win next turn */
        if(player_threats[enemy].playable_threat_count >0)
        {
            winner = enemy;
        }
        
        /* check if player has two playable threats and the enemy has 0 */
        if(player_threats[player].playable_threat_count >1
           && player_threats[enemy].playable_threat_count ==0)
        {
            winner = player;
        }
        
        
    }
    if(winner == C4_NONE)
    {
        winner =JNU_check_forcable_win(current_state,player_threats);
    }
    /* check for forceable win */
    return winner;
    
    
}

/* JNU_check_forcable_win looks for wins that can be forced near the end
 * of the game. This could be expanded upon to further use forced play.
 */
int JNU_check_forcable_win(Game_state *current_state,
                            Player_Threats player_threats[2])
{
    int winner;
    int * empty_squares;
    int full_columns = 0;
    int i,j;
    
    winner = C4_NONE;
    
    empty_squares = (int *)emalloc(current_state->width * sizeof(int));
    /* count up empty squares in each column */
    for(i=0;i<current_state->width;i++)
    {
        for(j=0;j<current_state->height;j++)
        {
            if(current_state->board[i][j]== C4_NONE)
            {
                empty_squares[i]++;
            }
        }
        if(empty_squares[i] == 0)
        {
            full_columns++;
        }
    }
    
    /* if exactly one column is not empty
     * player 0 will finish all odd threats
     * player 1 will finish all even threats
     */
    if(full_columns == current_state->width-1)
    {
        if(player_threats[0].odd_threat_count > 0
           && player_threats[1].total_threat_count == 0)
        {
            winner = 0;
        }
        if(player_threats[1].even_threat_count > 0
           && player_threats[0].total_threat_count == 0)
        {
            winner = 1;
        }
    }
    free(empty_squares);
    return winner;
}

/* get a random number between 1 and -1 */
double JNU_get_random_double()
{
    return (double) rand() / (RAND_MAX / 2) - 1;
}
