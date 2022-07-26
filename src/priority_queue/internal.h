#ifndef PQ_INTERNAL_H
#define PQ_INTERNAL_H

#include "priority_queue.h"
#include <stdlib.h>

#define make_heapify_func(type, name)											\
	static inline void __swap(type *a, type *b)									\
	{																			\
		type tmp = *a;															\
		*a = *b;																\
		*b = tmp;																\
	}																			\
	static inline void __pq_heapify_##name(pq_##name##_t *pq, int i)			\
	{																			\
		if (pq->nb < 2)															\
			return;																\
		int largest = i;														\
		int left = 2 * i + 1;													\
		int right = 2 * i + 2;													\
		if (left < pq->nb && pq->cmp(pq->data[largest], pq->data[left]) > 0)	\
			largest = left;														\
		if (right < pq->nb && pq->cmp(pq->data[largest], pq->data[right]) > 0)	\
			largest = right;													\
		if (largest != i)														\
		{																		\
			__swap(&pq->data[i], &pq->data[largest]);							\
			__pq_heapify_##name(pq, largest);									\
		}																		\
	}

#define make_new_function(type, name)							\
	pq_##name##_t *pq_##name##_new(compare_##name##_fun *cmp)	\
	{															\
		pq_##name##_t *pq = malloc(sizeof(pq_##name##_t));		\
		if (!pq)												\
			return NULL;										\
		pq->data = NULL;										\
		pq->free_data = NULL;									\
		pq->nb = 0;												\
		pq->cmp = cmp;											\
		return pq;												\
	}

#define make_free_function(type, name)			\
	void pq_##name##_free(pq_##name##_t *pq)	\
	{											\
		if (!pq)								\
			return;								\
		if (pq->free_data)						\
			for (int i = 0; i < pq->nb; i++)	\
				pq->free_data(pq->data[i]);		\
		free(pq->data);							\
		free(pq);								\
	}

#define make_push_function(type, name)							\
	void pq_##name##_push(pq_##name##_t *pq, type el)			\
	{															\
		if (!pq->nb)											\
		{														\
			pq->data = ft_calloc(1, sizeof(type));				\
			if (!pq->data)										\
				return;											\
			pq->data[0] = el;									\
			pq->nb++;											\
			return;												\
		}														\
		type *new_data = ft_calloc(pq->nb + 1, sizeof(type));	\
		if (!new_data)											\
			return;												\
		ft_memcpy(new_data, pq->data, pq->nb * sizeof(type));	\
		new_data[pq->nb] = el;									\
		free(pq->data);											\
		pq->data = new_data;									\
		pq->nb++;												\
		for (int i = pq->nb / 2 - 1; i >= 0; i--)				\
			 __pq_heapify_##name(pq, i);						\
	}

#define make_pop_function(type, name)							\
	bool pq_##name##_pop(pq_##name##_t *pq, type *ret)			\
	{															\
		if (!pq->nb)											\
			return false;										\
		*ret = pq->data[0];										\
		__swap(&pq->data[0], &pq->data[pq->nb - 1]);			\
		pq->nb--;												\
		type *new_data = ft_calloc(pq->nb, sizeof(type));		\
		if (!new_data)											\
			return false;										\
		ft_memcpy(new_data, pq->data, pq->nb * sizeof(type));	\
		free(pq->data);											\
		pq->data = new_data;									\
		for (int i = pq->nb / 2 - 1; i >= 0; i--)				\
			 __pq_heapify_##name(pq, i);						\
		return true;											\
	}

#endif
