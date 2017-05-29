
#include "ft_config.h"
#include "ft_common.h"
#include "ft_malloc.h"
#include "ft_list.h"
#include "ft_tasklet.h"

#ifdef __cplusplus
extern "C"{
#endif

int tsklet_init(struct task_let* tasklet)
{
    list_init(&tasklet->head);

#if !defined(WIN_CE) && !defined(WIN32)
    return pthread_mutex_init(&tasklet->lock, NULL);
#else
    InitializeCriticalSection(&tasklet->lock);
    return 0;
#endif
}

int tsklet_invokelater(struct task_let* tasklet, void (*task)(void *), void * arg)
{
    int ret = -1;
    struct task_let_item_t * item;

    item = (struct task_let_item_t *)ft_malloc(sizeof(struct task_let_item_t));
    if (!item) {
#if !defined(WIN_CE) && !defined(WIN32)
        errno = ENOMEM;
#endif
        return ENOMEM;
    }

    item->task = task;
    item->arg = arg;

    ret = pthread_mutex_lock(&tasklet->lock);
    if (ret != 0)
        printf("in %s:%d, ret=%d\n", __FUNCTION__, __LINE__, ret);

    list_add_before(&item->list, &tasklet->head);
    (void)pthread_mutex_unlock(&tasklet->lock);
    return 0;
}

void tsklet_invokeall(struct task_let * tasklet)
{
    int ret = -1;
    struct list_head *i, *temp;
    struct task_let_item_t * item;

    ret = pthread_mutex_lock(&tasklet->lock);
    if (ret != 0)
        printf("in %s:%d, ret=%d\n", __FUNCTION__, __LINE__, ret);

    for_each_list_item_safe(i, temp, &tasklet->head) {
        item = list_entry(i, struct task_let_item_t, list);
        /* _do_start_tm or _do_stop_tm, 
         * ((struct tm_start_info*)tasklet->head->next->arg)->timer is timer
         */
        item->task(item->arg);
        /* i is tasklet->head->next, so _tasket is a task list where each timer
         * in it will be invoked, after invoked , then also need be remove from 
         * g_taskLets
         */
        list_remove(i);
        ft_free(i);
    }
    (void)pthread_mutex_unlock(&tasklet->lock);
}

#ifdef __cplusplus
}
#endif

