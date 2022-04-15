#ifndef __UCSAN__
#define __UCSAN__

extern void __tsan_func_entry(void *call_pc);
extern void __tsan_func_exit(void *p);
extern void __tsan_init(void);

extern void tsan_read4(void *ptr);
extern void __tsan_read4(void *ptr);
extern void __tsan_write4(void *ptr);

#endif /* __UCSAN__ */
