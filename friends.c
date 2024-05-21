#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "friends.h"
#include "users.h"

#define INF 999

typedef struct queue_t
{
	/* Dimensiunea maxima a cozii */
	unsigned int max_size;
	/* Numarul de elemente din coada */
	unsigned int size;
	/* Dimensiunea in octeti a tipului de date stocat in coada */
	unsigned int data_size;
	/* Indexul de la care se vor efectua operatiile de front si dequeue */
	unsigned int read_idx;
	/* Indexul de la care se vor efectua operatiile de dequeue */
	unsigned int write_idx;
	/* Bufferul ce stocheaza elementele cozii */
	void **buff;
} queue_t;

//---------graph_functions------------

matrix_graph_t *
mg_create(int nodes)
{
	matrix_graph_t *mg = malloc(sizeof(*mg));

	mg->matrix = calloc(sizeof(int *), nodes);
	for (int i = 0; i < nodes; i++)
		mg->matrix[i] = calloc(sizeof(int), nodes);

	mg->nodes = nodes;
	return mg;
}

void
mg_add_edge(matrix_graph_t *graph, int src, int dest)
{
	graph->matrix[src][dest] = 1;
}

int
mg_has_edge(matrix_graph_t *graph, int src, int dest)
{
	if (graph->matrix[src][dest] == 1)
		return 1;
	return 0;
}

void
mg_remove_edge(matrix_graph_t *graph, int src, int dest)
{
	graph->matrix[src][dest] = 0;
}

void
mg_free(matrix_graph_t *graph)
{
	for (int i = 0; i < graph->nodes; i++)
		free(graph->matrix[i]);
	free(graph->matrix);

	free(graph);
}

//---------queue_functions-----------

queue_t *
q_create(unsigned int data_size, unsigned int max_size)
{
	queue_t *q = calloc(1, sizeof(*q));

	q->data_size = data_size;
	q->max_size = max_size;

	q->buff = malloc(max_size * sizeof(*q->buff));

	return q;
}

unsigned int
q_get_size(queue_t *q)
{
	return !q ? 0 : q->size;
}

unsigned int
q_is_empty(queue_t *q)
{
	return !q ? 1 : !q->size;
}

void *
q_front(queue_t *q)
{
	if (!q || !q->size)
		return NULL;
	return q->buff[q->read_idx];
}

bool
q_dequeue(queue_t *q)
{
	if (!q || !q->size)
		return false;

	free(q->buff[q->read_idx]);

	q->read_idx = (q->read_idx + 1) % q->max_size;
	--q->size;
	return true;
}

bool
q_enqueue(queue_t *q, void *new_data)
{
	void *data;
	if (!q || q->size == q->max_size)
		return false;

	data = malloc(q->data_size);
	memcpy(data, new_data, q->data_size);

	q->buff[q->write_idx] = data;
	q->write_idx = (q->write_idx + 1) % q->max_size;
	++q->size;

	return true;
}

void
q_clear(queue_t *q)
{
	unsigned int i;
	if (!q || !q->size)
		return;

	for (i = q->read_idx; i != q->write_idx; i = (i + 1) % q->max_size)
		free(q->buff[i]);

	q->read_idx = 0;
	q->write_idx = 0;
	q->size = 0;
}

void
q_free(queue_t *q)
{
	if (!q)
		return;

	q_clear(q);
	free(q->buff);
	free(q);
}

void
dfs(matrix_graph_t *mg, int node1, int *distance)
{
	int *color = malloc(sizeof(int) * mg->nodes);
	//the initialization of arrays
	for (int i = 0; i < mg->nodes; i++) {
		color[i] = 0; //node unvisited
		distance[i] = INF;
	}

	queue_t *Q = q_create(sizeof(int), mg->nodes);
	color[node1] = 1; //is being vesited
	distance[node1] = 0;

	q_enqueue(Q, &node1);

	while (Q->size != 0) {
		int v = *(int *)q_front(Q);
		q_dequeue(Q);

		for (int i = 0; i < mg->nodes; i++) {
			if (mg_has_edge(mg, v, i)) {
				if (color[i] == 0) {
					color[i] = 1;
					//calculate the distance
					distance[i] = distance[v] + 1;
					q_enqueue(Q, &i);
				}
			}
		}
		color[v] = 2; //it's visitation has finished
	}
	q_free(Q); //clear memory
	free(color);
}

//------FUNCTIONS---------

void add_friend(char *cmd, matrix_graph_t *mg)
{
	cmd = strtok(NULL, "\n "); //get the names
	char *name1 = strdup(cmd);
	cmd = strtok(NULL, "\n ");
	char *name2 = strdup(cmd);

	int id1 = get_user_id(name1); //get the id of each name
	int id2 = get_user_id(name2);

	mg_add_edge(mg, id1, id2); //create the link between names
	mg_add_edge(mg, id2, id1); //in both directions

	if (mg->matrix[id2][id1] == 1 && mg->matrix[id1][id2] == 1)
		printf("Added connection %s - %s\n", name1, name2);

	free(name1); //clear the memory
	free(name2);
}

void remove_friend(char *cmd, matrix_graph_t *mg)
{
	cmd = strtok(NULL, "\n "); //get the names
	char *name1 = strdup(cmd);
	cmd = strtok(NULL, "\n ");
	char *name2 = strdup(cmd);

	int id1 = get_user_id(name1); //get the id of each name
	int id2 = get_user_id(name2);

	mg_remove_edge(mg, id1, id2); //remove the link between names
	mg_remove_edge(mg, id2, id1); //in both directions

	if (mg->matrix[id2][id1] == 0 && mg->matrix[id1][id2] == 0)
		printf("Removed connection %s - %s\n", name1, name2);

	free(name1); //clear the memory
	free(name2);
}

void suggestions(char *cmd, matrix_graph_t *mg)
{
	cmd = strtok(NULL, "\n "); //get the name
	char *name1 = strdup(cmd);

	int id1 = get_user_id(name1); //get the id of the name

	//the array in which we save 1 for the possible friends
	int *possible_friends = calloc(mg->nodes, sizeof(int));

	int ok = 1; //verifies that there is a possible friend
	for (int i = 0; i < mg->nodes; i++) {
		if (mg->matrix[id1][i]) //for every friend
			for (int j = 0; j < mg->nodes; j++)
				if (mg->matrix[i][j] == 1) { //go through all friends' friends
					possible_friends[j] = 1; //update the variables
					ok = 0; //there is at least one friend
				}
	}

	if (ok == 0) {
		printf("Suggestions for %s:\n", name1);
		//go through the array of possible friends
		for (int i = 0; i < mg->nodes; i++)
			//if the value is 1 and it is not friend with out initial name
			if (possible_friends[i] == 1 && i != id1 && mg->matrix[id1][i] == 0)
				printf("%s\n", get_user_name(i));
	} else {
		printf("There are no suggestions for %s\n", name1);
	}

	free(possible_friends); //clear the memory
	free(name1);
}

void distance(char *cmd, matrix_graph_t *mg)
{
	cmd = strtok(NULL, "\n "); //get the names
	char *name1 = strdup(cmd);
	cmd = strtok(NULL, "\n ");
	char *name2 = strdup(cmd);

	int id1 = get_user_id(name1); //get the id of each name
	int id2 = get_user_id(name2);

	/* array in which is safed the distance from name1
	to every other name
	-> the vector is initialized in dfs with INF = 999
	on every position
	*/
	int *distance = malloc(sizeof(int) * mg->nodes);
	dfs(mg, id1, distance);

	//if there is no link between the 2 names the value is INF
	if (distance[id2] != INF)
		printf("The distance between %s - %s is %d\n",
			   name1, name2, distance[id2]);
	else
		printf("There is no way to get from %s to %s\n", name1, name2);

	free(distance); //clear the memory
	free(name1);
	free(name2);
}

void common(char *cmd, matrix_graph_t *mg)
{
	cmd = strtok(NULL, "\n "); //get the names
	char *name1 = strdup(cmd);
	cmd = strtok(NULL, "\n ");
	char *name2 = strdup(cmd);

	int id1 = get_user_id(name1); //get the id of each name
	int id2 = get_user_id(name2);

	int ok = 1; //verifies the 2 names have something in common
	for (int i = 0; i < mg->nodes; i++)
		//same column but different lines, link with the same thing
		if (mg->matrix[id1][i] == 1 && mg->matrix[id2][i] == 1) {
			ok = 0;
			break;
		}

	if (ok == 0) {
		printf("The common friends between %s and %s are:\n", name1, name2);
		for (int i = 0; i < mg->nodes; i++)
			if (mg->matrix[id1][i] == 1 && mg->matrix[id2][i] == 1)
				printf("%s\n", get_user_name(i));
	} else {
		printf("No common friends for %s and %s\n", name1, name2);
	}

	free(name1); //clear memory
	free(name2);
}

int friends(char *name1, matrix_graph_t *mg)
{
	int id1 = get_user_id(name1);
	int nr_of_friends = 0; //returned value

	for (int i = 0; i < mg->nodes; i++)
		if (mg_has_edge(mg, id1, i)) //one link = one friend
			nr_of_friends++;

	return nr_of_friends;
}

void popular(char *cmd, matrix_graph_t *mg)
{
	cmd = strtok(NULL, "\n "); //get the name
	char *name1 = strdup(cmd);

	int id1 = get_user_id(name1); //get the id of the name

	//the array that stores the amount of friends of each name
	int *most_popular = calloc(mg->nodes, sizeof(int));

	//the array is completed
	for (int i = 0; i < mg->nodes; i++)
		if (mg_has_edge(mg, id1, i))
			most_popular[i] = friends(get_user_name(i), mg);

	int ok = 1; //verifies if the name is the most popular
	int max_friends = friends(name1, mg);
	int id = id1;
	for (int i = 0; i < mg->nodes; i++)
		if (most_popular[i] > max_friends) {
			max_friends = most_popular[i]; //update the information
			id = i;                        //of the most popular
			ok = 0;
		}

	if (ok == 1)
		printf("%s is the most popular\n", name1);
	else
		printf("%s is the most popular friend of %s\n",
			   get_user_name(id), name1);

	free(most_popular); //clear memory
	free(name1);
}

void handle_input_friends(char *input, matrix_graph_t *mg)
{
	char *commands = strdup(input);
	char *cmd = strtok(commands, "\n ");

	if (!cmd)
		return;

	if (!strcmp(cmd, "add")) {
		add_friend(cmd, mg);
	} else if (!strcmp(cmd, "remove")) {
		remove_friend(cmd, mg);
	} else if (!strcmp(cmd, "suggestions")) {
		suggestions(cmd, mg);
	} else if (!strcmp(cmd, "distance")) {
		distance(cmd, mg);
	} else if (!strcmp(cmd, "common")) {
		common(cmd, mg);
	} else if (!strcmp(cmd, "friends")) {
		cmd = strtok(NULL, "\n ");
		char *name1 = strdup(cmd);

		int nr_of_friends = friends(name1, mg);
		printf("%s has %d friends\n", name1, nr_of_friends);
		free(name1);
	} else if (!strcmp(cmd, "popular")) {
		popular(cmd, mg);
	}

	free(commands);
}

