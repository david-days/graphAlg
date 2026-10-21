//
// Created by david on 10/18/26.
//

#ifndef GRAPHALG_STACK_H
#define GRAPHALG_STACK_H

/**
 * Singly-linked stack node.
 */
typedef struct snode_t snode_t, *snode;

struct snode_t {
    void *data;
    snode next;
};

typedef struct {
    snode top;
} stack_t;

typedef stack_t *stack;

/**
 * Creates a new empty stack.
 * @return stack handle, or NULL on allocation failure
 */
stack s_create(void);

/**
 * Checks if the stack is empty.
 * @param s stack to be checked
 * @return true if empty (or s is NULL), otherwise false
 */
int s_empty(stack s);

/**
 * Frees the stack nodes and handle. Does not free pushed payloads.
 * @param s stack to be freed
 * @return true if successful, otherwise false
 */
int s_free(stack s);

/**
 * Returns the top value without removing it.
 * @param s stack to be used
 * @return top payload, or NULL if empty
 */
void *s_peek(stack s);

/**
 * Push a value onto the top of the stack.
 * @param s stack to be used
 * @param v value to push (caller retains ownership)
 * @return true if successful, otherwise false
 */
int s_push(stack s, void *v);

/**
 * Remove the top item and optionally return its value.
 * @param s stack to be used
 * @param val out-param for the payload; set to NULL on failure. May be NULL.
 * @return true if an item was popped, false if empty/invalid
 */
int s_pop(stack s, void **val);

#endif //GRAPHALG_STACK_H
