/**
 * The entrypoint of the homework. Every initialization must be done here
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "users.h"
#include "friends.h"
#include "posts.h"
#include "feed.h"
/**
 * Initializez every task based on which task we are running
*/
void init_tasks(void)
{
	#ifdef TASK_1

	#endif

	#ifdef TASK_2

	#endif

	#ifdef TASK_3

	#endif
}

/**
 * Entrypoint of the program, compiled with different defines for each task
*/
int main(void)
{
	init_users();

	init_tasks();

	char *input = (char *)malloc(MAX_COMMAND_LEN);
	#ifdef TASK_2
	post_t *posts[1000];
	(void)posts;
	#endif
	#ifdef TASK_1
	matrix_graph_t *mg = mg_create(MAX_PEOPLE);
	#endif
	int size = -1;
	while (1) {
		char *command = fgets(input, MAX_COMMAND_LEN, stdin);

		// If fgets returns null, we reached EOF
		if (!command)
			break;

		#ifdef TASK_1
		handle_input_friends(input, mg);
		#endif

		#ifdef TASK_2
		size = handle_input_posts(posts, input);
		#endif

		#ifdef TASK_3
		handle_input_feed(input);
		#endif
	}
	#ifdef TASK_2
	free_all_memory(posts, size);
	#endif
	(void)size;
	free_users();
	free(input);

	return 0;
}
