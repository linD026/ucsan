#ifndef __PRINT_UNIT_TEST_H__
#define __PRINT_UNIT_TEST_H__

#include <stdio.h>
#include <assert.h>
#include <sys/time.h>

static inline unsigned long __get_ms(void)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);

	return (unsigned long)1000000 * tv.tv_sec + tv.tv_usec;
}

#define __stringlfy(s) #s

/*
 * For only get the file name gcc use __FILE_NAME__, clang use __FILE_NAME__.
 * Otherwire use -D __FILE_NAME__=\"$*.c\" in the GNU make.
 * But it will get the original file not the test suit file.
 * So here we use self-defined macro __FILE_NAME__.
 * To prevent the compile error, we set the default value of __FILE_NAME__.
 * When we want to use the __FILE_NAME__ we can undefine it and define our
 * own name.
 */

#define __FILE_NAME__ __FILE__

#define pr_err(fmt, ...)                                               \
	do {                                                           \
		fprintf(stderr, " [%-10lu] ERROR: %s:%d:%s(): " fmt,   \
			__get_ms(), __FILE_NAME__, __LINE__, __func__, \
			##__VA_ARGS__);                                \
	} while (0)

#define pr_info(fmt, ...)                                                  \
	do {                                                               \
		unsigned long __ms = __get_ms();                           \
		printf(" [%-10lu] INFO: %s:%d:%s(): " fmt, __ms,           \
		       __FILE_NAME__, __LINE__, __func__, ##__VA_ARGS__);  \
		fprintf(stderr, " [%-10lu] INFO: %s+%d:%s(): " fmt, __ms,  \
			__FILE_NAME__, __LINE__, __func__, ##__VA_ARGS__); \
	} while (0)

#define BUG_ON(cond)                                                          \
	do {                                                                  \
		if (cond) {                                                   \
			printf(" [%-10lu] BUG: %s:%d:%s(): %s\n", __get_ms(), \
			       __FILE_NAME__, __LINE__, __func__,             \
			       __stringlfy(cond));                            \
			assert(cond);                                         \
		}                                                             \
	} while (0)

#define UNIT_BUG_ON(cond)                                       \
	do {                                                    \
		BUG_ON(cond);                                   \
		printf(" [TEST SUCCESS] %s\n", #cond);          \
		fprintf(stderr, " [TEST SUCCESS] %s\n", #cond); \
	} while (0)

#endif /* __PRINT_UNIT_TEST_H__ */
