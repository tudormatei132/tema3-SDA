#ifndef POSTS_H
#define POSTS_H

/**
 * Function that handles the calling of every command from task 2
 *
 * Please add any necessary parameters to the functions
*/

typedef struct tree_t tree_t;
typedef struct tree_node_t tree_node_t;
struct tree_node_t {
	tree_node_t **children;
	int n_children;
	int pid;
	void *data;
};

struct tree_t {
	tree_node_t *root;
	int data_size;
};

typedef struct {
	char *title;
	int likes;
	int *ppl, id, uid;
	tree_t *tree;
} post_t;

int handle_input_posts(post_t **posts, char *input);

void free_all_memory(post_t **posts, int size);

#endif // POSTS_H
