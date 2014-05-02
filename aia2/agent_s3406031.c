#include <stdlib.h>
#include "c4.h"

#include <sys/time.h>


/* Connect 4 agent by John N. Uiterwyk
 * all functions and types are prefaced by JNU to avoid name clashing
 * s3406031
 *
 */

#ifndef JNU_ODD
#define JNU_ODD 0
#endif

#ifndef JNU_EVEN
#define JNU_EVEN 1
#endif

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


double JNU_heuristic_zugzwang(Game_state *current_state,
                              int player,
                              Player_Threats players[2])
{
    int i,j;
    double score = 0;
    int possible_winner = C4_NONE;

    JNU_Threat * p0_threat;
    JNU_Threat * p1_threat;

    int p0_solid_odd_threats = 0;
    int p1_even_threat_below = 0;
    int p1_odd_other_threats = 0;
    
    /* if player 0 has an odd threat and no even enemy threats below
     * and no enemy odd other columns
     * this is due to there always being an even number of moves left for 
     * player 0 (i.e. before first move there are 42 squares,
     * then 40 after enemy move)
     */
    
    /* first find an odd threat with no even enemy threats below */
    for(i=0;i<players[0].total_threat_count;i++)
    {
        p0_threat = &players[0].threats[i];
        p1_even_threat_below = 0;
        p1_odd_other_threats = 0;
        if(p0_threat->even_odd == JNU_ODD)
        {
            
            for(j=0;j<players[1].total_threat_count;j++)
            {
                p1_threat = &players[1].threats[j];
                /* if same column and below */
                if(p0_threat->x == p1_threat->x
                   && p0_threat->y > p1_threat->y
                   && p1_threat->even_odd == JNU_EVEN)
                {
                    p1_even_threat_below++;
                }else if(p0_threat->x != p1_threat->x
                         && p1_threat->even_odd == JNU_ODD)
                {
                    p1_odd_other_threats++;
                }
                
            }
            if(p1_even_threat_below == 0)
            {
                p0_solid_odd_threats++;
            }
            if(p1_even_threat_below == 0 && p1_odd_other_threats ==0)
            {
                possible_winner = 0;
                break;
            }
        }
    }
    
    /* if player 0 has more odd threats (with no even enemy below)
     * than player 1's total odd threats, and player 1 has no even threats
     */
    if(possible_winner == C4_NONE
       && p0_solid_odd_threats > players[1].odd_threat_count
       && players[1].even_threat_count == 0)
    {
        possible_winner = 0;
    }
    
    /* other wise, if player 1 has any even threats */
    if(possible_winner == C4_NONE && players[1].even_threat_count > 0)
    {
        possible_winner = 1;
    }
    if(possible_winner == player)
    {
        return 1;
    }else if(possible_winner == other(player))
    {
        return -1;
    }else
    {
        return 0;
    }
    
       
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
        
        /* random is included for tie breaking*/
        score += 0.0001 * JNU_get_random_double();
        
        /* more lines has a little value, helps claim the centre early on */
        score += .05 * JNU_heuristic_claim_lines(current_state, player);
        
        /*  having unblocked lines of length 2 has some value */
        score += .2 * JNU_heuristic_unblocked_lines(current_state, player, 2);
        
        /*  having unblocked lines of length 3 has more value */
        score += .5 * JNU_heuristic_unblocked_lines(current_state, player, 3);
        
        /*  having enemy unblocked lines of length 3 is bad */
        score += -.1 * JNU_heuristic_unblocked_lines(current_state, enemy, 3);
        
        /* TODO: check if its possible to force a win using Zugzwang
        score +=.5 * JNU_heuristic_zugzwang(current_state,player,player_threats);
         */
    }
    
    /* free the threat arrays */
    free(player_threats[0].threats);
    free(player_threats[1].threats);
    return score;
}


/* claim lines values having a chip in as many lines as possible 
 * this is used early on to increase the possibilities for creating 
 * winning lines
 * it also reduces the number of options for the enemy
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



int JNU_smart_win_checker(Game_state *current_state,
                          int player,
                          Player_Threats player_threats[2])
{
    /* first check for 4 in a row */
    int winner = game_won(current_state);
    int enemy = other(player);
    if(winner != player && winner != enemy)
    {
        /*check if the enemy has a playable win */
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
    return winner;
    
    
}
double JNU_get_random_double()
{
    return (double) rand() / (RAND_MAX / 2) - 1;
}
