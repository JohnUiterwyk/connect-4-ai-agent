#include "c4.h"
#include <stdlib.h>

void build_map(Game_state *current_state)
{
   int size_y = current_state->height;
   int size_x = current_state->width;
   int i, j, k, x, win_index = 0;
   int *win_indices;
   int ***map = current_state->map;
   int num_to_connect = current_state->num_to_connect;

 /* Fill in the horizontal win positions */
    for (i=0; i<size_y; i++)
        for (j=0; j<size_x-num_to_connect+1; j++) {
            for (k=0; k<num_to_connect; k++) {
                win_indices = map[j+k][i];
                for (x=0; win_indices[x] != -1; x++)
                    ;
                win_indices[x++] = win_index;
                win_indices[x] = -1;
            }
            win_index++;
        }

    /* Fill in the vertical win positions */
    for (i=0; i<size_x; i++)
        for (j=0; j<size_y-num_to_connect+1; j++) {
            for (k=0; k<num_to_connect; k++) {
                win_indices = map[i][j+k];
                for (x=0; win_indices[x] != -1; x++)
                    ;
                win_indices[x++] = win_index;
                win_indices[x] = -1;
            }
            win_index++;
        }

    /* Fill in the forward diagonal win positions */
    for (i=0; i<size_y-num_to_connect+1; i++)
        for (j=0; j<size_x-num_to_connect+1; j++) {
            for (k=0; k<num_to_connect; k++) {
                win_indices = map[j+k][i+k];
                for (x=0; win_indices[x] != -1; x++)
                    ;
                win_indices[x++] = win_index;
                win_indices[x] = -1;
            }
            win_index++;
        }

    /* Fill in the backward diagonal win positions */
    for (i=0; i<size_y-num_to_connect+1; i++)
        for (j=size_x-1; j>=num_to_connect-1; j--) {
            for (k=0; k<num_to_connect; k++) {
                win_indices = map[j-k][i+k];
                for (x=0; win_indices[x] != -1; x++)
                    ;
                win_indices[x++] = win_index;
                win_indices[x] = -1;
            }
            win_index++;
        }
}

/** Determines whether a player has won by checking if any of the lines have num_to_connect pieces in them */      
int game_won(Game_state *current_state)
{
   int i;
   int num_win_places = num_of_win_places(current_state->width, current_state->height, current_state->num_to_connect);
   
   for (i = 0; i < num_win_places; i++)
   {
      if (current_state->score_array[0][i] == current_state->num_to_connect)
         return 0;
      else if (current_state->score_array[1][i] == current_state->num_to_connect)
         return 1;
   }
   
   return C4_NONE;
}

