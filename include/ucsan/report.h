#ifndef __UCSAN_REPORT_H__
#define __UCSAN_REPORT_H__

#include <ucsan/compiler.h>
#include <stdio.h>

#define UCSAN_REPORT_FILE_NAME "ucsan_report.log"

/*
 * To exclude the unit test detection, we use stdout to print the data race
 * report.
 */
#define report_print(fmt, ...)                                          \
	do {                                                            \
		if (likely(UCSAN_REPORT_FILE))                          \
			fprintf(UCSAN_REPORT_FILE, fmt, ##__VA_ARGS__); \
		fprintf(stdout, fmt, ##__VA_ARGS__);                    \
	} while (0)

#define report_init()                                            \
	UCSAN_REPORT_FILE = fopen(UCSAN_REPORT_FILE_NAME, "w+"); \
	report_print(                                            \
		"==================================================================\n");

#define report_exit()                                                                    \
	report_print(                                                                    \
		"==================================================================\n"); \
	fclose(UCSAN_REPORT_FILE);

#endif /* __UCSAN_REPORT_H__ */
