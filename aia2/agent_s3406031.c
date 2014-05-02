#include <stdlib.h>
#include "c4.h"

#include <sys/time.h>


/* Connect 4 agent by John N. Uiterwyk
 * all functions and types are prefaced by JNU to avoid name clashing
 * s3406031
 *
 */

#ifndef JNU_ODD
#define JNU_ODD 1
#endif

#ifndef JNU_EVEN
#define JNU_EVEN 0
#endif

typedef struct {
    int player;
    int x;
    int y;
    int is_playable;
    int even_odd;
    
}JNU_Threat;


double agent_s3406031(Game_state *current_state, int player, int x, int y);
double JNU_get_random_double();
double JNU_heuristic_lines(Game_state *current_state, int player, int chip_count);
double JNU_heuristic_playable_threats(Game_state *current_state, int player,int chip_count);
double JNU_heuristic_double_threats(Game_state *current_state, int player, int x, int y);
double JNU_heuristic_enemy_threat_created(Game_state *current_state, int player, int x, int y);
double JNU_heuristic_claim_center(Game_state *current_state, int player, int x, int y);
double JNU_heuristic_defense(Game_state *current_state, int player, int x, int y);
int JNU_count_playable_threats(Game_state *current_state, int player,int chip_count);
int JNU_get_num_lines_with_count(Game_state *current_state,int chip_count);
int JNU_get_total_lines(Game_state *current_state);

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
        /* create a useless threats array:
            it should read the board for 3 long threats
            then figure if the spot below makes a thing for the enemy
            use this to inform the 3x lines counter
         */
        
        
        /* create a "forced win" checker that uses forced play */
        
        
        /* random is included for tie breaking*/
        score += 0.01 * JNU_get_random_double();
        
        score += .15 * JNU_heuristic_claim_center(current_state, player, x ,y);
        /* spots part of alot of lines is better than spots part of few */
        
        /*almost complete player lines are good */
        score += .1 * JNU_heuristic_lines(current_state, player, 1);
        score += .15 * JNU_heuristic_lines(current_state, player, 2);
        score += .6 * JNU_heuristic_lines(current_state, player, 3);
        
        /* playable almost wins  are even better */
        /* score += .1 * JNU_heuristic_playable_threats(current_state, player, 3);*/
        
        /*almost complete enemy lines are bad */
        /*score += -.1 * JNU_heuristic_lines(current_state, other(player), 2);*/
         score += -.1 * JNU_heuristic_lines(current_state, other(player), 3);
         /* playable almost complete enemy lines are very bad */
        /*score += -.1 * JNU_heuristic_playable_threats(current_state, other(player), 2); */
        score += -.9 * JNU_heuristic_playable_threats(current_state, other(player), 3);
        /* dont create threats */
        /* score += -.9 * JNU_heuristic_enemy_threat_created(current_state, player, x ,y); */
        
    }
    return score;
}

double JNU_heuristic_claim_center(Game_state *current_state, int player, int x, int y)
{
    double score = 0;
    if(current_state->board[3][0] == player)
    {
        score +=.7;
    }
    if(current_state->board[3][1] == player  )
    {
        score +=.1;
    }
    if(current_state->board[2][0] == player )
    {
        score +=.1;
    }
    
    if(current_state->board[4][0] == player)
    {
        score +=.1;
    }
    return score;
}
double JNU_heuristic_defense(Game_state *current_state, int player, int x, int y)
{
    double score = 0;
    int *list_of_lines;
    int line_num;
    
    int i=0 ;
    list_of_lines =  current_state->map[x][y];
    /* get list of lines */
    line_num = list_of_lines[0];
    while(line_num != -1)
    {
        score +=current_state->score_array[other(player)][line_num];
        i++;
        line_num = list_of_lines[i];
    }
    return score/(double)i;
}

double JNU_heuristic_lines(Game_state *current_state, int player, int chip_count)
{
    int i;
    int lines = 0;
    int possible = JNU_get_total_lines(current_state);
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

double JNU_heuristic_enemy_threat_created(Game_state *current_state, int player, int x, int y)
{
    double score = 0;
    int *list_of_lines;
    int line_num;
    
    int i=0 ;
    if(y < current_state->height-1)
    {
        list_of_lines =  current_state->map[x][y+1];
        /* get list of lines */
        line_num = list_of_lines[0];
        while(line_num != -1)
        {
            if(current_state->score_array[other(player)][line_num] == 3)
            {
                score = 1;
            }
            i++;
            line_num = list_of_lines[i];
        }
    }
    return score;
}

double JNU_heuristic_playable_threats(Game_state *current_state, int player, int chip_count)
{
    int threats = JNU_count_playable_threats(current_state, player, chip_count);
    if(threats == 1)
    {
        return 0.5;
    }else if(threats > 1){
        return 1;
    }else
    {
        return 0;
    }
}

/*
JNU_get_threats(Game_state *current_state, int player, int x, int y)
{
    
}


int JNU_player_has_zugzwang(Game_state *current_state, int player)
{
    
 }
 */

/* playable threats are lines for a player that can be completed next turn */
int JNU_count_playable_threats(Game_state *current_state, int player, int chip_count)
{
    int threats = 0;
    int i,k;
    int top;
    int *list_of_lines;
    int line_num;
    
    /* cycle through all columns */
    for(i = 0; i<current_state->width; i++)
    {
        /* find the row top */
        top = 0;
        while(top < current_state->height
              && current_state->board[i][top] != C4_NONE)
        {
            top++;
        };
            
        
        if(current_state->board[i][top] == C4_NONE)
        {
            /* get list of lines */
            list_of_lines = current_state->map[i][top];
            line_num = list_of_lines[0];
            k=0;
            while(line_num != -1)
            {
                if(current_state->score_array[player][line_num] == chip_count)
                {
                    threats += 1;
                }
                k++;
                line_num = list_of_lines[k];
            }
        }
        
    }
    return threats;
}

/*
 * this creates an array of the lines of 3 chip threats
 * this fucntion excludes useless threats that are not playable sincefor
 */
int JNU_get_threats(Game_state *current_state, int player, int chip_count)
{
    
}

int JNU_get_total_lines(Game_state *current_state)
{
    return num_of_win_places(current_state->width,
                      current_state->height,
                      current_state->num_to_connect);
}

double JNU_get_random_double()
{
    return (double) rand() / (RAND_MAX / 2) - 1;
}
