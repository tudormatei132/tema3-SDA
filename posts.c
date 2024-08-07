#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "users.h"
#include "posts.h"

#define CREATE_FORMAT "Created \"%s\" for %s\n"
#define REPOST_FORMAT "Created repost #%d for %s\n"
#define COMMON_REPOST "The first common repost of %d and %d is %d\n"
#define GET_REPOST_FORMAT "Repost #%d by %s\n"
#define GET_REPOST_TITLE "\"%s\" - Post by %s\n"
#define LIKED_POST "User %s liked post \"%s\"\n"
#define LIKED_REPOST "User %s liked repost \"%s\"\n"
#define UNLIKED_POST "User %s unliked post \"%s\"\n"
#define UNLIKED_REPOST "User %s unliked repost \"%s\"\n"
#define GET_POST_LIKES "Post \"%s\" has %d likes\n"
#define GET_REPOST_LIKES "Repost #%d has %d likes\n"
#define GET_RATIOD "Post %d got ratio'd by repost %d\n"
#define NOT_RATIOD "The original post is the highest rated\n"
#define DELETE_POST "Deleted \"%s\"\n"
#define DELETE_REPOST "Deleted repost #%d of post \"%s\"\n"

tree_t *init_tree(int data_size)
{
	tree_t *ans = malloc(sizeof(*ans));
	ans->data_size = data_size;

	return ans;
}

tree_node_t *search(tree_node_t *root, int pid)
{
	if (!root)
		return NULL;
	post_t *temp = root->data;
	if (temp->id == pid)
		return root;
	for (int i = 0; i < root->n_children; i++) {
		tree_node_t *res = search(root->children[i], pid);
		if (res)
			return res;
	}

	return NULL;
}

void add_node(tree_t *tree, void *data, int pid)
{
	tree_node_t *node = malloc(sizeof(*node));
	node->data = malloc(tree->data_size);
	node->children = NULL;
	node->pid = pid;
	memcpy(node->data, data, tree->data_size);
	node->n_children = 0;
	if (pid == -1) {
		tree->root = node;
		return;
	}
	tree_node_t *parent = search(tree->root, pid);
	if (!parent)
		return;
	parent->n_children++;
	if (!parent->children) {
		parent->children = malloc(sizeof(*parent->children));
		parent->children[0] = node;
	} else {
		parent->children = realloc(parent->children, parent->n_children *
								   sizeof(*parent->children));
		parent->children[parent->n_children - 1] = node;
	}
}

char *get_title(char *cmd)
{
	char *ans = malloc(strlen(cmd) - 1);
	for (size_t i = 0; i < strlen(cmd) - 2; i++)
		ans[i] = cmd[i + 1];
	ans[strlen(cmd) - 2] = 0;
	return ans;
}

int first_common_repost(tree_node_t *root, int id1, int id2)
{
	int found = -1;
	post_t *temp = root->data;
	for (int i = 0; i < root->n_children; i++) {
		if (search(root->children[i], id1) && search(root->children[i], id2)) {
			// if we find both nodes in the same subtree, then we need to
			// lower the level of the first common ancestor
			found = i;
			break;
		}
	}
	if (found == -1) // if we can't find both nodes below, then stop
		return temp->id;
	return first_common_repost(root->children[found], id1, id2);
}

void print_children(tree_node_t *node)
{
	for (int i = 0; i < node->n_children; i++) {
		post_t *temp = node->children[i]->data;
		char *name = get_user_name(temp->uid);
		printf(GET_REPOST_FORMAT, temp->id, name);
		print_children(node->children[i]);
	}
}

int is_in_array(int *v, int n, int x)
{
	for (int i = 0; i < n; i++) {
		if (x == v[i])
			return i;
	}
	return -1;
}

void del_elem(int *v, int n, int pos)
{
	for (int i = pos; i < n - 1; i++)
		v[i] = v[i + 1];
}

void manage_like(tree_node_t *liked, char *name, char *title)
{
	post_t *temp = liked->data;
	int uid = get_user_id(name);
	int found = is_in_array(temp->ppl, temp->likes, uid);
	if (found == -1) {
		// check if the person has already liked the post
		// if they didn't, then add them to the list of
		// person who liked
		temp->likes++;
		if (temp->likes > 1)
			temp->ppl = realloc(temp->ppl, temp->likes * sizeof(int));
		else
			temp->ppl = malloc(sizeof(int));
		temp->ppl[temp->likes - 1] = uid;
		if (temp->title) //check if it's a post or a repost
			printf(LIKED_POST, name, temp->title);
		else
			printf(LIKED_REPOST, name, title);
		return;
	}
	// remove the person from the list if they had already liked the post
	del_elem(temp->ppl, temp->likes, found);
	temp->likes--;
	if (temp->likes)
		temp->ppl = realloc(temp->ppl, temp->likes * sizeof(int));
	else
		free(temp->ppl);
	if (temp->title)
		printf(UNLIKED_POST, name, temp->title);
	else
		printf(UNLIKED_REPOST, name, title);
}

int ratio(tree_node_t *root)
{
	post_t *base = root->data;
	int id = root->n_children;
	int max = base->likes;
	// check the children's like counter
	// then compare it to the original post
	for (int i = 0; i < root->n_children; i++) {
		post_t *rep = root->children[i]->data;
		if (max < rep->likes) {
			max = rep->likes;
			id = rep->id;
		} else if (max == rep->likes) {
			if (id > rep->id)
				id = rep->id;
		}
	}
	if (id == root->n_children)
		return -1;
	return id;
}

void post_free(post_t **post)
{
	if ((*post)->title)
		free((*post)->title);
	if ((*post)->ppl)
		free((*post)->ppl);
	free(*post);
}

void free_tree(tree_node_t *root)
{
	post_t *temp = root->data;
	post_free(&temp);
	for (int i = 0; i < root->n_children; i++)
		free_tree(root->children[i]);

	if (root->children)
		free(root->children);
	free(root);
}

void delete_node(tree_t *tree, tree_node_t *node)
{
	if (node->pid == -1) {
		free_tree(tree->root);
		tree->root = NULL;
		return;
	}
	tree_node_t *parent = search(tree->root, node->pid);
	for (int i = 0; i < parent->n_children; i++) {
		if (parent->children[i] == node) {
			// remove the node from the children of its parent
			for (int j = i; j < parent->n_children - 1; j++)
				parent->children[j] = parent->children[j + 1];
			parent->n_children--;
			if (parent->n_children) {
				parent->children = realloc(parent->children, sizeof
									       (tree_node_t *)*parent->n_children);
			} else {
				free(parent->children);
				parent->children = NULL;
			}
			break;
		}
	}
	//remove all the corresponding subtree
	free_tree(node);
}

void create_post(post_t **posts, char *cmd, int *size, int *post_no)
{
	cmd = strtok(NULL, "\n ");
	char *name = strdup(cmd);
	int uid = get_user_id(cmd);
	cmd = strtok(NULL, "\n");
	char *title = get_title(cmd);
	post_t post;

	post.uid = uid;
	post.id = *post_no + 1;
	post.title = title;
	post.likes = 0;
	post.ppl = NULL;
	// temporary post which will be the root of the new tree
	posts[*size] = malloc(sizeof(post_t));
	memcpy(posts[*size], &post, sizeof(post_t));
	posts[*size]->tree = init_tree(sizeof(post_t));
	add_node(posts[*size]->tree, &post, -1);
	// increment the size and the next post/repost id
	(*post_no)++;
	(*size)++;
	printf(CREATE_FORMAT, title, name);
	free(name);
}

void create_repost(post_t **posts, char *cmd, int *size, int *post_no)
{
	cmd = strtok(NULL, "\n ");
	char *name = strdup(cmd);
	cmd = strtok(NULL, "\n ");
	int root = atoi(cmd);
	cmd = strtok(NULL, "\n ");
	int node = root;
	if (cmd)
		node = atoi(cmd);
	// works the same way as the previous function
	// but it adds the node to the array of children
	// of its parent
	post_t post;
	post.title = NULL;
	post.likes = 0;
	post.ppl = NULL;
	post.id = ++(*post_no);
	post.uid = get_user_id(name);
	for (int i = 0; i < *size; i++) {
		if (posts[i]->id == root)
			add_node(posts[i]->tree, &post, node);
	}
	printf(REPOST_FORMAT, post.id, name);
	free(name);
}

void delete_post(post_t **posts, char *cmd, int *size)
{
	cmd = strtok(NULL, "\n ");
	int root = atoi(cmd);
	int node = root;
	cmd = strtok(NULL, "\n ");
	if (cmd)
		node = atoi(cmd);
	for (int i = 0; i < *size; i++) {
		if (posts[i]->id == root) {
			// find our post/repost in the array
			// and then in the corresponding tree
			tree_node_t *del = search(posts[i]->tree->root, node);
			post_t *temp1 = del->data;
			char *title = NULL;
			if (temp1->title)
				title = strdup(temp1->title);
			delete_node(posts[i]->tree, del);

			if (title) {
				// if we deleted a post, then remove the element from the array
				printf(DELETE_POST, title);
				for (int j = i; j < *size - 1; j++)
					posts[j] = posts[j + 1];
				free(title);
				(*size)--;
			} else {
				printf(DELETE_REPOST, node, posts[i]->title);
			}
		}
	}
}

void get_reposts(post_t **posts, char *cmd, int size)
{
	cmd = strtok(NULL, "\n ");
	int root = atoi(cmd);
	int node = root;
	cmd = strtok(NULL, "\n ");
	if (cmd)
		node = atoi(cmd);
	for (int i = 0; i < size; i++) {
		if (posts[i]->id == root) {
			// search for the original post
			// then use the function to search in the tree
			tree_node_t *temp1 = search(posts[i]->tree->root, node);
			post_t *temp = temp1->data;
			char *name = get_user_name(temp->uid);
			if (temp->title)
				printf(GET_REPOST_TITLE, temp->title, name);
			else
				printf(GET_REPOST_FORMAT, temp->id, name);
			print_children(temp1);
			break;
		}
	}
}

int handle_input_posts(post_t **posts, char *input)
{
	char *commands = strdup(input);
	char *cmd = strtok(commands, "\n ");
	static int post_no, size;
	if (!cmd)
		return -1;
	if (!strcmp(cmd, "create")) {
		create_post(posts, cmd, &size, &post_no);
	} else if (!strcmp(cmd, "repost")) {
		create_repost(posts, cmd, &size, &post_no);
	} else if (!strcmp(cmd, "common-repost")) {
		cmd = strtok(NULL, "\n ");
		int root = atoi(cmd);
		cmd = strtok(NULL, "\n ");
		int id1 = atoi(cmd);
		cmd = strtok(NULL, "\n ");
		int id2 = atoi(cmd);
		int res;
		for (int i = 0; i < size; i++) {
			if (posts[i]->id == root)
				res = first_common_repost(posts[i]->tree->root, id1, id2);
		}
		printf(COMMON_REPOST, id1, id2, res);
	} else if (!strcmp(cmd, "like")) {
		cmd = strtok(NULL, "\n ");
		char *name = strdup(cmd);
		cmd = strtok(NULL, "\n ");
		int root = atoi(cmd);
		int node = root;
		cmd = strtok(NULL, "\n ");
		if (cmd)
			node = atoi(cmd);
		for (int i = 0; i < size; i++) {
			// search for the original post in the array
			// then for the repost in the tree
			if (posts[i]->id == root) {
				tree_node_t *liked = search(posts[i]->tree->root, node);
				manage_like(liked, name, posts[i]->title);
				free(name);
			}
		}
	} else if (!strcmp(cmd, "ratio")) {
		cmd = strtok(NULL, "\n ");
		int root = atoi(cmd);
		for (int i = 0; i < size; i++) {
			if (posts[i]->id == root) {
				int ratiod = ratio(posts[i]->tree->root);
				if (ratiod == -1)
					printf(NOT_RATIOD);
				else
					printf(GET_RATIOD, root, ratiod);
			}
		}
	} else if (!strcmp(cmd, "delete")) {
		delete_post(posts, cmd, &size);
	} else if (!strcmp(cmd, "get-likes")) {
		cmd = strtok(NULL, "\n ");
		int root = atoi(cmd);
		cmd = strtok(NULL, "\n ");
		int node = root;
		if (cmd)
			node = atoi(cmd);
		for (int i = 0; i < size; i++) {
			if (posts[i]->id == root) {
				tree_node_t *get = search(posts[i]->tree->root, node);
				post_t *temp = get->data;
				if (temp->title)
					printf(GET_POST_LIKES, temp->title, temp->likes);
				else
					printf(GET_REPOST_LIKES, temp->id, temp->likes);
				break;
			}
		}
	} else if (!strcmp(cmd, "get-reposts")) {
		get_reposts(posts, cmd, size);
	}
	free(commands);
	return size;
}

void free_all_memory(post_t **posts, int size)
{
	for (int i = 0; i < size; i++) {
		free_tree(posts[i]->tree->root);
		free(posts[i]->tree);
		free(posts[i]);
	}
}
