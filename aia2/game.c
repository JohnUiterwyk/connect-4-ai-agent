/***************************************************************************
**                                                                        **
**                          Connect-4 Algorithm                           **
**                                                                        **
****************************************************************************
**                                                                        **
**                          Sample Implementation!                        **
**                                                                        **
**  This code is poorly written and contains no internal documentation.   **
**  Its sole purpose is to quickly demonstrate an actual implementation   **
**  of the functions contained in the file "c4.c".  It's a fully working  **
**  game which should work on any type of system, so give it a shot!      **
**                                                                        **
**  The computer is pretty brain-dead at level 3 or less, but at level 4  **
**  and up it provides quite a challenge!                                 **
**                                                                        **
****************************************************************************
**  Adaptation of a Connect4 program with author's permission.            **
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "c4.h"

#define BUFFER_SIZE 80

enum {HUMAN = 0, COMPUTER = 1};

static int get_num(char *prompt, int lower, int upper, int default_val);
static void print_board(int width, int height);
static void print_dot(void);

static char piece[2] = { 'X', 'O' };

int
main(int argc, char **argv)
{
    int level, turn = 0, move;
    int width, height, num_to_connect;
    int selectedAgent;
    /*int x1, y1, x2, y2;*/
    char buffer[BUFFER_SIZE];
    int num_agents = count_agents();
    Agent **available_agents = get_agents();
    Agent *agents[2];
    int count;
    int fastMode = FALSE;
    
    if ((argc == 2 || argc == 4) && strcmp(argv[argc - 1], "--fast") == 0)
    {
       fastMode = TRUE;
       argc--;
    }
  
    printf("\n****  Welcome to the game of Connect!  ****\n\n");
 
    width = 7; /* get_num("Width of board", 1, 40, 7); */
    height = 6; /* get_num("Height of board", 1, 40, 6); */
    num_to_connect = 4; /* get_num("Number to connect", 1, 40, 4); */
    level = 3; /* get_num("Search depth of AI agents, 1, 10, 3); */

    if (argc != 1 && argc != 3)
    {
       printf("Usage: %s [agent1 agent2] [--fast]\n", argv[0]);
       printf("   agent1 and agent2 are the names of the agents you wish to play.\n");
       printf("   --fast disables the delay between computer moves, making the game much faster.\n\n");
       printf("Available agents: human");
       for (count = 0; count < num_agents; count++)
          printf(", %s", available_agents[count]->name);

       printf("\n");   

       exit(EXIT_FAILURE);
    }

    if (argc == 3)
    {
        agents[0] = NULL;
        agents[1] = NULL;
        
        for (count = 0; count < num_agents; count++)
        {
            if (!strcmp(argv[1], available_agents[count]->name))
                agents[0] = available_agents[count];
            if (!strcmp(argv[2], available_agents[count]->name))
                agents[1] = available_agents[count];
        }

        if (agents[0] == NULL && strcmp(argv[1], "human"))
        {
            printf("Unknown agent '%s'\nType %s --help for help\n", argv[1], argv[0]);
            exit(EXIT_FAILURE);
        }

        if (agents[1] == NULL && strcmp(argv[2], "human"))
        {
            printf("Unknown agent '%s'\nType %s --help for help\n", argv[2], argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        printf("Here are the available agents:\n");
        
        printf("  1. Human\n");
        
        for (count = 0; count < num_agents; count++)
           printf("  %i. %s\n", count + 2, available_agents[count]->name);
        
        printf("\n");
        
        selectedAgent = get_num("Please select an agent for player 1:", 1, num_agents + 1, 1);
        if (selectedAgent == 1)  /* Human */
           agents[0] = NULL;
        else
           agents[0] = available_agents[selectedAgent - 2];
               
        selectedAgent = get_num("Please select an agent for player 2:", 1, num_agents + 1, 1);
        if (selectedAgent == 1)  /* Human */
           agents[1] = NULL;
        else
           agents[1] = available_agents[selectedAgent - 2];       
    }

    printf("\nSelected agents: ");
    printf("%s vs ", agents[0] == NULL ? "Human" : agents[0]->name);
    printf("%s\n\n", agents[1] == NULL ? "Human" : agents[1]->name);

    c4_new_game(width, height, num_to_connect, agents);    
    c4_poll(print_dot, CLOCKS_PER_SEC/2);

    do {
        print_board(width, height);
        if (agents[turn] == NULL) {  /* Human */
            do 
            {
               sprintf(buffer, "Player %c, drop in which column",
                            piece[turn]);
               move = get_num(buffer, 1, width, -1) - 1;
            }
            while (!c4_make_move(turn, move, NULL));
        }
        else {
            printf("Player %c (%s) is thinking.", piece[turn], agents[turn]->name);
            fflush(stdout);
            c4_auto_move(turn, level, &move, NULL);
                
            printf("\n\nPlayer %c (%s) dropped its piece into column %d.\n",
                   piece[turn], agents[turn]->name, move+1);
            
            if (agents[0] != NULL && agents[1] != NULL && !fastMode)
               sleep(2);
        }

        turn = !turn;

    } while (!c4_is_winner(0) && !c4_is_winner(1) && !c4_is_tie());

    print_board(width, height);

    if (c4_is_winner(0)) {
        if (agents[0] == NULL)
           printf("Player %c won!", piece[0]);
        else
           printf("Player %c (%s) won!", piece[0], agents[0]->name);
/*        c4_win_coords(&x1, &y1, &x2, &y2);
        printf("  (%d,%d) to (%d,%d)\n\n", x1+1, y1+1, x2+1, y2+1);*/
    }
    else if (c4_is_winner(1)) {
        if (agents[1] == NULL)
           printf("Player %c won!", piece[1]);
        else
           printf("Player %c (%s) won!", piece[1], agents[1]->name);
        /*c4_win_coords(&x1, &y1, &x2, &y2);
        printf("  (%d,%d) to (%d,%d)\n\n", x1+1, y1+1, x2+1, y2+1);*/
    }
    else {
        printf("There was a tie!\n\n");
    }

    c4_end_game();
    
    for (count = 0; count < num_agents; count++)
       free(available_agents[count]);
    
    free(available_agents);
       
    printf("\n");
    return 0;
}


/****************************************************************************/

static int
get_num(char *prompt, int lower, int upper, int default_value)
{
    int number = -1;
    int result;
    static char numbuf[40];

    do {
        if (default_value != -1)
            printf("%s [%d]? ", prompt, default_value);
        else
            printf("%s? ", prompt);

        if (!fgets(numbuf, 40, stdin) || numbuf[0] == 'q') {
            printf("\nGoodbye!\n");
            exit(0);
        }
        result = sscanf(numbuf, "%d", &number);
    } while (result == 0 || (result != EOF && (number<lower || number>upper)));

    return (result == EOF? default_value : number);
}

/****************************************************************************/

static void
print_board(int width, int height)
{
    int x, y;
    char **board, spacing[2], dashing[2];

    board = c4_board();

    spacing[1] = dashing[1] = '\0';
    if (width > 19) {
        spacing[0] = '\0';
        dashing[0] = '\0';
    }
    else {
        spacing[0] = ' ';
        dashing[0] = '-';
    }

    printf("\n");
    for (y=height-1; y>=0; y--) {

        printf("|");
        for (x=0; x<width; x++) {
            if (board[x][y] == C4_NONE)
                printf("%s %s|", spacing, spacing);
            else
                printf("%s%c%s|", spacing, piece[(int)board[x][y]], spacing);
        }
        printf("\n");

        printf("+");
        for (x=0; x<width; x++)
            printf("%s-%s+", dashing, dashing);
        printf("\n");
    }

    printf(" ");
    for (x=0; x<width; x++)
        printf("%s%d%s ", spacing, (x>8)?(x+1)/10:x+1, spacing);
    if (width > 9) {
        printf("\n ");
        for (x=0; x<width; x++)
            printf("%s%c%s ", spacing, (x>8)?'0'+(x+1)-((x+1)/10)*10:' ',
                              spacing);
    }
    printf("\n\n");
}

/****************************************************************************/

static void
print_dot(void)
{
    printf(".");
    fflush(stdout);
}
