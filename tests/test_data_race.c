#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <threads.h>

#include "printut.h"

#undef __FILE_NAME__
#define __FILE_NAME__ "tests/test_data_race.c"

#define NR_ROUND 8
#define NR_WORKER 4

#ifndef UCSAN_REPORT_FILE_NAME
#define UCSAN_REPORT_FILE_NAME "ucsan_report.log"
#endif
static FILE *UCSAN_REPORT_FILE = NULL;

struct report_check_table {
	const char *substring;
	bool found;
};

const static int nr_check_table = 3;

static int foo;

static int write_func(void *unused)
{
	int i;
	for (i = 0; i < NR_ROUND / 2; i++)
		foo = i;

	return 0;
}

static int read_func(void *unused)
{
	int i, a;
	for (i = 0; i < NR_ROUND / 2; i++)
		a = foo;

	return 0;
}

static int check_data_race(void)
{
	char *line = NULL;
	size_t line_size = 0;
	ssize_t nr_read;
	int line_count = 0, check_count = 0, i;
	struct report_check_table report_check_table[] = {
		[0] = { .substring = "BUG: UCSAN: data-race", .found = false },
		[1] = { .substring = "write to", .found = false },
		[2] = { .substring = "value changed:", .found = false },
	};

	UCSAN_REPORT_FILE = fopen(UCSAN_REPORT_FILE_NAME, "r");
	if (!UCSAN_REPORT_FILE) {
		pr_info("%s not exist\n", UCSAN_REPORT_FILE_NAME);
		return -1;
	}

	while ((nr_read = getline(&line, &line_size, UCSAN_REPORT_FILE)) !=
	       -1) {
		line_count++;
		if (nr_read <= 1)
			continue;
		line[nr_read - 1] = '\0';
		for (i = check_count; i < nr_check_table; i++) {
			if (report_check_table[i].found)
				continue;
			if (strstr(line, report_check_table[i].substring) !=
			    NULL) {
				report_check_table[i].found = true;
				check_count++;
				break;
			}
		}
		free(line);
		line = NULL;
	}

	if (check_count != nr_check_table)
		return -1;
	return 0;
}

static int test_data_race(void)
{
	thrd_t p[NR_WORKER];
	int i, j;
	bool data_race = false;

	pr_info("round=%d, worker=%d\n", NR_ROUND, NR_WORKER);
	pr_info("start testing\n");

	for (i = 0; i < NR_ROUND; i++) {
		for (j = 0; j < NR_WORKER; j++)
			thrd_create(&p[j], (j & 0x1) ? read_func : write_func,
				    NULL);
		for (j = 0; j < NR_WORKER; j++)
			thrd_join(p[j], NULL);
		if (!check_data_race()) {
			data_race = true;
			pr_info("Catch the data race\n");
			break;
		}
	}

	pr_info("End testing\n");

	if (!data_race) {
		pr_err("TOTAL ROUND:%d: NOt FOUND DATA RACE\n", NR_ROUND);
		return -1;
	}
	return 0;
}

int main(void)
{
	UNIT_BUG_ON(test_data_race());

	return 0;
}
