
#ifndef __FT_TIMER_H__
#define __FT_TIMER_H__

#include "ft_list.h"

#ifdef __cplusplus
extern "C"{
#endif

/* 系统时间改变标识 */
#define TM_CHANGED     0
#define TM_UNCHANGE    1

/* timer structure */
struct tm_cb_t {
    struct list_head list;
    unsigned int msec;          /* expire time in ms */
    unsigned int ticks_left;    /* time passed, used in hash link */
    int (*timeout)(void *);
    void * arg;
    int mode;                   /* PETF_ONE_SHOOT, PETF_AUTO_FREE */
    int link;                   /* RELTM_LINK_ZERO or RELTM_LINK_SORTED */
    //unsigned int magic;       /* magic number for reltimer */
    int index;                  /* index in timer array */
};

typedef void* ftimer_t;

int ft_start_timer_task(void* thread, void* pattr);
ftimer_t ft_create_timer(int (*timeout)(void * arg), int flag);
int ft_start_timer(ftimer_t timer, unsigned long msec, void* arg);
int ft_stop_timer(ftimer_t timer, void (* finallize)(void *), void* arg);
int ft_reset_timer(ftimer_t timer, unsigned long msec, void* arg);
int ft_free_timer(ftimer_t timer);
void ft_finish_timer_task(void* thread);

#if !defined(WIN_CE) && !defined(WIN32)
int ft_time_change_init(void); 
void ft_set_time_change_flag(int flag);
#endif
 
#ifdef __cplusplus
}
#endif

#endif // __FT_TIMER_H__

