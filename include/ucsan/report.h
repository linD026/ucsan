#ifndef __UCSAN_REPORT_H__
#define __UCSAN_REPORT_H__

#include <stdio.h>

#define report_print(fmt, ...)                       \
	do {                                         \
		fprintf(stderr, fmt, ##__VA_ARGS__); \
	} while (0)

#define report_init() \
	report_print( \
		"==================================================================\n");

#define report_exit() \
	report_print( \
		"==================================================================\n");

#endif /* __UCSAN_REPORT_H__ */
