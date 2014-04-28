#include <stdlib.h>
#include "c4.h"

/* Sample agents for Connect 4. 
 * 
 * Here are some agents to get you started.
 *
 * Game_state is defined in c4.h. It contains interesting things such as 
 * the board positions, size and the number you need to connect. It also
 * contains the line scores and map.
 *
 * player is the number of the player (0 or 1) that just moved.
 * x is the column the player just moved in (0 is the left-most column)
 * y is the row the player just moved in (0 is the bottom column)
 *
 * The function should return a double based on how 'good' it thinks the
 * board position is for the player given in 'player'. A higher number 
 * indicates a better board. Remember to give winning positions the
 * highest score!
 *
 * You should not assume that width, height and num_to_connect will always
 * be the same. Your agent (and other functions) should work correctly with
 * different board sizes and num_to_connect values. 
 *
 * Please make sure you only submit one agent, and it is called agent_<your username>
 *
 * Stefan Bird <stbird@seatiger.org>
 */


double agent_random(Game_state *current_state, int player, int x, int y);
double agent_simple(Game_state *current_state, int player, int x, int y);

/* This agent will just pick a random place to move. It should be very easy
 * to defeat this agent! 
 */

double agent_random(Game_state *current_state, int player, int x, int y)
{
   return (double) rand() / (RAND_MAX / 2) - 1;
}


/* This agent will try to make a winning line if it can (it will also try to 
 * block your winning lines). If it can't make a winning line, it will just
 * use the random agent to evaluate the position. Note that it scales down the 
 * score from the random agent by 2 - by having separate functions that return
 * between 0 and 1, and scaling them up or down you can easily adjust the weight
 * of each heuristic you use.
 *
 * Make sure you don't call your helper functions agent_<...> - it will confuse the compiler
 */

double agent_simple(Game_state *current_state, int player, int x, int y)
{
   double score = 0;
   int is_game_won = game_won(current_state);
   
   if (is_game_won == player)
      score = 1;
   else if (is_game_won == other(player))
      score = -1;   
   else 
   {
      score += 0.5 * agent_random(current_state, player, x, y);
      /* score += 0.3 * super_cool_heuristic_a(current_state, player, x, y) */ 
      /* score += 0.8 * super_cool_heuristic_b(current_state, player, x, y) */
   }
  
   return score;   
}


