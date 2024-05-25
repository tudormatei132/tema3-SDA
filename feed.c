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

void search_reposts(tree_node_t *root, int uid, char *title)
{
	// find all reposts of a user in a tree, recursively
	post_t *temp = root->data;
	if (temp->uid == uid) {
		if (!temp->title)
			printf(SHOW_REPOST, title);
	}
	// start from the root, then search it through its children
	for (int i = 0; i < root->n_children; i++)
		search_reposts(root->children[i], uid, title);
}

void show_profile(post_t **posts, int size, int uid)
{
	for (int i = 0; i < size; i++) {
		// find all posts first
		if (posts[i]->uid == uid)
			printf(SHOW_POST, posts[i]->title);
	}
	for (int i = 0; i < size; i++) {
		search_reposts(posts[i]->tree->root, uid, posts[i]->title);
	}
}

int *who_reposted(tree_node_t *node, int *v_size)
{
	(*v_size) = 0;
	int *result = malloc(sizeof(int));
	// forms an array of people that reposted a post/repost
	for (int i = 0; i < node->n_children; i++) {
		post_t *temp = node->children[i]->data;
		(*v_size)++;
		// add the uid of every child to the array
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
		// go through our array of people who reposted
		// check if there are any friends and print their name
		if (mg->matrix[uid][repost[i]]) 
			printf("%s\n", get_user_name(repost[i]));
	}
	free(repost);
}

void add_to_set(int set[], int *size, int vertex)
{
	set[(*size)++] = vertex;
}

void remove_from_set(int set[], int *size, int vertex)
{
	int i;
	for (i = 0; i < *size; i++) {
		if (set[i] == vertex)
			break;
	}

	for (int j = i; j < *size - 1; j++)
		set[j] = set[j + 1];

	(*size)--;
}

int *intersection(int set1[], int size1, int set2[], int size2,
				  int *new_size, matrix_graph_t *mg)
{
	int *intersect = malloc(mg->nodes * sizeof(int));
	*new_size = 0;
	for (int i = 0; i < size1; i++) {
		for (int j = 0; j < size2; j++) {
			if (set1[i] == set2[j]) {
				intersect[(*new_size)++] = set1[i];
				break;
			}
		}
	}
	return intersect;
}

void bron_kerbosch1(int *r, int *p, int *x, int r_size, int p_size,
					int x_size, matrix_graph_t *mg, int id1, int *printed)
{
	if (p_size == 0 && x_size == 0) {
		if (r_size > *printed)
			*printed = r_size; //here we get the maximum length
		return;
	}

	int *p_copy = malloc(sizeof(int) * mg->nodes);
	for (int i = 0; i < p_size; i++)
		p_copy[i] = p[i];
	int p_copy_size = p_size;

	for (int i = 0; i < p_copy_size; i++) {
		int v = p_copy[i];

		int *n_v = malloc(sizeof(int) * mg->nodes);
		int n_v_size = 0;
		for (int j = 0; j < mg->nodes; j++) {
			if (mg->matrix[v][j])
				n_v[n_v_size++] = j;
		}

		//this is just the skeleton from the link given
		int p_intersect_nv_size;
		int x_intersect_nv_size;
		int *p_intersect_nv = intersection(p, p_size, n_v, n_v_size,
										   &p_intersect_nv_size, mg);
		int *x_intersect_nv = intersection(x, x_size, n_v, n_v_size,
										   &x_intersect_nv_size, mg);

		add_to_set(r, &r_size, v);
		bron_kerbosch1(r, p_intersect_nv, x_intersect_nv, r_size,
					   p_intersect_nv_size, x_intersect_nv_size,
					   mg, id1, printed);
		remove_from_set(r, &r_size, v);

		remove_from_set(p, &p_size, v);
		add_to_set(x, &x_size, v);

		free(n_v);
		free(p_intersect_nv);
		free(x_intersect_nv);
	}

	free(p_copy);
}

void bron_kerbosch2(int *r, int *p, int *x, int r_size, int p_size,
					int x_size, matrix_graph_t *mg, int id1, int printed)
{
	if (p_size == 0 && x_size == 0 && r_size == printed) {
		for (int i = 0; i < r_size; i++)
			printf("%s\n", get_user_name(r[i]));
		return;
	}

	int *p_copy = malloc(sizeof(int) * mg->nodes);
	for (int i = 0; i < p_size; i++)
		p_copy[i] = p[i];
	int p_copy_size = p_size;

	for (int i = 0; i < p_copy_size; i++) {
		int v = p_copy[i];

		int *n_v = malloc(sizeof(int) * mg->nodes);
		int n_v_size = 0;
		for (int j = 0; j < mg->nodes; j++) {
			if (mg->matrix[v][j])
				n_v[n_v_size++] = j;
		}

		int p_intersect_nv_size;
		int x_intersect_nv_size;
		int *p_intersect_nv = intersection(p, p_size, n_v, n_v_size,
										   &p_intersect_nv_size, mg);
		int *x_intersect_nv = intersection(x, x_size, n_v, n_v_size,
										   &x_intersect_nv_size, mg);

		add_to_set(r, &r_size, v);
		bron_kerbosch2(r, p_intersect_nv, x_intersect_nv, r_size,
					   p_intersect_nv_size, x_intersect_nv_size,
					   mg, id1, printed);
		remove_from_set(r, &r_size, v);

		remove_from_set(p, &p_size, v);
		add_to_set(x, &x_size, v);

		free(n_v);
		free(p_intersect_nv);
		free(x_intersect_nv);
	}

	free(p_copy);
}

void common_groups(char *cmd, matrix_graph_t *mg)
{
	cmd = strtok(NULL, "\n ");
	char *name1 = strdup(cmd);

	int id1 = get_user_id(name1);

	//if he doesn't have friends, he's solo
	//this if saves time and memory
	if (friends(name1, mg) == 0) {
		printf("%s\n", name1);
		free(name1);
		return;
	}

	///BronKerbosch algorithm
	int *r = malloc(sizeof(int) * mg->nodes);
	int *p = malloc(sizeof(int) * mg->nodes);
	int *x = malloc(sizeof(int) * mg->nodes);
	int r_size = 0, p_size = 0, x_size = 0;

	for (int i = 0; i < mg->nodes; i++) {
		//important if, we want to find the maximum clique
		//just for the friends of our user
		if (mg->matrix[id1][i] || id1 == i)
			p[p_size++] = i;
	}

	int printed = 0; //safes the length of the maximum clique
	printf("The closest friend group of %s is:\n", name1);
	//it modifies printed
	bron_kerbosch1(r, p, x, r_size, p_size, x_size, mg, id1, &printed);

	free(r); //clear the memory
	free(p);
	free(x);

	//reinitalize the algorithm
	r = malloc(sizeof(int) * mg->nodes);
	p = malloc(sizeof(int) * mg->nodes);
	x = malloc(sizeof(int) * mg->nodes);
	r_size = 0, p_size = 0, x_size = 0;

	for (int i = 0; i < mg->nodes; i++) {
		if (mg->matrix[id1][i] || id1 == i)
			p[p_size++] = i;
	}

	//the same as bron_kerbosch1, just that it prints the maximum cliqure
	bron_kerbosch2(r, p, x, r_size, p_size, x_size, mg, id1, printed);

	free(r); //clear the memory
	free(p);
	free(x);
	free(name1);
}

void handle_input_feed(matrix_graph_t *mg, post_t **posts,
					   int *size, char *input)
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
			// find the most recent posts
			// so we need to start our search from the end of the array
			if (posts[j]->uid == uid || mg->matrix[uid][posts[j]->uid]) {

				char *name = get_user_name(posts[j]->uid);
				feed_size--;
				printf(GET_FEED, name, posts[j]->title);
			}
		}
	} else if (!strcmp(cmd, "view-profile")) {
		cmd = strtok(NULL, "\n ");
		int uid = get_user_id(cmd);
		show_profile(posts, *size, uid);
	} else if (!strcmp(cmd, "friends-repost")) {
		cmd = strtok(NULL, "\n ");
		int uid = get_user_id(cmd);
		cmd = strtok(NULL, "\n ");
		int id = atoi(cmd);
		for (int i = 0; i < *size; i++) {
			post_t *temp = posts[i]->tree->root->data;
			if (temp->id == id)
				reposts_by_friends(mg, posts[i]->tree->root, uid);
		}
	} else if (!strcmp(cmd, "common-group")) {
		common_groups(cmd, mg);
	}

	free(commands);
}
