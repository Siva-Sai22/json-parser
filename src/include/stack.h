#include "token.h"
#include <stdbool.h>

typedef struct stack_node_t stack_node_t;

struct stack_node_t {
	container_type_t type;
	stack_node_t *next;
};

bool isEmpty(stack_node_t **head);
void push(stack_node_t **head, container_type_t val);
stack_node_t *pop(stack_node_t **head);
