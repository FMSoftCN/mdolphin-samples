
#ifndef __FT_LIST_H__
#define __FT_LIST_H__

#ifdef __cplusplus
extern "C"{
#endif

/** The struct of a list item*/
struct list_head {
     struct list_head * next, *prev;
};

/** init the list item of list head */
#define list_init(head) /*lint -save -e717*/ \
do { \
    (head)->next = (head)->prev = (head); \
}while(0)/*lint -restore*/

/** Add an item to a list after a special location */
#define list_add(item, where) /*lint -save -e717*/ \
do { \
       (item)->next = (where)->next; \
       (item)->prev = (where); \
       (where)->next = (item); \
       (item)->next->prev = (item); \
}while(0)/*lint -restore*/

/** Add an item to a list before a special location */
#define list_add_before(item, where) list_add(item, (where)->prev)

/** remove an item from a list */
#define list_remove(item) /*lint -save -e717*/ \
do { \
        (item)->prev->next = (item)->next; \
        (item)->next->prev = (item)->prev; \
}while(0)/*lint -restore*/

/** true if the list is empty */
#define list_isempty(head) ((head)->next == (head))

/** true if an item is just init and wasn't added to the queue */
#define list_isinqueue(item) ((item)->next != (item))

/** traval a list, but you cannot remove item while travaling */
#define for_each_list_item(item, head) \
        for(item=(head)->next; item!=head; item = (item)->next)

/** traval a list, you can remove item while travaling */
#define for_each_list_item_safe(item, temp, head)\
        for( (item) = (head)->next, (temp) = (item)->next; \
             (item) != (head); \
             (item) = (temp), (temp) = (item)->next )

/** get the entry by list field */
#define list_entry(item, type, member)\
        ((type *)((char *)item - (char *)(&((type *)0)->member)))

#ifdef __cplusplus
}
#endif

#endif // __FT_LIST_H__

