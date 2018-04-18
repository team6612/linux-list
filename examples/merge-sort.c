#include <assert.h>
#include <stdlib.h>
#include "list.h"

#include "common.h"

static uint16_t values[256];

static void merge(struct list_head *left,
                  struct list_head *right,
                  struct list_head *head)
{
    assert(list_empty(head));

    while (!list_empty(left) || !list_empty(right)) {
        if (list_empty(left) ||
            (!list_empty(right) &&
             list_entry((right)->next, struct listitem, list)->i <
                 list_entry((left)->next, struct listitem, list)->i))
            list_move_tail((right)->next, head);
        else
            list_move_tail((left)->next, head);
    }
}

static void list_msort(struct list_head *head)
{
    struct list_head list_left, list_right;

    if (list_empty(head) || list_is_singular(head))
        return;

    INIT_LIST_HEAD(&list_left);
    INIT_LIST_HEAD(&list_right);

    while (!list_empty(head)) {
        list_move_tail((head)->next, &list_left);
        if (!list_empty(head))
            list_move((head)->prev, &list_right);
    }

    list_msort(&list_left);
    list_msort(&list_right);

    merge(&list_left, &list_right, head);
}

int main(void)
{
    struct list_head testlist;
    struct listitem *item, *is = NULL;
    size_t i;

    random_shuffle_array(values, (uint16_t) ARRAY_SIZE(values));

    INIT_LIST_HEAD(&testlist);

    assert(list_empty(&testlist));

    for (i = 0; i < ARRAY_SIZE(values); i++) {
        item = (struct listitem *) malloc(sizeof(*item));
        assert(item);  // test the successful of malloc
        item->i = values[i];
        list_add_tail(&item->list, &testlist);
    }

    assert(!list_empty(&testlist));

    qsort(values, ARRAY_SIZE(values), sizeof(values[0]), cmpint);
    list_msort(&testlist);

    i = 0;
    list_for_each_entry_safe (item, is, &testlist, list) {
        assert(item->i == values[i]);
        list_del(&item->list);
        free(item);
        i++;
    }

    assert(i == ARRAY_SIZE(values));
    assert(list_empty(&testlist));

    return 0;
}
