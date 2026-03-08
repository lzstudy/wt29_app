#ifndef __LIST__H__
#define __LIST__H__

#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))


struct list_head {
	struct list_head *next, *prev;
};

/* 链表初始化 */
#define LIST_HEAD_INIT(name) { .next = &name, .prev = &name }

/* 静态初始化链表 */
#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)

/* 链表入口 */
#define list_entry(ptr, type, member) \
	container_of(ptr, type, member)

/* 链表遍历 */
#define list_for_each(list, node)		\
	for ((node) = (list)->next;		\
	     (node) != (list);			\
	     (node) = (node)->next)

/* 遍历链表 */
#define list_for_each_entry(pos, head, type, member) \
    for (pos = list_entry((head)->next, type, member); \
         &pos->member != (head); \
         pos = list_entry(pos->member.next, type, member))

/* 安全遍历 */
#define list_for_each_entry_safe(pos, n, head, type, member) \
		for (pos = list_entry((head)->next, type, member), \
			n = list_entry(pos->member.next, type, member); \
			&pos->member != (head); \
			pos = n, n = list_entry(n->member.next, type, member))

#if 0
/* 链表遍历 */
#define list_for_each_entry(pos, head, member)				\
	for (pos = list_entry((head)->next, typeof(*pos), member);	\
	     &pos->member != (head); 	\
	     pos = list_entry(pos->member.next, typeof(*pos), member))

/* 安全遍历 */
#define list_for_each_entry_safe(pos, n, head, member)			\
	for (pos = list_entry((head)->next, typeof(*pos), member),	\
		n = list_entry(pos->member.next, typeof(*pos), member);	\
	     &pos->member != (head);					\
	     pos = n, n = list_entry(n->member.next, typeof(*n), member))
#endif

static inline void list_init(struct list_head *list)
{
	list->prev = list;
	list->next = list;
}

static inline void list_add_before(struct list_head *node,
					 struct list_head *new_node)
{
	new_node->prev = node->prev;
	new_node->next = node;
	new_node->next->prev = new_node;
	new_node->prev->next = new_node;
}

static inline void list_add_after(struct list_head *node,
					struct list_head *new_node)
{
	new_node->prev = node;
	new_node->next = node->next;
	new_node->next->prev = new_node;
	new_node->prev->next = new_node;
}

static inline void list_add_head(struct list_head *list,
				       struct list_head *node)
{
	list_add_after(list, node);
}

static inline void list_add_tail(struct list_head *list,
				       struct list_head *node)
{
	list_add_before(list, node);
}

static inline int list_is_empty(struct list_head *list)
{
	return list->next == list;
}

static inline void list_del(struct list_head *node)
{
	node->next->prev = node->prev;
	node->prev->next = node->next;
	node->prev = node;
	node->next = node;
}

static inline struct list_head *list_first(struct list_head *list)
{
	return list_is_empty(list) ? NULL : list->next;
}

static inline bool list_find_node(struct list_head *list,
					struct list_head *node)
{
	struct list_head *n;

	list_for_each(list, n) {
		if (n == node)
			return true;
	}
	return false;
}
/** @} */

#ifdef __cplusplus
}
#endif

#endif
