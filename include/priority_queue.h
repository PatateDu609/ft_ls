#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#include "libft.h"

#define CAT(a, b) CAT2(a, b)
#define CAT2(a, b) a##b

#endif

// NOTE: This portion of the code is not guarded as it is intended to be included multiple times.

#ifndef PQ_T
# error "You must define PQ_T before including this file."
#endif

#ifndef PQ_NAME
#define PQ_NAME CAT(CAT(pq, _), PQ_T)
#endif

#ifndef PQ_PREFIX
#define PQ_PREFIX CAT(pq_, CAT(PQ_NAME, _))
#endif

#undef PQ_STRUCT_TYPE
#define PQ_STRUCT_TYPE CAT(PQ_PREFIX, t)

#ifndef PQ_LINKAGE
#define PQ_LINKAGE static inline
#endif

#define make_function_name(name) CAT(PQ_PREFIX, name)
#define make_typedef_name(name) CAT(PQ_PREFIX, name##_fun)
#define make_typedef_fun(ret, name, ...) typedef ret (make_typedef_name(name))(__VA_ARGS__)

make_typedef_fun(void, free, PQ_T);
make_typedef_fun(int, cmp, const PQ_T, const PQ_T);

typedef struct PQ_STRUCT_TYPE
{
	PQ_T *data;
	size_t size;
	size_t capacity;

	make_typedef_name(cmp) *cmp;
	make_typedef_name(free) *free_data;
} PQ_STRUCT_TYPE;

/*
 * @brief: Swap two elements in the priority queue.
 * @param: a: The first element.
 * @param: b: The second element.
*/
PQ_LINKAGE void make_function_name(swap)(PQ_T *a, PQ_T *b)
{
	PQ_T tmp = *a;
	*a = *b;
	*b = tmp;
}

/*
 * @brief: Heapify the priority queue (i.e. make it a heap).
 * @param: pq: The priority queue.
 * @param: i: The index of the element to heapify.
*/
PQ_LINKAGE void make_function_name(heapify)(PQ_STRUCT_TYPE *pq, size_t i)
{
	if (pq->size <= 1)
		return;
	size_t largest = i;
	size_t left = 2 * i + 1;
	size_t right = 2 * i + 2;

	if (left < pq->size && pq->cmp(pq->data[left], pq->data[largest]) < 0)
		largest = left;
	if (right < pq->size && pq->cmp(pq->data[right], pq->data[largest]) < 0)
		largest = right;

	if (largest != i)
	{
		make_function_name(swap)(&pq->data[i], &pq->data[largest]);
		make_function_name(heapify)(pq, largest);
	}
}

/*
 * @brief: Creates a new priority queue.
 * @param: cmp: The comparison function used in the heapify function.
*/
PQ_LINKAGE PQ_STRUCT_TYPE *make_function_name(new)(make_typedef_name(cmp) *cmp)
{
	PQ_STRUCT_TYPE *pq = malloc(sizeof(PQ_STRUCT_TYPE));
	if (!pq)
		return NULL;

	pq->cmp = cmp;
	pq->free_data = NULL;
	pq->data = NULL;
	pq->size = 0;

	return pq;
}

/*
 * @brief: Frees the memory allocated for the priority queue.
 * @param: pq: The priority queue.
*/
PQ_LINKAGE void make_function_name(free)(PQ_STRUCT_TYPE *pq)
{
	if (!pq)
		return;

	if (pq->free_data)
	{
		for (size_t i = 0; i < pq->size; i++)
			pq->free_data(pq->data[i]);
	}
	free(pq->data);
	free(pq);
}

/*
 * @brief: Pushes an element to the priority queue.
 * @param: pq: The priority queue.
 * @param: data: The element to push.
 * @return: True if the push was successful, false otherwise.
*/
PQ_LINKAGE bool make_function_name(push)(PQ_STRUCT_TYPE *pq, PQ_T data)
{
	if (!pq->size)
	{
		pq->capacity = 128;
		pq->data = ft_calloc(pq->capacity, sizeof(PQ_T));

		if (!pq->data)
			return false;
		pq->data[0] = data;
		pq->size = 1;
		return true;
	}
	if (pq->size == pq->capacity)
	{
		pq->capacity *= 2;
		PQ_T *tmp = realloc(pq->data, sizeof(PQ_T) * pq->capacity);
		if (!tmp)
			return false;
		pq->data = tmp;
	}
	pq->data[pq->size] = data;
	pq->size++;
	for (int i = pq->size / 2 - 1; i >= 0; i--)
		make_function_name(heapify)(pq, i);
	return true;
}

/*
 * @brief: Pops the top element of the priority queue.
 * @param: pq: The priority queue.
 * @param: ret: The element popped.
 * @return: True if the element was popped, false otherwise.
*/
PQ_LINKAGE bool make_function_name(pop)(PQ_STRUCT_TYPE *pq, PQ_T *ret)
{
	if (!pq->size)
		return false;

	*ret = pq->data[0];
	pq->data[0] = pq->data[pq->size - 1];
	pq->size--;
	for (int i = pq->size / 2 - 1; i >= 0; i--)
		make_function_name(heapify)(pq, i);
	return true;
}

// Cleanup defines to avoid name clashes and to make this header usable multiple times.

#undef PQ_PREFIX
#undef PQ_T
#undef PQ_NAME
#undef PQ_STRUCT_TYPE
#undef PQ_LINKAGE
#undef make_function_name
#undef make_typedef_fun
#undef make_typedef_name
