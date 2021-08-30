#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
queue_t *q_new()
{
    queue_t *q = malloc(sizeof(queue_t));
    if (!q) {
        return NULL;
    }
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    if (!q) {
        return;
    }
    while (q->head) {
        list_ele_t *tmp = q->head;
        q->head = q->head->next;
        free(tmp->value);
        free(tmp);
        q->size--;
    }
    /* Free queue structure */
    free(q);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(queue_t *q, char *s)
{
    if (!q) {
        return false;
    }
    list_ele_t *newh;
    newh = malloc(sizeof(list_ele_t));
    if (!newh) {
        return false;
    }
    /* Don't forget to allocate space for the string and copy it */
    /* What if either call to malloc returns NULL? */
    int len = strlen(s) + 1;
    newh->value = malloc(sizeof(char) * len);
    if (!newh->value) {
        free(newh);
        return false;
    }
    memcpy(newh->value, s, sizeof(char) * (len - 1));
    newh->value[len - 1] = '\0';

    newh->next = q->head;
    q->head = newh;
    q->size++;
    if (!q->tail) {
        q->tail = newh;
    }
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(queue_t *q, char *s)
{
    if (!q) {
        return false;
    }
    list_ele_t *newt = malloc(sizeof(list_ele_t));
    if (!newt) {
        return false;
    }
    int len = strlen(s) + 1;
    newt->value = malloc(sizeof(char) * len);
    newt->next = NULL;  // I miss this and debug long time...
    if (!newt->value) {
        free(newt);
        return false;
    }
    memcpy(newt->value, s, sizeof(char) * (len - 1));
    newt->value[len - 1] = '\0';

    if (!q->tail) {
        q->tail = newt;
        q->head = newt;
    } else {
        q->tail->next = newt;
        q->tail = newt;
    }
    q->size++;

    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return true if successful.
 * Return false if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 * The space used by the list element and the string should be freed.
 */
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    if (!q || !q->head || !sp) {
        return false;
    }
    list_ele_t *tmp = q->head;
    q->head = q->head->next;
    q->size--;
    if (q->size == 0) {
        q->tail = NULL;
    }
    strncpy(sp, tmp->value, bufsize - 1);
    sp[bufsize - 1] = '\0';
    free(tmp->value);
    free(tmp);
    return true;
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    return q ? q->size : 0;
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(queue_t *q)
{
    if (!q) {
        return;
    }
    list_ele_t *l = NULL, *r = q->head;
    q->tail = q->head;
    while (r) {
        list_ele_t *tmp = r->next;
        r->next = l;
        l = r;
        r = tmp;
    }
    q->head = l;
}

static void swap_str(char **a, char **b)
{
    char *tmp = *a;
    *a = *b;
    *b = tmp;
}


// Inplace sort the link list, if carefully choose pivot,
//     the average time complexity is O(nlogn),
//     but worst case is
//     O(n ^ 2)
static void quick_sort(list_ele_t *first, list_ele_t *last)
{
    if (!first || !last) {
        return;
    }
    if (first == last || last->next == first) {
        return;
    }
    // take last as pivot
    list_ele_t *l = first, *r = first, *prev = 0;
    while (r != last) {
        if (strcmp(r->value, last->value) < 0) {
            swap_str(&r->value, &l->value);
            prev = l;
            l = l->next;
        }
        r = r->next;
    }
    swap_str(&l->value, &last->value);

    quick_sort(first, prev);
    quick_sort(l->next, last);
}

// Inplace on link-list, and can make sure time complexity
// is strictly O(nlogn)
static void merge_sort(list_ele_t **head)
{
    if (!head || !*head || !(*head)->next) {
        return;
    }

    // find half
    list_ele_t *left = *head, *right = (*head)->next;
    while (right && right->next) {
        left = left->next;
        right = right->next->next;
    }
    right = left->next;  // reuse, save a pointer
    left->next = NULL;

    merge_sort(head);
    merge_sort(&right);

    // merge
    left = *head;

    while (left && right) {
        if (strcmp(left->value, right->value) < 0) {
            *head = left;
            left = left->next;
        } else {
            *head = right;
            right = right->next;
        }
        head = &(*head)->next;
    }
    *head = left ? left : right;
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(queue_t *q)
{
    if (!q) {
        return;
    }

    if (1)
        merge_sort(&q->head);
    else
        quick_sort(q->head, q->tail);

    while (q->tail && q->tail->next) {
        q->tail = q->tail->next;
    }

    return;
}
