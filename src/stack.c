#include "stack.h"
#include "token.h"
#include <stdlib.h>

bool isEmpty(stack_node_t **node) {
    return (*node == NULL);
}

void push(stack_node_t **head, container_type_t val) {
    stack_node_t *new_node = malloc(sizeof(stack_node_t));

    new_node->type = val;
    new_node->next = *head;

    *head = new_node;
}

stack_node_t *pop(stack_node_t **head) {
    stack_node_t *top = *head;
    *head = (*head)->next;

    return top;
}