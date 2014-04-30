#include <stdlib.h>
#include "c4.h"


/* Connect 4 agent by John Uiterwyk
 * s3406031
 *
 */

double agent_s3406031(Game_state *current_state, int player, int x, int y);
double jnu_get_random_double();
double jnu_heuristic_start(Game_state *current_state, int player, int x, int y);



double agent_s3406031(Game_state *current_state, int player, int x, int y)
{
    double score = 0;
    int is_game_won = game_won(current_state);
    
    if (is_game_won == player)
    {
        score = 1;
    }
    else if (is_game_won == other(player))
    {
        score = -1;
    }
    else
    {
        /* random is included for tie breaking*/
        score += 0.01 * agent_random(current_state, player, x, y);
        score += 0.1 * jnu_heuristic_start
        /* score += 0.3 * super_cool_heuristic_a(current_state, player, x, y) */
        /* score += 0.8 * super_cool_heuristic_b(current_state, player, x, y) */
    }
    
    return score;
}

double jnu_heuristic_start(Game_state *current_state, int player, int x, int y)
{
    double score = 0;
    if(current_state->num_of_pieces == 1)
    {
        
    }
}

double jnu_get_random_double()
{
    return (double) rand() / (RAND_MAX / 2) - 1;
}