#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "friends.h"
#include "users.h"

#define INF 999

typedef struct ll_node_t
{
	void  *data;
	struct ll_node_t *next;
} ll_node_t;

typedef struct linked_list_t
{
	ll_node_t *head;
	unsigned int data_size;
	unsigned int size;
} linked_list_t;

typedef struct stack_t
{
	struct linked_list_t *list;
} stack;

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

matrix_graph_t *mg_create(int nodes)
{
	matrix_graph_t *mg = malloc(sizeof(*mg));

	mg->matrix = calloc(sizeof(int *), nodes);
	for (int i = 0; i < nodes; i++)
		mg->matrix[i] = calloc(sizeof(int), nodes);

	mg->nodes = nodes;
	return mg;
}

/* Adauga o muchie intre nodurile sursa si destinatie */
void
mg_add_edge(matrix_graph_t *graph, int src, int dest)
{
	graph->matrix[src][dest] = 1;
}

/* Returneaza 1 daca exista muchie intre cele doua noduri, 0 in caz contrar */
int
mg_has_edge(matrix_graph_t *graph, int src, int dest)
{
	if (graph->matrix[src][dest] == 1)
		return 1;
	return 0;
}

/* Elimina muchia dintre nodurile sursa si destinatie */
void
mg_remove_edge(matrix_graph_t *graph, int src, int dest)
{
	graph->matrix[src][dest] = 0;
}

/* Elibereaza memoria folosita de matricea de adiacenta a grafului */
void
mg_free(matrix_graph_t *graph)
{
	for (int i = 0; i < graph->nodes; i++)
		free(graph->matrix[i]);
	free(graph->matrix);

	free(graph);
}

void
print_matrix_graph(matrix_graph_t *mg)
{
	for (int i = 0; i < mg->nodes; i++) {
		for (int j = 0; j < mg->nodes; j++)
			printf("%d ", mg->matrix[i][j]);
		printf("\n");
	}
}

linked_list_t *
ll_create(unsigned int data_size)
{
	linked_list_t *ll;

	ll = malloc(sizeof(*ll));

	ll->head = NULL;
	ll->data_size = data_size;
	ll->size = 0;

	return ll;
}

ll_node_t *get_nth_node(linked_list_t *list, unsigned int n)
{
	if (!list)
		return NULL;

	unsigned int len = list->size - 1;
	unsigned int i;
	ll_node_t *node = list->head;

	if (n < len)
		n = n;
	else
		n = len;

	for (i = 0; i < n; ++i)
		node = node->next;

	return node;
}

void
ll_add_nth_node(linked_list_t *list, unsigned int n, const void *new_data)
{
	ll_node_t *prev, *curr;
	ll_node_t *new_node;

	if (!list)
		return;

	/* n >= list->size inseamna adaugarea unui nou nod la finalul listei. */
	if (n > list->size)
		n = list->size;

	curr = list->head;
	prev = NULL;
	while (n > 0) {
		prev = curr;
		curr = curr->next;
		--n;
	}

	new_node = malloc(sizeof(*new_node));
	new_node->data = malloc(list->data_size);
	memcpy(new_node->data, new_data, list->data_size);

	new_node->next = curr;
	if (!prev) {
		/* Adica n == 0. */
		list->head = new_node;
	} else {
		prev->next = new_node;
	}

	list->size++;
}

ll_node_t *
ll_remove_nth_node(linked_list_t *list, unsigned int n)
{
	ll_node_t *prev, *curr;

	if (!list || !list->head)
		return NULL;

	/* n >= list->size - 1 inseamna eliminarea nodului de la finalul listei. */
	if (n > list->size - 1)
		n = list->size - 1;

	curr = list->head;
	prev = NULL;
	while (n > 0) {
		prev = curr;
		curr = curr->next;
		--n;
	}

	if (!prev) {
		/* Adica n == 0. */
		list->head = curr->next;
	} else {
		prev->next = curr->next;
	}

	list->size--;

	return curr;
}

unsigned int
ll_get_size(linked_list_t *list)
{
	if (!list)
		return -1;

	return list->size;
}

void
ll_free(linked_list_t **pp_list)
{
	ll_node_t *curr;

	if (!pp_list || !*pp_list)
		return;

	while (ll_get_size(*pp_list) > 0) {
		curr = ll_remove_nth_node(*pp_list, 0);
		free(curr->data);
		curr->data = NULL;
		free(curr);
		curr = NULL;
	}

	free(*pp_list);
	*pp_list = NULL;
}

//---------stack functions

stack *
st_create(unsigned int data_size)
{
	stack *st = malloc(sizeof(*st));
	st->list = ll_create(data_size);

	return st;
}

unsigned int
st_get_size(stack *st)
{
	if (!st || !st->list)
		return 0;
	return st->list->size;
}

unsigned int
st_is_empty(stack *st)
{
	return !st || !st->list || !st->list->size;
}

void *
st_peek(stack *st)
{
	if (!st || !st->list || !st->list->size)
		return NULL;

	return st->list->head->data;
}

void
st_pop(stack *st)
{
	if (!st || !st->list)
		return;

	ll_node_t *rem = ll_remove_nth_node(st->list, 0);
	free(rem->data);
	free(rem);
}

void
st_push(stack *st, void *new_data)
{
	if (!st || !st->list)
		return;

	ll_add_nth_node(st->list, 0, new_data);
}

void
st_clear(stack *st)
{
	if (!st || !st->list)
		return;

	ll_free(&st->list);
}

void
st_free(stack *st)
{
	if (!st || !st->list)
		return;

	ll_free(&st->list);
	free(st);
}

//---------queue functions

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
	for (int i = 0; i < mg->nodes; i++) {
		color[i] = 0;
		distance[i] = INF;
	}

	queue_t *Q = q_create(sizeof(int), mg->nodes);
	color[node1] = 1;
	distance[node1] = 0;

	q_enqueue(Q, &node1);

	while (Q->size != 0) {
		int v = *(int *)q_front(Q);
		q_dequeue(Q);

		for (int i = 0; i < mg->nodes; i++) {
			if (mg_has_edge(mg, v, i)) {
				if (color[i] == 0) {
					color[i] = 1;
					distance[i] = distance[v] + 1;
					q_enqueue(Q, &i);
				}
			}
		}
		color[v] = 2;
	}
	q_free(Q);
	free(color);
}

void add_friend(char *cmd, matrix_graph_t *mg)
{
	cmd = strtok(NULL, "\n ");
	char *name1 = strdup(cmd);
	cmd = strtok(NULL, "\n ");
	char *name2 = strdup(cmd);

	int id1 = get_user_id(name1);
	int id2 = get_user_id(name2);

	mg_add_edge(mg, id1, id2);
	mg_add_edge(mg, id2, id1);

	if (mg->matrix[id2][id1] == 1 && mg->matrix[id1][id2] == 1)
		printf("Added connection %s - %s\n", name1, name2);

	free(name1);
	free(name2);
}

void remove_friend(char *cmd, matrix_graph_t *mg)
{
	cmd = strtok(NULL, "\n ");
	char *name1 = strdup(cmd);
	cmd = strtok(NULL, "\n ");
	char *name2 = strdup(cmd);

	int id1 = get_user_id(name1);
	int id2 = get_user_id(name2);

	mg_remove_edge(mg, id1, id2);
	mg_remove_edge(mg, id2, id1);

	if (mg->matrix[id2][id1] == 0 && mg->matrix[id1][id2] == 0)
		printf("Removed connection %s - %s\n", name1, name2);

	free(name1);
	free(name2);
}

void suggestions(char *cmd, matrix_graph_t *mg)
{
	cmd = strtok(NULL, "\n ");
	char *name1 = strdup(cmd);

	int id1 = get_user_id(name1);

	int *possible_friends = calloc(mg->nodes, sizeof(int));
	int ok = 1;

	for (int i = 0; i < mg->nodes; i++) {
		if (mg->matrix[id1][i])
			for (int j = 0; j < mg->nodes; j++)
				if (mg->matrix[i][j] == 1) {
					possible_friends[j] = 1;
					ok = 0;
				}
	}

	if (ok == 0) {
		printf("Suggestions for %s:\n", name1);
		for (int i = 0; i < mg->nodes; i++)
			if (possible_friends[i] == 1 && i != id1 && mg->matrix[id1][i] == 0)
				printf("%s\n", get_user_name(i));
	} else {
		printf("There are no suggestions for %s\n", name1);
	}

	free(possible_friends);
	free(name1);
}

void distance(char *cmd, matrix_graph_t *mg)
{
	cmd = strtok(NULL, "\n ");
	char *name1 = strdup(cmd);
	cmd = strtok(NULL, "\n ");
	char *name2 = strdup(cmd);

	int id1 = get_user_id(name1);
	int id2 = get_user_id(name2);

	int *distance = malloc(sizeof(int) * mg->nodes);
	dfs(mg, id1, distance);

	if (distance[id2] != INF)
		printf("The distance between %s - %s is %d\n",
			   name1, name2, distance[id2]);
	else
		printf("There is no way to get from %s to %s\n", name1, name2);

	free(distance);
	free(name1);
	free(name2);
}

void common(char *cmd, matrix_graph_t *mg)
{
	cmd = strtok(NULL, "\n ");
	char *name1 = strdup(cmd);
	cmd = strtok(NULL, "\n ");
	char *name2 = strdup(cmd);

	int id1 = get_user_id(name1);
	int id2 = get_user_id(name2);

	int ok = 1;
	for (int i = 0; i < mg->nodes; i++)
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

	free(name1);
	free(name2);
}

int friends(char *name1, matrix_graph_t *mg)
{
	int id1 = get_user_id(name1);
	int nr_of_friends = 0;

	for (int i = 0; i < mg->nodes; i++)
		if (mg_has_edge(mg, id1, i))
			nr_of_friends++;

	return nr_of_friends;
}

void popular(char *cmd, matrix_graph_t *mg)
{
	cmd = strtok(NULL, "\n ");
	char *name1 = strdup(cmd);

	int id1 = get_user_id(name1);
	int *most_popular = calloc(mg->nodes, sizeof(int));

	for (int i = 0; i < mg->nodes; i++)
		if (mg_has_edge(mg, id1, i))
			most_popular[i] = friends(get_user_name(i), mg);

	int ok = 1;
	int max_friends = friends(name1, mg);
	int id = id1;
	for (int i = 0; i < mg->nodes; i++)
		if (most_popular[i] > max_friends) {
			max_friends = most_popular[i];
			id = i;
			ok = 0;
		}

	if (ok == 1)
		printf("%s is the most popular\n", name1);
	else
		printf("%s is the most popular friend of %s\n",
			   get_user_name(id), name1);

	free(most_popular);
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

