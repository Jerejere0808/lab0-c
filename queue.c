#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */



/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head =
        (struct list_head *) malloc(sizeof(struct list_head));
    if (!head) {
        free(head);
        return NULL;
    }
    INIT_LIST_HEAD(head);


    return head;
}


/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    element_t *entry, *safe;


    if (!l || list_empty(l)) {
        free(l);
        return;
    }


    list_for_each_entry_safe (entry, safe, l, list) {
        q_release_element(entry);
    }
    free(l);
    return;
}


/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *new_ele = (element_t *) malloc(sizeof(element_t));
    if (!new_ele)
        return false;
    new_ele->value = (char *) malloc((strlen(s) + 1) * sizeof(char));
    if (!new_ele->value) {
        free(new_ele);
        return false;
    }
    strncpy(new_ele->value, s, (strlen(s) + 1));
    list_add(&new_ele->list, head);
    return true;
}


/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *new_ele = (element_t *) malloc(sizeof(element_t));
    if (!new_ele)
        return false;
    new_ele->value = (char *) malloc((strlen(s) + 1) * sizeof(char));
    if (!new_ele->value) {
        free(new_ele);
        return false;
    }
    strncpy(new_ele->value, s, (strlen(s) + 1));
    list_add_tail(&new_ele->list, head);
    return true;
}


/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *remove_ele = list_first_entry(head, element_t, list);
    list_del(head->next);
    if (sp) {
        strncpy(sp, remove_ele->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return remove_ele;
}


/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *remove_ele = list_last_entry(head, element_t, list);
    list_del(head->prev);
    if (sp) {
        strncpy(sp, remove_ele->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return remove_ele;
}


/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;
    int len = 0;
    struct list_head *li;
    list_for_each (li, head)
        len++;
    return len;
}


/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;
    struct list_head *l = head->next;
    struct list_head *r = head->prev;
    while (1) {
        if (l == r) {
            list_del(l);  // remove list firstly
            q_release_element(list_entry(l, element_t, list));
            break;
        } else if (l->next == r) {  // remove later one
            list_del(r);            // remove list firstly
            q_release_element(list_entry(r, element_t, list));
            break;
        }
        l = l->next;
        r = r->prev;
    }
    return true;
}


/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head))
        return false;
    struct list_head *node, *safe;
    bool dup_last = false;
    list_for_each_safe (node, safe, head) {
        bool dup = false;
        element_t *cur = list_entry(node, element_t, list);
        if (node->next != head) {
            element_t *next = list_entry(node->next, element_t, list);
            dup = !strcmp(cur->value, next->value);
        }
        if (dup || dup_last) {
            list_del(node);
            q_release_element(cur);
        }
        dup_last = dup;
    }
    return true;
}


/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head))
        return;
    struct list_head *cur;
    for (cur = head->next; cur != head && cur->next != head; cur = cur->next) {
        struct list_head *next = cur->next;
        list_del(cur);
        list_add(cur, next);
    }
    return;
}
static inline void swap(struct list_head *n1, struct list_head *n2)
{
    if (n1 == n2)
        return;
    struct list_head *n1_prev = n1->prev;
    struct list_head *n2_prev = n2->prev;
    if (n2->prev != n1)
        list_move(n1, n2_prev);
    list_move(n2, n1_prev);
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head *l = head->next;
    struct list_head *r = head->prev;
    while (1) {
        if (l == r)
            break;
        swap(l, r);
        if (l->prev == r)
            break;
        struct list_head *old_r = r;
        r = l->prev;
        l = old_r->next;
    }
}


/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head)
        return;
    struct list_head *cur, *safe, *h;
    int count = k;
    h = head;
    LIST_HEAD(tmp_h);
    list_for_each_safe (cur, safe, head) {
        if (--count)
            continue;
        count = k;
        list_cut_position(&tmp_h, h, cur);
        q_reverse(&tmp_h);
        list_splice(&tmp_h, h);
        h = safe->prev;
    }
}


/* Sort elements of queue in ascending order */
//#define list_size 1000000

struct list_head *mergeTwoLists(struct list_head *list1,
                                struct list_head *list2)
{
    struct list_head *head;
    char *value1 = list_entry(list1, element_t, list)->value;
    char *value2 = list_entry(list2, element_t, list)->value;
    if (strcmp(value1, value2) <= 0) {
        head = list1;
        list1 = list1->next;
    } else {
        head = list2;
        list2 = list2->next;
    }
    list_del_init(head);
    while (list1->next != head && list2->next != head) {
        value1 = list_entry(list1, element_t, list)->value;
        value2 = list_entry(list2, element_t, list)->value;
        if (strcmp(value1, value2) <= 0) {
            list1 = list1->next;
            list_move_tail(list1->prev, head);
        } else {
            list2 = list2->next;
            list_move_tail(list2->prev, head);
        }
    }
    if (list2->next != head) {
        struct list_head *head_last = head->prev;
        struct list_head *list_last = list2->prev;
        head->prev = list_last;
        list_last->next = head;
        list2->prev = head_last;
        head_last->next = list2;
    } else {
        struct list_head *head_last = head->prev;
        struct list_head *list_last = list1->prev;
        head->prev = list_last;
        list_last->next = head;
        list1->prev = head_last;
        head_last->next = list1;
    }
    return head;
}

void q_sort(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    if (list_is_singular(head))
        return;
    struct list_head *node, *safe;
    list_for_each_safe (node, safe, head) {
        node->prev = node;
    }

    struct list_head *first = head->next;
    INIT_LIST_HEAD(head);

    while (first->prev->next != head) {
        struct list_head **cur;
        struct list_head *next_l, *nnext_l;
        cur = &first;
        next_l = (*cur)->prev->next;
        nnext_l = next_l->prev->next;
        while (*cur != head && next_l != head) {
            (*cur)->prev->next = (*cur);
            next_l->prev->next = next_l;
            *cur = mergeTwoLists(*cur, next_l);

            cur = &((*cur)->prev->next);
            *cur = nnext_l;
            next_l = (*cur)->prev->next;
            nnext_l = next_l->prev->next;
        }
    }
    list_add_tail(head, first);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;

    struct list_head *cur = head->prev;
    int count = 0;
    element_t *MAX = list_last_entry(head, element_t, list);
    while (cur != head) {
        element_t *ele = list_entry(cur, element_t, list);
        cur = cur->prev;
        if (strcmp(ele->value, MAX->value) < 0) {
            list_del(cur->next);
            q_release_element(ele);
        } else {
            MAX = ele;
            count++;
        }
    }

    return count;
}


/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/

    if (!head || list_empty(head))
        return 0;
    queue_contex_t *cur = head->next;
    LIST_HEAD(h);
    list_for_each_entry (cur, head, chain) {
        list_splice_init(cur->q, &h);
    }
    q_sort(&h);
    list_splice_init(&h, list_first_entry(head, queue_contex_t, chain)->q);
    return q_size(list_first_entry(head, queue_contex_t, chain)->q);
}
