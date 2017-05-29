
#include "ft_config.h"
#include "ft_common.h"
#include "ft_timer.h"
#include "ft_tasklet.h"
#include "ft_list.h"
#include <stdlib.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C"{
#endif

struct tm_start_info {
    struct tm_cb_t* timer;
    unsigned long msec;
    void* arg;
};

struct tm_stop_info {
    struct tm_cb_t* timer;
    void (*finallize)(void *);
    void* arg;
};

struct timer_hook_t {
    struct timer_hook_t* next;
    void (*hook)(long);
    long interval;
    long escape;
};

struct tm_hash_t {
    struct list_head zero_link;
    struct list_head sorted_link;
};

#define tm_list_entry(item) list_entry((item), struct tm_cb_t, list)

/* the max number of timers */
#define FT_MAX_TIMER_NUM  2000

#define FTF_ONE_SHOOT 0x1  /* the timer start once and then stop */
#define FTF_AUTO_FREE 0x2  /* the timer start once and then free */
#define FT_LINK_ZERO        1

#define TM_HASH_LEN         (1 << PERELTM_HASH_BITS)
#define TM_HASH_MASK        (TM_HASH_LEN - 1)
#define TM_TMLEN_MASK       (~TM_HASH_MASK)
#define TM_HASH_FUNC(value) ((value) & TM_HASH_MASK)

#define TIMER_LINK_ZERO     0
#define TIMER_LINK_SORTED   1

#define MSEC_PER_TICK       (1000/TICKS_PER_SECOND)
#define USEC_PER_TICK       (1000*MSEC_PER_TICK)
#define USEC_PER_SECOND     1000000

#if !defined(WIN_CE) && !defined(WIN32)
static void* _timer_thread(void * arg);
#else
static void CALLBACK _timer_win(unsigned id, unsigned msg, DWORD user, DWORD dw1, DWORD dw2);
#endif

static int init_timer(void);
static int init_timer_ids();

static int start_timer_task(pthread_t * thread, const pthread_attr_t * attr);
static int _register_tickhook(void (*hook)(long), long tick);
static void _fire_all_timers(long escape);
static void _timer_hook(long escape);
static void _fire_a_timer(struct tm_cb_t* timer);

static ftimer_t _create_timer(int (*timeout)(void *), int mode);
static int _start_timer(ftimer_t timer, unsigned msec, void* arg);
static int _stop_timer(ftimer_t timer, void (*finallize)(void *), void * arg);
static int _free_timer(ftimer_t timer);

static void _do_start_tm(struct tm_start_info * info);
static void _do_stop_tm(struct tm_stop_info * info);
static void _do_free_tm(struct tm_cb_t * timer);
static void _do_restart_tm(struct tm_cb_t * timer);

static void _del_from_hash(struct tm_cb_t* timer);
static void _add2hash(struct tm_cb_t* timer);

#if !defined(WIN_CE) && !defined(WIN32)
static pthread_mutex_t g_timeChangeLock;
static unsigned g_timeChangeFlag;
#endif

static int g_instanceCounter = 0; /* currently, the module support one instance only. */

static int g_maxTimerNum = FT_MAX_TIMER_NUM;
static int g_timerNum = 0; /* the number of the registered timers. */

static int* g_availHead = 0; /* avaliable list head */
static int* g_availTail = 0; /* avaliable list tail */
static int* g_indexTimer = 0; /* timer index */

static pthread_mutex_t g_indexLock; /* 对索引表操作实施保护 */
static unsigned int g_tickCount; /* ticks last since the module start */

/** 
 * I'm not going to protect this list with lock, the user should register
 * all hooks before start the thread 
 */
static struct timer_hook_t* g_hookHead = NULL;

static struct tm_hash_t* g_timerHash;
static struct task_let g_taskLets;
static unsigned g_lastHash = 0xffffffff;

int ft_start_timer_task(void* thread, void* pattr)
{
    return start_timer_task(thread, pattr);
}

ftimer_t ft_create_timer(int (*timeout)(void * arg), int flag)
{
    ftimer_t ulRet;

    if (g_timerNum >= g_maxTimerNum || g_timerNum >= FT_MAX_TIMER_NUM)
        return NULL;

    ulRet = _create_timer(timeout, flag);
    if (NULL != ulRet)
        g_timerNum++;

    return ulRet;
}

int ft_start_timer(ftimer_t timer, unsigned long msec, void* arg)
{
    return _start_timer(timer, msec, arg);
}

int ft_stop_timer(ftimer_t timer, void (* finallize)(void *), void* arg)
{
    return _stop_timer(timer, finallize, arg);
}

int ft_reset_timer(ftimer_t timer, unsigned long msec, void* arg)
{
    NOT_IMPLEMENT();
	return 0;
}

int ft_free_timer(ftimer_t timer)
{
    return _free_timer(timer);
}

void ft_finish_timer_task(void* thread)
{
#if !defined(WIN_CE) && !defined(WIN32)
    pthread_cancel((pthread_t*)thread);
#else
    NOT_IMPLEMENT();
#endif
}

static int start_timer_task(pthread_t* thread, const pthread_attr_t* attr)
{
    int ret;

    if (g_instanceCounter > 0)
        return 0;

    g_instanceCounter++;

    ret = init_timer();
    if (ret)
        return ret;

#if !defined(WIN_CE) && !defined(WIN32)
    ret = ft_time_change_init();
    if (ret)
        return ret;

    return pthread_create(thread, attr, _timer_thread, NULL);
#else
    return !(int)timeSetEvent(MSEC_PER_TICK, 0, _timer_win, 0, TIME_PERIODIC);
#endif
}

#if !defined(WIN_CE) && !defined(WIN32)
static void* _timer_thread(void * arg)
{
    struct timeval tickval;
    struct timeval fixed, last, ready;

    int ticks;
    int tickfix = 0, triggerfix = 0;
    int selectfix = 0, selecttemp = 0;
    /*for PC-LINT*/
    arg = arg;

/*  result will overflow if time elapsed more than 42 seconds  */
#define time_elapsed_us(after, before)\
    (((after).tv_sec - (before).tv_sec)* USEC_PER_SECOND \
            + (after).tv_usec - (before).tv_usec)

    (void)gettimeofday(&last, NULL);
    for (;;) {
        (void)gettimeofday(&fixed, NULL);
        /* 判断系统时间是否被修改 */
        if (TM_CHANGED == g_timeChangeFlag) {
            ft_set_time_change_flag(TM_UNCHANGE);
            last = fixed;
        }

        tickval.tv_sec = 0;

        /* trigger_timer()的误差补偿 */
        triggerfix = time_elapsed_us(fixed, last)/USEC_PER_TICK;
        tickval.tv_usec = USEC_PER_TICK - (time_elapsed_us(fixed, last)%USEC_PER_TICK);
        selecttemp = tickval.tv_usec;

        (void)select(0, NULL, NULL, NULL, &tickval); /* I don't care if it is fail */
        (void)gettimeofday(&ready, NULL);

        /* 判断系统时间是否被修改 */
        if (TM_CHANGED == g_timeChangeFlag) {
            ft_set_time_change_flag(TM_UNCHANGE);
            fixed = ready;
        }

        /* select 的 误差修正 */
        selectfix += selecttemp - time_elapsed_us(ready, fixed);
        if (selectfix <= (0 - USEC_PER_TICK)) {
            selectfix += USEC_PER_TICK;
            tickfix = 1;
        } else if (selectfix >= USEC_PER_TICK) {
            selectfix -= USEC_PER_TICK;
            tickfix = -1;
        } else
            tickfix = 0;

        ticks = triggerfix + tickfix + 1;
        if (ticks)
            _fire_all_timers(ticks);

        last = ready;
    }
}

int ft_time_change_init(void) 
{
    g_timeChangeFlag = TM_UNCHANGE;
    return pthread_mutex_init(&g_timeChangeLock, NULL);
}

void ft_set_time_change_flag(int flag) 
{
    int ret = -1;

    ret = pthread_mutex_lock(&g_timeChangeLock);
    if (ret != 0)
        printf("in %s:%d, ret=%d\n", __FUNCTION__, __LINE__, ret);

    g_timeChangeFlag = flag;
    (void)pthread_mutex_unlock(&g_timeChangeLock);
    return;
}
#else
static void CALLBACK _timer_win(UINT uTimerID, UINT uMsg, DWORD dwUser,
    DWORD dw1, DWORD dw2)
{
    _fire_all_timers(1);
}
#endif

static int init_timer(void)
{
    int i, ret;

    ret = _register_tickhook(_timer_hook, 1);
    if (ret)
        return ret;

    ret = tsklet_init(&g_taskLets);
    if (ret) {
        /* the hook is not free, this may cause memory leak, 
         * but If this happen, you can do nothing */
        return ret;
    }

    ret = init_timer_ids();
    if (ret)
        return ret;

    g_tickCount = 0;

    g_timerHash = (struct tm_hash_t *)ft_malloc(sizeof(struct tm_hash_t)*TM_HASH_LEN);
    if (!g_timerHash) {
        if (g_indexTimer) {
            ft_free(g_indexTimer);
            g_indexTimer = 0;
        }
        g_availHead = g_availTail = 0;

#if !defined(WIN_CE) && !defined(WIN32)
        errno = ENOMEM;
#endif
        return ENOMEM;
    }

    for(i=0; i<TM_HASH_LEN; i++) {
        list_init(&g_timerHash[i].zero_link);
        list_init(&g_timerHash[i].sorted_link);
    }
    return 0;
}

static int _register_tickhook(void (*hook)(long), long tick)
{
    struct timer_hook_t * hookitem;

    if (hook == NULL || tick==0)
        return EINVAL;

    SAFE_MALLOC_RETURNABLE(hookitem, struct timer_hook_t, ENOMEM);
    hookitem->interval = tick;
    hookitem->hook     = hook;
    hookitem->escape   = 0;
    hookitem->next = g_hookHead;
    g_hookHead = hookitem;
    return 0;
}

static int init_timer_ids()
{
    int i = 0;

#if !defined(WIN_CE) && !defined(WIN32)
    pthread_mutex_init(&g_indexLock, NULL);
#else
    InitializeCriticalSection(&g_indexLock);
#endif

    if (g_indexTimer)
        ft_free(g_indexTimer);

    g_indexTimer = ft_malloc(g_maxTimerNum*sizeof(int));
    if (!g_indexTimer) {
        errno = ENOMEM;
        return errno;
    }

    //初始化数组，将数组的内容指向下一个节点的地址
    for (i = 0; i < g_maxTimerNum; i++ )
        g_indexTimer[i] = (int)(g_indexTimer + i + 1);

    //最后一个节点指向0
    g_indexTimer[g_maxTimerNum- 1] = 0;

    //指向空闲节点头
    g_availHead = &g_indexTimer[0];

    //指向空闲节点尾
    g_availTail = &g_indexTimer[g_maxTimerNum - 1];
    return 0;
}

static void _fire_all_timers(long escape)
{
    struct timer_hook_t* i = g_hookHead;
    while (i) {
        i->escape += escape;
        if (i->escape>=i->interval) {
            i->hook(i->escape);
            i->escape=0;
        }
        i = i->next;
    }
}

static void _timer_hook(long escape)
{
    long i;
    struct list_head* item, *temp;

    /* trigger for every tick */
    for(i=0; i<escape; i++, g_tickCount++) {
        tsklet_invokeall(&g_taskLets);

        /* traval the zero link, all timers in it should be triggered.
         *
         * i use safe loop because _trigger_a_timer() may remove the item,
         * same as sorted link
         */
        for_each_list_item_safe(item, temp, &g_timerHash[TM_HASH_FUNC(g_tickCount)].zero_link) {
            _fire_a_timer(tm_list_entry(item));
        }

        /* traval the sorted link */
        for_each_list_item_safe(item, temp, &g_timerHash[TM_HASH_FUNC(g_tickCount)].sorted_link) {
            if (tm_list_entry(item)->ticks_left < TM_HASH_LEN)
                _fire_a_timer(tm_list_entry(item));
            else {
                tm_list_entry(item)->ticks_left -= TM_HASH_LEN;
                break;
            }
        }
    }
}

static void _fire_a_timer(struct tm_cb_t* timer)
{ 
    int ret = -1;
    int index;

	/* 在超时回调函数中停止定时器，返回成功，但定时器并没有被停止 */
	list_remove(&timer->list);
	list_init(&timer->list);

    if ((!(timer->mode & FTF_ONE_SHOOT)) && (!(timer->mode & FTF_AUTO_FREE)))
        (void)tsklet_invokelater(&g_taskLets, (void (*)(void *))_do_restart_tm, timer);

    //先保存index，避免在timeout中被修改
    index = timer->index;
    timer->timeout(timer->arg);

    if (timer->mode & FTF_AUTO_FREE) {
        //如果没有被释放
        if (g_indexTimer[index]) {
            ret = pthread_mutex_lock(&g_indexLock);
            if (ret)
				printf("in %s:%d, ret=%d\n", __FUNCTION__, __LINE__, ret);

            //如果链表为空
            if (!g_availTail)
                g_availHead = g_availTail = &g_indexTimer[timer->index];
            else {
                //将索引释放到空闲链表末尾
                *g_availTail = (int)(&g_indexTimer[timer->index]);
                g_availTail = &g_indexTimer[timer->index];        
            }

            //将空闲链表尾置0
            *g_availTail = 0;

            ret = pthread_mutex_unlock(&g_indexLock);
            if (ret)
                printf("in %s:%d\n, ret=%d\n", __FUNCTION__, __LINE__, ret);

            (void)tsklet_invokelater(&g_taskLets, (void (*)(void *))_do_free_tm, timer);
        }
    } else if(!(timer->mode & FTF_ONE_SHOOT)) {
    }
}

static ftimer_t _create_timer(int (*timeout)(void *), int mode)
{
    struct tm_cb_t * tmcb;
    int ret = -1;
    int index = 0;

    // timer分配已满
    if (!g_availHead)
        return NULL;

    if (!timeout) {
#if !defined(WIN_CE) && !defined(WIN32)
        errno = EINVAL;
#endif
        return NULL;
    }

    SAFE_MALLOC_RETURNABLE(tmcb, struct tm_cb_t, NULL);
    list_init(&tmcb->list);
    tmcb->mode = (unsigned char)mode;
    tmcb->timeout = timeout;

    ret = pthread_mutex_lock(&g_indexLock);

    //找到空闲链表头部，并分离给timer
    index = g_availHead - g_indexTimer;
    //空闲链表头指向下一个节点
    g_availHead = (int*)(g_indexTimer[index]);
    //如果再无空闲节点，则将头尾节点置空
    if (!g_availHead)
        g_availTail = 0;
    tmcb->index = index;
    //将空闲节点指向timer
    g_indexTimer[index] = (int)tmcb;

   (void)pthread_mutex_unlock(&g_indexLock);
    return &g_indexTimer[index];
}

static int _start_timer(ftimer_t timer, unsigned msec, void* arg)
{
    int ret;
    struct tm_start_info * info;

    /*check if the arg is valid*/    
    if (NULL == timer || 0 == *(int*)timer)
        return EINVAL;

    SAFE_MALLOC_RETURNABLE(info, struct tm_start_info, ENOMEM);
    info->timer = (struct tm_cb_t *)(*(int*)timer);        
    info->msec = msec;
    info->arg = arg;
    ret = tsklet_invokelater(&g_taskLets, (void (*)(void *))_do_start_tm, info);
    if (ret)
        ft_free(info);

    return ret;
}

static int _stop_timer(ftimer_t timer, void (*finallize)(void *), void * arg)
{
    int ret;
    struct tm_stop_info * info;

    /*check if the arg is valid*/    
    if (NULL == timer || 0 == *(int*)timer)
        return EINVAL;

    SAFE_MALLOC_RETURNABLE(info, struct tm_stop_info, ENOMEM);
    info->timer = (struct tm_cb_t *)(*(int*)timer);
    info->finallize = finallize;
    info->arg = arg;
    ret = tsklet_invokelater(&g_taskLets, (void (*)(void *))_do_stop_tm, info);
    if (ret)
        ft_free(info);

    return ret;
}

static int _free_timer(ftimer_t timer)
{
    int* index;
    int ret = -1;

    /*check if the arg is valid,防止重复释放 */    
    if ((NULL == timer) || (0 == *(int*)timer))
        return EINVAL;

    ret = pthread_mutex_lock(&g_indexLock);
    if (ret != 0)
        printf("in %s:%d, ret=%d\n", __FUNCTION__, __LINE__, ret);

    //如果链表为空
    if (!g_availTail)
        g_availHead = g_availTail = timer;
    else {
        //将索引释放到空闲链表末尾
        *g_availTail = (int)timer;
        g_availTail = timer;        
    }

    //保存索引里面的timer地址，并将索引的内容置空
    index = (int*)(*(int*)timer);
    //由于可能在释放任务开始之前又调用该接口，所以此处在创建释放任务时就将索引内容置空
    *((int*)timer) = 0;

    (void)pthread_mutex_unlock(&g_indexLock);
    return tsklet_invokelater(&g_taskLets, (void (*)(void *))_do_free_tm, index);
}

static void _do_start_tm(struct tm_start_info * info)
{
    if (list_isinqueue(&info->timer->list)) {
        /* the timer in queue? restart it */
        _del_from_hash(info->timer);
    }

    info->timer->msec = info->msec;
    info->timer->arg = info->arg;
    _add2hash(info->timer);

    ft_free(info);
}

static void _do_stop_tm(struct tm_stop_info * info)
{
    if (list_isinqueue(&info->timer->list))
        _del_from_hash(info->timer);

    if (info->finallize)
        info->finallize(info->arg);

    ft_free(info);
}

static void _do_restart_tm(struct tm_cb_t * timer)
{
    _add2hash(timer);
}

static void _do_free_tm(struct tm_cb_t * timer)
{
    if (list_isinqueue(&timer->list))
        _del_from_hash(timer);
    
    ft_free(timer);
    g_timerNum--;
}

static void _add2hash(struct tm_cb_t* timer)
{
    unsigned ttick, i;
    struct list_head * item;

    /* milliseconds to ticks */
    ttick = (timer->msec + MSEC_PER_TICK/2)/MSEC_PER_TICK;
    ttick = ttick ? ttick : 1;
    i = TM_HASH_FUNC(g_tickCount + ttick);

    /* ticks left for this timer is equal to the hash index plus how many
     * times the "ttick" is greater than the size of hash pool. 
     *
     * one tick is removed from "ttick" is to avoid travalling extra cycle on
     * the the hash pool */
    timer->ticks_left = i + ((ttick-1) & TM_TMLEN_MASK);
    if (ttick > TM_HASH_LEN) {
		for_each_list_item(item, &g_timerHash[i].sorted_link) {
            if(timer->ticks_left < tm_list_entry(item)->ticks_left) {
                break;
            } else
                timer->ticks_left -= tm_list_entry(item)->ticks_left & TM_TMLEN_MASK;
        }

        timer->link = TIMER_LINK_SORTED;
        list_add_before(&timer->list, item);
        if (item != &g_timerHash[i].sorted_link)
            tm_list_entry(item)->ticks_left -= timer->ticks_left & TM_TMLEN_MASK;
    } else {
        if ((int)timer == g_indexTimer[0] || (int)timer == g_indexTimer[1]) {
            if ((int)timer == g_indexTimer[1])
                g_lastHash = i;
        }

        timer->link = FT_LINK_ZERO;
        list_add_before(&timer->list, &g_timerHash[i].zero_link);
    }
}

static void _del_from_hash(struct tm_cb_t* timer)
{
	if ((int)timer == g_indexTimer[0] || (int)timer == g_indexTimer[1]) {
        if (timer == g_indexTimer[1])
            g_lastHash = 0xffffffff;
    }

    list_remove(&timer->list); //delink hash from timer. zhoupeng 20060323
    list_init(&timer->list);   //delink timer from hash

    if (TIMER_LINK_SORTED == timer->link) {
        if (timer->list.next != &g_timerHash[TM_HASH_FUNC(timer->ticks_left)].sorted_link)
            tm_list_entry(timer->list.next)->ticks_left += timer->ticks_left & TM_TMLEN_MASK;
    }
}

#ifdef __cplusplus
}
#endif

