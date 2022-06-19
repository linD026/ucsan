#ifndef __UCSAN_UNIT_TEST_H__
#define __UCSAN_UNIT_TEST_H__

#ifdef CONFIG_UNIT_TEST

/* PWD = root/include */

#if defined(__UT_DETECT)
#include "../tests/test_watchpoint.c"
#elif defined(__UT_UNIFY)
#include "../tests/test_unify.c"
#endif

#endif /* CONFIG_UNIT_TEST */

#endif /* __UCSAN_UNIT_TEST_H__ */
