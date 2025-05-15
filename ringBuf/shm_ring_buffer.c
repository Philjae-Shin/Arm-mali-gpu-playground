/*
 * shm_ring_buffer.c – Single‑file demo: lock‑free SPSC ring buffer in
 * 64 KiB POSIX shared memory, signalled by one eventfd, with futex
 * back‑off after 100 spins.
 *
 * Build:  gcc -O2 -pthread -std=gnu11 -Wall -o ring shm_ring_buffer.c
 * Run  :  ./ring           (forks producer & consumer for a quick test)
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/eventfd.h>
#include <sys/syscall.h>
#include <linux/futex.h>
#include <time.h>
#include <errno.h>

#define SHM_NAME "/ringbuf64k"
#define BUF_SZ (64 * 1024 / 128)    /* 512 × 128‑byte slots = 64 KiB */

struct ring {
    _Atomic uint32_t head;          /* producer writes, consumer reads */
    _Atomic uint32_t tail;          /* consumer writes, producer reads */
    char data[BUF_SZ][128];
};

/* ----------------------------------------------------------------------------
 *  Thin wrappers – cpu_relax() / futex helpers
 * --------------------------------------------------------------------------*/
static inline void cpu_relax(void)
{
#if defined(__x86_64__) || defined(__i386__)
    __asm__ __volatile__("pause");
#elif defined(__aarch64__)
    __asm__ __volatile__("yield");
#endif
}

static inline void futex_wait_u32(volatile uint32_t *addr, uint32_t val)
{
    syscall(SYS_futex, addr, FUTEX_WAIT_PRIVATE, val, NULL, NULL, 0);
}
static inline void futex_wake_u32(volatile uint32_t *addr, int n)
{
    syscall(SYS_futex, addr, FUTEX_WAKE_PRIVATE, n, NULL, NULL, 0);
}

/* ----------------------------------------------------------------------------
 *  Enqueue (producer) / Dequeue (consumer)
 * --------------------------------------------------------------------------*/
static bool enqueue(struct ring *r, const char msg[128], int efd)
{
    int spins = 0;
    for (;;) {
        uint32_t h = atomic_load_explicit(&r->head, memory_order_relaxed);
        uint32_t t = atomic_load_explicit(&r->tail, memory_order_acquire);
        if (((h + 1) % BUF_SZ) != t) {                 /* space available */
            memcpy(r->data[h], msg, 128);
            atomic_store_explicit(&r->head, (h + 1) % BUF_SZ,
                                  memory_order_release);
            eventfd_write(efd, 1);                     /* notify consumer */
            return true;
        }
        /* Buffer full – busy‑spin 100 ×, then futex sleep on tail. */
        if (++spins < 100)
            cpu_relax();
        else {
            futex_wait_u32(&r->tail, t);
            spins = 0;
        }
    }
}

static bool dequeue(struct ring *r, char out[128])
{
    uint32_t t = atomic_load_explicit(&r->tail, memory_order_relaxed);
    uint32_t h = atomic_load_explicit(&r->head, memory_order_acquire);
    if (t == h)
        return false;                                   /* empty */

    memcpy(out, r->data[t], 128);
    atomic_store_explicit(&r->tail, (t + 1) % BUF_SZ,
                          memory_order_release);
    futex_wake_u32(&r->tail, 1);                        /* wake producer */
    return true;
}

/* ----------------------------------------------------------------------------
 *  Helper – monotonic clock in ns
 * --------------------------------------------------------------------------*/
static inline uint64_t nsec_now(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ull + ts.tv_nsec;
}

/* ----------------------------------------------------------------------------
 *  Quick self‑test – forks producer & consumer, sends 5 000 000 msgs
 * --------------------------------------------------------------------------*/
int main(void)
{
    /* 1. Create / open shared ring */
    int shmfd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0600);
    if (shmfd < 0) { perror("shm_open"); return 1; }
    if (ftruncate(shmfd, sizeof(struct ring)) < 0) { perror("ftruncate"); return 1; }

    struct ring *r = mmap(NULL, sizeof(struct ring),
                          PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
    if (r == MAP_FAILED) { perror("mmap"); return 1; }

    /* First process zero‑initialises once (racy‑safe enough at start) */
    if (atomic_load(&r->head) >= BUF_SZ) atomic_store(&r->head, 0);
    if (atomic_load(&r->tail) >= BUF_SZ) atomic_store(&r->tail, 0);

    /* 2. Single eventfd shared via fork */
    int efd = eventfd(0, 0);
    if (efd < 0) { perror("eventfd"); return 1; }

    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return 1; }

    /* --------------------------------------------------------------------- */
    if (pid == 0) {            /* === Consumer === */
        char buf[128];
        uint64_t msgs = 0, sig;
        uint64_t t0 = nsec_now();

        while (msgs < 5 * 1000 * 1000ULL) {
            if (dequeue(r, buf)) {
                msgs++;
            } else {
                /* Block until producer writes event */
                if (eventfd_read(efd, &sig) == -1 && errno != EAGAIN)
                    perror("eventfd_read");
            }
        }
        double dt = (nsec_now() - t0) / 1e9;
        printf("Consumer   %llu msgs in %.3f s  =>  %.1f msgs/s\n",
               (unsigned long long)msgs, dt, msgs / dt);
    } else {                  /* === Producer === */
        char msg[128] = {0};
        uint64_t msgs = 0;
        uint64_t t0 = nsec_now();

        while (msgs < 5 * 1000 * 1000ULL) {
            memcpy(msg, &msgs, sizeof(msgs));   /* dummy payload */
            enqueue(r, msg, efd);
            msgs++;
        }
        double dt = (nsec_now() - t0) / 1e9;
        printf("Producer   %llu msgs in %.3f s  =>  %.1f msgs/s\n",
               (unsigned long long)msgs, dt, msgs / dt);
    }

    return 0;
}
