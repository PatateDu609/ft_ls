#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <stdbool.h>
#include "libft.h"

/*
 * @brief: A macro to generate a priority queue header with a given name and type.
 * @param: name: The name of the priority queue.
 * @param: type: The type of the priority queue.
 */
#define make_priority_queue(type, name)							\
	typedef int compare_##name##_fun(const type, const type);	\
	typedef void free_##name##_fun(type);						\
	typedef struct												\
	{															\
		type *data;												\
		int nb;													\
		compare_##name##_fun *cmp;								\
		free_##name##_fun *free_data;							\
	} pq_##name##_t;											\
	pq_##name##_t *pq_##name##_new(compare_##name##_fun *cmp);	\
	void pq_##name##_free(pq_##name##_t *pq);					\
	void pq_##name##_push(pq_##name##_t *pq, type data);		\
	bool pq_##name##_pop(pq_##name##_t *pq, type *ret);

make_priority_queue(char *, str)

#endif
