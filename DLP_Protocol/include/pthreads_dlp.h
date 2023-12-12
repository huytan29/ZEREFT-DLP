/* Copyright (C) 2023, LancsNet Jsc.
 * All rights reserved.
 */

#ifndef PTHREADS_OP_H
#define PTHREADS_OP_H

#ifndef WIN32
#define w_create_thread(x, y) if (!CreateThread((void * (*) (void *))x, y)) lerror_exit(THREAD_ERROR);
#else
#define w_create_thread(x, y, z, a, b, c) ({HANDLE hd; if (!(hd = CreateThread(x,y,z,a,b,c))) lerror_exit(THREAD_ERROR); hd;})
#endif
#define w_mutex_init(x, y) { int error = pthread_mutex_init(x, y); if (error) lerror_exit("At pthread_mutex_init(): %s", strerror(error)); }
#define w_mutex_lock(x) { int error = pthread_mutex_lock(x); if (error) lerror_exit("At pthread_mutex_lock(): %s", strerror(error)); }
#define w_mutex_unlock(x) { int error = pthread_mutex_unlock(x); if (error) lerror_exit("At pthread_mutex_unlock(): %s", strerror(error)); }
#define w_mutex_destroy(x) { int error = pthread_mutex_destroy(x); if (error) lerror_exit("At pthread_mutex_destroy(): %s", strerror(error)); }
#define w_cond_init(x, y) { int error = pthread_cond_init(x, y); if (error) lerror_exit("At pthread_cond_init(): %s", strerror(error)); }
#define w_cond_wait(x, y) { int error = pthread_cond_wait(x, y); if (error) lerror_exit("At pthread_cond_wait(): %s", strerror(error)); }
#define w_cond_signal(x) { int error = pthread_cond_signal(x); if (error) lerror_exit("At pthread_cond_signal(): %s", strerror(error)); }
#define w_cond_broadcast(x) { int error = pthread_cond_broadcast(x); if (error) lerror_exit("At pthread_cond_broadcast(): %s", strerror(error)); }
#define w_cond_destroy(x) { int error = pthread_cond_destroy(x); if (error) lerror_exit("At pthread_cond_destroy(): %s", strerror(error)); }
#define w_rwlock_init(x, y) { int error = pthread_rwlock_init(x, y); if (error) lerror_exit("At pthread_rwlock_init(): %s", strerror(error)); }
#define w_rwlock_rdlock(x) { int error = pthread_rwlock_rdlock(x); if (error) lerror_exit("At pthread_rwlock_rdlock(): %s", strerror(error)); }
#define w_rwlock_wrlock(x) { int error = pthread_rwlock_wrlock(x); if (error) lerror_exit("At pthread_rwlock_wrlock(): %s", strerror(error)); }
#define w_rwlock_unlock(x) { int error = pthread_rwlock_unlock(x); if (error) lerror_exit("At pthread_rwlock_unlock(): %s", strerror(error)); }
#define w_rwlock_destroy(x) { int error = pthread_rwlock_destroy(x); if (error) lerror_exit("At pthread_rwlock_destroy(" #x "): %s", strerror(error)); }
#define w_mutexattr_init(x) { int error = pthread_mutexattr_init(x); if (error) lerror_exit("At pthread_mutexattr_init(): %s", strerror(error)); }
#define w_mutexattr_settype(x, y) { int error = pthread_mutexattr_settype(x, y); if (error) lerror_exit("At pthread_mutexattr_settype(): %s", strerror(error)); }
#define w_mutexattr_destroy(x) { int error = pthread_mutexattr_destroy(x); if (error) lerror_exit("At pthread_mutexattr_destroy(): %s", strerror(error)); }

#ifndef WIN32
int CreateThread(void * (*function_pointer)(void *), void * data) __attribute__((nonnull(1)));
int CreateThreadJoinable(pthread_t *lthread, void * (*function_pointer)(void *), void *data);
#endif

#endif
