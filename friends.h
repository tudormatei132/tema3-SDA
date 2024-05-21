#ifndef FRIENDS_H
#define FRIENDS_H
#include <stdbool.h>
#define MAX_COMMAND_LEN 500
#define MAX_PEOPLE 550

typedef struct
{
    int **matrix; /* Matricea de adiacenta a grafului */
    int nodes;    /* Numarul de noduri din graf. */
} matrix_graph_t;

typedef struct ll_node_t
{
    void* data;
    struct ll_node_t* next;
} ll_node_t;

typedef struct linked_list_t
{
    ll_node_t* head;
    unsigned int data_size;
    unsigned int size;
} linked_list_t;

typedef struct stack_t
{
    struct linked_list_t *list;
}stack;

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
  void** buff;
} queue_t;

linked_list_t * ll_create(unsigned int data_size);
ll_node_t* get_nth_node(linked_list_t* list, unsigned int n);
void ll_add_nth_node(linked_list_t* list, unsigned int n, const void* new_data);
ll_node_t * ll_remove_nth_node(linked_list_t* list, unsigned int n);
unsigned int ll_get_size(linked_list_t* list);
void ll_free(linked_list_t** pp_list);

stack* st_create(unsigned int data_size);
unsigned int st_get_size(stack* st);
unsigned int st_is_empty(stack* st);
void* st_peek(stack* st);
void st_pop(stack* st);
void st_push(stack* st, void* new_data);
void st_clear(stack* st);
void st_free(stack* st);

queue_t* q_create(unsigned int data_size, unsigned int max_size);
unsigned int q_get_size(queue_t* q);
unsigned int q_is_empty(queue_t* q);
void* q_front(queue_t* q);
bool q_dequeue(queue_t* q);
bool q_enqueue(queue_t* q, void* new_data);
void q_clear(queue_t* q);
void q_free(queue_t* q);

matrix_graph_t *mg_create(int nodes);
void mg_add_edge(matrix_graph_t* graph, int src, int dest);
int mg_has_edge(matrix_graph_t* graph, int src, int dest);
void mg_remove_edge(matrix_graph_t* graph, int src, int dest);
void mg_free(matrix_graph_t* graph);
void print_matrix_graph(matrix_graph_t* mg);
void print_matrix_graph(matrix_graph_t* mg);
int ceBOALAai(matrix_graph_t *mg, int node1, int node2, int *distance, int *parents);


/**
 * Function that handles the calling of every command from task 1
 *
 * Please add any necessary parameters to the functions
*/
void handle_input_friends(char *input, matrix_graph_t *mg);

#endif // FRIENDS_H