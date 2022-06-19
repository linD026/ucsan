#ifndef __UCSAN_LIST_H__
#define __UCSAN_LIST_H__

#include <stddef.h>

struct list_head {
	struct list_head *next;
	struct list_head *prev;
};

static inline void list_init(struct list_head *node)
{
	node->next = node;
	node->prev = node;
}

static inline void __list_add(struct list_head *new, struct list_head *prev,
			      struct list_head *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

static inline void list_add(struct list_head *new, struct list_head *head)
{
	__list_add(new, head, head->next);
}

static inline void list_add_tail(struct list_head *new, struct list_head *head)
{
	__list_add(new, head->prev, head);
}

static inline void __list_del(struct list_head *prev, struct list_head *next)
{
	next->prev = prev;
	prev->next = next;
}

static inline void list_del(struct list_head *node)
{
	__list_del(node->prev, node->next);
	list_init(node);
}

#define list_for_each(n, head) for (n = (head)->next; n != (head); n = n->next)

#define list_for_each_from(pos, head) for (; pos != (head); pos = pos->next)

#define list_for_each_safe(pos, n, head)                       \
	for (pos = (head)->next, n = pos->next; pos != (head); \
	     pos = n, n = pos->next)

#endif /* __UCSAN_LIST_H__ */
