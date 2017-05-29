
#ifndef __FT_TSKLET_H__
#define __FT_TSKLET_H__

#include "ft_list.h"

#ifdef __cplusplus
extern "C"{
#endif

struct task_let {
    struct list_head head;
    pthread_mutex_t lock;
};

struct task_let_item_t {
    struct list_head list;
    void (*task)(void *);
    void * arg;
};

extern int tsklet_init(struct task_let * tasklet);
extern int tsklet_invokelater(struct task_let * tasklet, 
        void (*task)(void *), void * arg);
extern void tsklet_invokeall(struct task_let * tasklet);

#ifdef __cplusplus
}
#endif

#endif // __FT_TSKLET_H__

