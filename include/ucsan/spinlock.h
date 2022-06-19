#ifndef __UCSAN_SPINLOCK_H__
#define __UCSAN_SPINLOCK_H__

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef pthread_mutex_t spinlock_t;

#define DEFINE_SPINLOCK(lock) spinlock_t lock = PTHREAD_MUTEX_INITIALIZER
#define SPINLOCK_INIT PTHREAD_MUTEX_INITIALIZER

static __inline__ void spin_lock_init(spinlock_t *sp)
{
	int ret;

	ret = pthread_mutex_init(sp, NULL);
	if (ret != 0) {
		fprintf(stderr, "spin_lock_init:pthread_mutex_init %d\n", ret);
		abort();
	}
}

static __inline__ void spin_lock(spinlock_t *sp)
{
	int ret;

	ret = pthread_mutex_lock(sp);
	if (ret != 0) {
		fprintf(stderr, "spin_lock:pthread_mutex_lock %d\n", ret);
		abort();
	}
}

static __inline__ void spin_unlock(spinlock_t *sp)
{
	int ret;

	ret = pthread_mutex_unlock(sp);
	if (ret != 0) {
		fprintf(stderr, "spin_unlock:pthread_mutex_unlock %d\n", ret);
		abort();
	}
}

#endif /* __UCSAN_SPINLOCK_H__ */
