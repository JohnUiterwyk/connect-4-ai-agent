#include <stdlib.h>
#include "c4.h"

#include <sys/time.h>


/* Connect 4 agent by John Uiterwyk
 * s3406031
 *
 */

double agent_s3406031(Game_state *current_state, int player, int x, int y);
double jnu_get_random_double();
double jnu_heuristic_lines(Game_state *current_state, int player, int chip_count);
double jnu_heuristic_playable_threats(Game_state *current_state, int player, int x, int y);
int jnu_get_num_lines_with_count(Game_state *current_state,int chip_count);
int jnu_get_total_lines(Game_state *current_state);


/* init_random_seed() is a fix for faulty test_agent script 
 * as per blackboard post byJames Alan NGUYEN
 Test-script-proof solution
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
    
    /*
     * We asses the board from the perspective of the player who played last
     */
    int winner = game_won(current_state);
    
    /* reset random seed */
    init_random_seed();
    
    if (winner == player)
    {
        score = 1;
    }
    else if (winner == other(player))
    {
        score = -1;
    }
    else
    {
        
        /* random is included for tie breaking*/
        score += 0.01 * jnu_get_random_double();
        
        /*almost complete player lines are good */
        score += .2 * jnu_heuristic_lines(current_state, player, 2);
        score += .3 * jnu_heuristic_lines(current_state, player, 3);
        
        /*almost complete enemy lines are very! bad */
        score += -.5 * jnu_heuristic_lines(current_state, other(player), 3);
        score += -.3 * jnu_heuristic_lines(current_state, other(player), 2);
    }
    return score;
}

double jnu_heuristic_lines(Game_state *current_state, int player, int chip_count)
{
    int i;
    int lines = 0;
    int possible = jnu_get_total_lines(current_state);
    int enemy = other(player);
    for(i =0;i<possible;i++)
    {
        if(current_state->score_array[player][i] == chip_count
           && current_state->score_array[enemy][i] == 0)
        {
            lines++;
        }
    }
    return (double)lines / (double)possible;
    
}
/* playable threats are threats that the enemy could play next turn */
double jnu_heuristic_playable_threats(Game_state *current_state, int player, int x, int y)
{
    int i,j;
    int lines = 0;
    int possible = jnu_get_total_lines(current_state);
    int enemy = other(player);
    int playable;
    int top;
    
    /* cycle through all columns */
    for(i = 0; i<current_state->width; i++)
    {
        /* find the row top */
        j = 0;
        while(current_state->board[i][j] != C4_NONE && j < current_state->height)
        {
            j++;
        }
        top = j;
        
        
        /* get list of lines */
        
    }
    /* check if it is a playable for win next chip */
    for(i =0;i<possible;i++)
    {
        /* find uncontested lines */
        if(current_state->score_array[enemy][i] == 3
           && current_state->score_array[player][i] == 0)
        {
            lines++;
        }
    }
}
int jnu_get_total_lines(Game_state *current_state)
{
    return num_of_win_places(current_state->width,
                      current_state->height,
                      current_state->num_to_connect);
}

double jnu_get_random_double()
{
    return (double) rand() / (RAND_MAX / 2) - 1;
}
