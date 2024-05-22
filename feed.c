#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "feed.h"
#include "users.h"
#include "posts.h"
#include "friends.h"

#define GET_FEED "%s: \"%s\"\n"
#define SHOW_POST "Posted: \"%s\"\n"
#define SHOW_REPOST "Reposted: \"%s\"\n"
void search_posts(tree_node_t *root, int uid, char *title)
{
	post_t *temp = root->data;
	if (temp->uid == uid) {
		if (temp->title) {
			printf(SHOW_POST, title);
		} else {
			printf(SHOW_REPOST, title);
		}
	}
	for (int i = 0; i < root->n_children; i++) {
		search_posts(root->children[i], uid, title);
	}
}


void show_profile(post_t **posts, int size, int uid)
{
	for (int i = 0; i < size; i++) {
		post_t *temp = posts[i]->tree->root->data;
		search_posts(posts[i]->tree->root, uid, temp->title);
	}
}


int *who_reposted(tree_node_t *node, int *v_size)
{
	(*v_size) = 0;
	int *result = malloc(sizeof(int));
	for (int i = 0; i < node->n_children; i++) {
		post_t *temp = node->children[i]->data;
		(*v_size)++;

		if ((*v_size) > 1)
			result = realloc(result, sizeof(int) * (*v_size));
		result[(*v_size) - 1] = temp->uid;
	}
	return result;
}

void reposts_by_friends(matrix_graph_t *mg, tree_node_t *node, int uid)
{
	int v_size;
	int *repost = who_reposted(node, &v_size);
	for (int i = 0; i < v_size; i++) {
		if (mg->matrix[uid][repost[i]])
			printf("%s\n", get_user_name(repost[i]));
	}
	free(repost);
}

void handle_input_feed(matrix_graph_t *mg, post_t **posts, int *size, char *input)
{
	char *commands = strdup(input);
	char *cmd = strtok(commands, "\n ");

	if (!cmd)
		return;

	if (!strcmp(cmd, "feed")) {
		cmd = strtok(NULL, "\n ");

		int uid = get_user_id(cmd);
		cmd = strtok(NULL, "\n ");

		int feed_size = atoi(cmd);
		for (int j = *size - 1; j >= 0 && feed_size; j--) {
			post_t *temp = posts[j]->tree->root->data;
			if (temp->uid == uid || mg->matrix[uid][temp->uid]) {
				char *name = get_user_name(temp->uid);
				feed_size--;
				printf(GET_FEED, name, temp->title);
			}
		}
		
		
	}
		// TODO: Add function
	else if (!strcmp(cmd, "view-profile")) {
		cmd = strtok(NULL, "\n ");
		int uid = get_user_id(cmd);
		show_profile(posts, *size, uid);
	}
		// TODO: Add function
	else if (!strcmp(cmd, "friends-repost")) {
		cmd = strtok(NULL, "\n ");
		int uid = get_user_id(cmd);
		cmd = strtok(NULL, "\n ");
		int id = atoi(cmd);
		for (int i = 0; i < *size; i++) {
			post_t *temp = posts[i]->tree->root->data;
			if (temp->id == id)
				reposts_by_friends(mg, posts[i]->tree->root, uid);
		}
	}
		// TODO: Add function
	else if (!strcmp(cmd, "common-group"))
		(void)cmd;
		// TODO: Add function

	free(commands);
}
