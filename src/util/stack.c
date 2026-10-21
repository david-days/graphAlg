#include <stdbool.h>
#include <stdlib.h>
#include <util/stack.h>

stack s_create(void) {
    stack s = malloc(sizeof(stack_t));
    if (s == NULL) {
        return NULL;
    }
    s->top = NULL;
    return s;
}

int s_free(stack s) {
    if (s == NULL) {
        return false;
    }
    snode curr = s->top;
    while (curr != NULL) {
        snode next = curr->next;
        free(curr);
        curr = next;
    }
    free(s);
    return true;
}

int s_empty(stack s) {
    return s == NULL || s->top == NULL;
}

int s_push(stack s, void *v) {
    if (s == NULL) {
        return false;
    }
    snode nd = malloc(sizeof(snode_t));
    if (nd == NULL) {
        return false;
    }
    nd->data = v;
    nd->next = s->top;
    s->top = nd;
    return true;
}

int s_pop(stack s, void **val) {
    if (val != NULL) {
        *val = NULL;
    }
    if (s_empty(s)) {
        return false;
    }

    snode top = s->top;
    if (val != NULL) {
        *val = top->data;
    }
    s->top = top->next;
    free(top);
    return true;
}

void *s_peek(stack s) {
    if (s_empty(s)) {
        return NULL;
    }
    return s->top->data;
}
