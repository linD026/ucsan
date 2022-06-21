#ifdef __UT_UNIFY
#include <stdbool.h>
#include <string.h>
#include <ucsan/compiler.h>
#include <ucsan/ucsan.h>
#include <ucsan/report.h>

#include "printut.h"

#undef __FILE_NAME__
#define __FILE_NAME__ "tests/test_unify.c"

struct task_info;

int test_producer_consumer(void)
{
	int ret = 0, foo = 0;
	void *ptr = &foo;
	size_t size = sizeof(__typeof__(foo));
	int access_type = UCSAN_ACCESS_WRITE;
	unsigned long ip = _RET_IP_;
	struct task_info *task = NULL;

	pr_info("ptr=%p size=%zu access_type=%x ip=%lx\n", ptr, size,
		access_type, ip);

	if (unify_task_container_producer(ptr, size, access_type, ip)) {
		pr_err("task container producer\n");
		return -1;
	}

	task = unify_task_conatainer_consumer(ptr, size);
	if (!task) {
		pr_err("task container consumer\n");
		return -1;
	}

	if (task->ptr != ptr) {
		pr_err("ptr not equal\n");
		ret = -1;
	}
	if (task->size != size) {
		pr_err("size not equal\n");
		ret = -1;
	}
	if (task->access_type != access_type) {
		pr_err("access_type not equal\n");
		ret = -1;
	}
	if (task->ip != ip) {
		pr_err("ip not equal\n");
		ret = -1;
	}

	return ret;
}

struct report_check_table {
	const char *substring;
	bool found;
};

static struct report_check_table report_check_table[] = {
	[0] = { .substring = "BUG: UCSAN: data-race", .found = false },
	[1] = { .substring = "write to", .found = false },
	[2] = { .substring = "value changed:", .found = false },
};
static int nr_check_table = 3;

static int test_report(void)
{
	char *line = NULL;
	size_t line_size = 0;
	ssize_t nr_read;
	int line_count = 0, check_count = 0, i, ret = 0, foo = 0;
	void *ptr = &foo;
	size_t size = sizeof(__typeof__(foo));
	int access_type = UCSAN_ACCESS_WRITE;
	unsigned long ip = _RET_IP_;

	pr_info("ptr=%p size=%zu access_type=%x ip=%lx\n", ptr, size,
		access_type, ip);

	unify_set_info(ptr, size, access_type, ip, 0);
	unify_report(ptr, size, access_type, ip, 0, 1);

	UCSAN_REPORT_FILE = fopen(UCSAN_REPORT_FILE_NAME, "r");
	if (!UCSAN_REPORT_FILE) {
		pr_err("Doesn't create the %s\n", UCSAN_REPORT_FILE_NAME);
		ret = -1;
	}

	while ((nr_read = getline(&line, &line_size, UCSAN_REPORT_FILE)) !=
	       -1) {
		line_count++;
		if (nr_read <= 1)
			continue;
		line[nr_read - 1] = '\0';
		pr_info("read line:%d: \"%s\"\n", line_count, line);
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

	if (check_count != nr_check_table) {
		for (i = 0; i < nr_check_table; i++) {
			if (!report_check_table[i].found)
				pr_err("\"%s\" not found\n",
				       report_check_table[i].substring);
		}
		ret = -1;
	}

	return ret;
}

int main(void)
{
	UNIT_BUG_ON(test_producer_consumer());
	UNIT_BUG_ON(test_report());

	return 0;
}
#endif /* __UT_UNIFY */
