/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id$
 *
 * Copyright (C) 2006 Dan Everton
 *
 * All files in this archive are subject to the GNU General Public License.
 * See the file COPYING in the source tree root for full license agreement.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ****************************************************************************/

#include <stdbool.h>
#include <time.h>
#include <SDL.h>
#include <SDL_thread.h>
#include <stdlib.h>
#include <memory.h>
#include <setjmp.h>
#include "thread-sdl.h"
#include "kernel.h"
#include "thread.h"
#include "debug.h"

/* Define this as 1 to show informational messages that are not errors. */
#define THREAD_SDL_DEBUGF_ENABLED 0

#if THREAD_SDL_DEBUGF_ENABLED
#define THREAD_SDL_DEBUGF(...) DEBUGF(__VA_ARGS__)
static char __name[32];
#define THREAD_SDL_GET_NAME(thread) \
    ({ thread_get_name(__name, sizeof(__name)/sizeof(__name[0]), thread); __name; })
#else
#define THREAD_SDL_DEBUGF(...)
#define THREAD_SDL_GET_NAME(thread)
#endif

#define THREAD_PANICF(str...) \
    ({ fprintf(stderr, str); exit(-1); })

/* Thread entries as in core */
struct thread_entry threads[MAXTHREADS];
/* Jump buffers for graceful exit - kernel threads don't stay neatly
 * in their start routines responding to messages so this is the only
 * way to get them back in there so they may exit */
static jmp_buf thread_jmpbufs[MAXTHREADS];
static SDL_mutex *m;
static struct thread_entry *running;
static bool threads_exit = false;

extern long start_tick;

void thread_sdl_shutdown(void)
{
    int i;
    /* Take control */
    SDL_LockMutex(m);

    /* Tell all threads jump back to their start routines, unlock and exit
       gracefully - we'll check each one in turn for it's status. Threads
       _could_ terminate via remove_thread or multiple threads could exit
       on each unlock but that is safe. */
    threads_exit = true;

    for (i = 0; i < MAXTHREADS; i++)
    {
        struct thread_entry *thread = &threads[i];
        if (thread->context.t != NULL)
        {
            /* Signal thread on delay or block */
            SDL_Thread *t = thread->context.t;
            SDL_CondSignal(thread->context.c);
            SDL_UnlockMutex(m);
            /* Wait for it to finish */
            SDL_WaitThread(t, NULL);
            /* Relock for next thread signal */
            SDL_LockMutex(m);
        }        
    }

    SDL_UnlockMutex(m);
    SDL_DestroyMutex(m);
}

/* Do main thread creation in this file scope to avoid the need to double-
   return to a prior call-level which would be unaware of the fact setjmp
   was used */
extern void app_main(void *param);
static int thread_sdl_app_main(void *param)
{
    SDL_LockMutex(m);
    running = &threads[0];

    /* Set the jump address for return */
    if (setjmp(thread_jmpbufs[0]) == 0)
    {
        app_main(param);
        /* should not ever be reached but... */
        THREAD_PANICF("app_main returned!\n");
    }

    /* Unlock and exit */
    SDL_UnlockMutex(m);
    return 0;
}

/* Initialize SDL threading */
bool thread_sdl_init(void *param)
{
    memset(threads, 0, sizeof(threads));

    m = SDL_CreateMutex();

    if (SDL_LockMutex(m) == -1)
    {
        fprintf(stderr, "Couldn't lock mutex\n");
        return false;
    }

    /* Slot 0 is reserved for the main thread - initialize it here and
       then create the SDL thread - it is possible to have a quick, early
       shutdown try to access the structure. */
    running = &threads[0];
    running->stack = "       ";
    running->stack_size = 8;
    running->name = "main";
    running->statearg = STATE_RUNNING;
    running->context.c = SDL_CreateCond();

    if (running->context.c == NULL)
    {
        fprintf(stderr, "Failed to create main condition variable\n");
        return false;
    }

    running->context.t = SDL_CreateThread(thread_sdl_app_main, param);

    if (running->context.t == NULL)
    {
        fprintf(stderr, "Failed to create main thread\n");
        return false;
    }

    THREAD_SDL_DEBUGF("Main thread: %p\n", running);

    SDL_UnlockMutex(m);
    return true;
}

static int find_empty_thread_slot(void)
{
    int n;

    for (n = 0; n < MAXTHREADS; n++)
    {
        if (threads[n].name == NULL)
            break;
    }

    return n;
}

static void add_to_list(struct thread_entry **list,
                        struct thread_entry *thread)
{
    if (*list == NULL)
    {
        /* Insert into unoccupied list */
        thread->next = thread;
        thread->prev = thread;
        *list = thread;
    }
    else
    {
        /* Insert last */
        thread->next = *list;
        thread->prev = (*list)->prev;
        thread->prev->next = thread;
        (*list)->prev = thread;
    }
}

static void remove_from_list(struct thread_entry **list,
                             struct thread_entry *thread)
{
    if (thread == thread->next)
    {
        /* The only item */
        *list = NULL;
        return;
    }

    if (thread == *list)
    {
        /* List becomes next item */
        *list = thread->next;
    }

    /* Fix links to jump over the removed entry. */
    thread->prev->next = thread->next;
    thread->next->prev = thread->prev;
}

struct thread_entry *thread_get_current(void)
{
    return running;
}

void thread_sdl_lock(void)
{
    SDL_LockMutex(m);
}

void thread_sdl_unlock(void)
{
    SDL_UnlockMutex(m);
}

void switch_thread(bool save_context, struct thread_entry **blocked_list)
{
    struct thread_entry *current = running;

    SDL_UnlockMutex(m);
    /* Any other thread waiting already will get it first */
    SDL_LockMutex(m);
    running = current;

    if (threads_exit)
        remove_thread(NULL);

    (void)save_context; (void)blocked_list;
}

void sleep_thread(int ticks)
{
    struct thread_entry *current;
    int rem;

    current = running;
    current->statearg = STATE_SLEEPING;

    rem = (SDL_GetTicks() - start_tick) % (1000/HZ);
    if (rem < 0)
        rem = 0;

    rem = (1000/HZ) * ticks + ((1000/HZ)-1) - rem;

    if (rem == 0)
    {
        /* Unlock and give up rest of quantum */
        SDL_UnlockMutex(m);
        SDL_Delay(0);
        SDL_LockMutex(m);
    }
    else
    {
        /* These sleeps must be signalable for thread exit */
        SDL_CondWaitTimeout(current->context.c, m, rem);
    }

    running = current;

    current->statearg = STATE_RUNNING;

    if (threads_exit)
        remove_thread(NULL);
}

int runthread(void *data)
{
    struct thread_entry *current;
    jmp_buf *current_jmpbuf;

    /* Cannot access thread variables before locking the mutex as the
       data structures may not be filled-in yet. */
    SDL_LockMutex(m);
    running = (struct thread_entry *)data;
    current = running;
    current_jmpbuf = &thread_jmpbufs[running - threads];

    /* Setup jump for exit */
    if (setjmp(*current_jmpbuf) == 0)
    {
        /* Run the thread routine */
        current->context.start();
        THREAD_SDL_DEBUGF("Thread Done: %d (%s)\n",
                          current - threads, THREAD_SDL_GET_NAME(current));
        /* Thread routine returned - suicide */
        remove_thread(NULL);
    }
    else
    {
        /* Unlock and exit */
        SDL_UnlockMutex(m);
    }

    return 0;
}

struct thread_entry* 
    create_thread(void (*function)(void), void* stack, int stack_size,
                  const char *name)
{
    /** Avoid compiler warnings */
    SDL_Thread* t;
    SDL_cond *cond;
    int slot;

    THREAD_SDL_DEBUGF("Creating thread: (%s)\n", name ? name : "");

    slot = find_empty_thread_slot();
    if (slot >= MAXTHREADS)
    {
        DEBUGF("Failed to find thread slot\n");
        return NULL;
    }

    cond = SDL_CreateCond();
    if (cond == NULL)
    {
        DEBUGF("Failed to create condition variable\n");
        return NULL;
    }

    t = SDL_CreateThread(runthread, &threads[slot]);
    if (t == NULL)
    {
        DEBUGF("Failed to create SDL thread\n");
        SDL_DestroyCond(cond);
        return NULL;
    }

    threads[slot].stack = stack;
    threads[slot].stack_size = stack_size;
    threads[slot].name = name;
    threads[slot].statearg = STATE_RUNNING;
    threads[slot].context.start = function;
    threads[slot].context.t = t;
    threads[slot].context.c = cond;

    THREAD_SDL_DEBUGF("New Thread: %d (%s)\n",
                      slot, THREAD_SDL_GET_NAME(&threads[slot]));

    return &threads[slot];
}

void block_thread(struct thread_entry **list)
{
    struct thread_entry *thread = running;

    thread->statearg = STATE_BLOCKED;
    add_to_list(list, thread);

    SDL_CondWait(thread->context.c, m);
    running = thread;

    if (threads_exit)
        remove_thread(NULL);
}

void block_thread_w_tmo(struct thread_entry **list, int ticks)
{
    struct thread_entry *thread = running;

    thread->statearg = STATE_BLOCKED_W_TMO;
    add_to_list(list, thread);

    SDL_CondWaitTimeout(thread->context.c, m, (1000/HZ) * ticks);
    running = thread;

    if (thread->statearg == STATE_BLOCKED_W_TMO)
    {
        /* Timed out */
        remove_from_list(list, thread);
        thread->statearg = STATE_RUNNING;
    }

    if (threads_exit)
        remove_thread(NULL);
}

void wakeup_thread(struct thread_entry **list)
{
    struct thread_entry *thread = *list;

    if (thread == NULL)
    {
        return;
    }

    switch (thread->statearg)
    {
    case STATE_BLOCKED:
    case STATE_BLOCKED_W_TMO:
        remove_from_list(list, thread);
        thread->statearg = STATE_RUNNING;
        SDL_CondSignal(thread->context.c);
        break;
    }
}

void init_threads(void)
{
    /* Main thread is already initialized */
    if (running != &threads[0])
    {
        THREAD_PANICF("Wrong main thread in init_threads: %p\n", running);
    }

    THREAD_SDL_DEBUGF("First Thread: %d (%s)\n",
            0, THREAD_SDL_GET_NAME(&threads[0]));
}

void remove_thread(struct thread_entry *thread)
{
    struct thread_entry *current = running;
    SDL_Thread *t;
    SDL_cond *c;

    if (thread == NULL)
    {
        thread = current;
    }

    t = thread->context.t;
    c = thread->context.c;
    thread->context.t = NULL;

    if (thread != current)
        SDL_CondSignal(c);

    THREAD_SDL_DEBUGF("Removing thread: %d (%s)\n",
        thread - threads, THREAD_SDL_GET_NAME(thread));

    thread->name = NULL;

    SDL_DestroyCond(c);

    if (thread == current)
    {
        /* Do a graceful exit - perform the longjmp back into the thread
           function to return */
        longjmp(thread_jmpbufs[current - threads], 1);
    }

    SDL_KillThread(t);
}

int thread_stack_usage(const struct thread_entry *thread)
{
    return 50;
    (void)thread;
}

int thread_get_status(const struct thread_entry *thread)
{
    return thread->statearg;
}

/* Return name if one or ID if none */
void thread_get_name(char *buffer, int size,
                     struct thread_entry *thread)
{
    if (size <= 0)
        return;

    *buffer = '\0';

    if (thread)
    {
        /* Display thread name if one or ID if none */
        bool named = thread->name && *thread->name;
        const char *fmt = named ? "%s" : "%08lX";
        intptr_t name = named ?
            (intptr_t)thread->name : (intptr_t)thread;
        snprintf(buffer, size, fmt, name);
    }
}
