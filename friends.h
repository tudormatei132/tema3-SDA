#ifndef FRIENDS_H
#define FRIENDS_H

#define MAX_COMMAND_LEN 500
#define MAX_PEOPLE 550

typedef struct
{
	int **matrix; /* Matricea de adiacenta a grafului */
	int nodes;    /* Numarul de noduri din graf. */
} matrix_graph_t;

matrix_graph_t *mg_create(int nodes);

void mg_free(matrix_graph_t *graph);

/**
 * Function that handles the calling of every command from task 1
 *
 * Please add any necessary parameters to the functions
*/
void handle_input_friends(char *input, matrix_graph_t *mg);

#endif // FRIENDS_H
