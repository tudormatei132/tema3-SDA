#ifndef FEED_H
#define FEED_H
#include "posts.h"
#include "friends.h"
/**
 * Function that handles the calling of every command from task 3
 *
 * Please add any necessary parameters to the functions
*/
void handle_input_feed(matrix_graph_t *mg, post_t **posts, int *size, char *input);

#endif // FEED_H
