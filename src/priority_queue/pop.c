#include "priority_queue.h"
#include "libft.h"
#include "internal.h"

static void swap(char **a, char **b)
{
	char *tmp = *a;
	*a = *b;
	*b = tmp;
}

char *priority_queue_pop(priority_queue_t *pq)
{
	if (!pq->nb)
		return NULL;

	char *ret = pq->el[0];
	swap(&pq->el[0], &pq->el[pq->nb - 1]);
	pq->nb--;

	char **new_el = ft_calloc(pq->nb, sizeof(char *));
	if (!new_el)
		return NULL;
	ft_memcpy(new_el, pq->el, pq->nb * sizeof(char *));
	free(pq->el);
	pq->el = new_el;

	for (int i = pq->nb / 2 - 1; i >= 0; i--)
		__pq_heapify(pq, i);
	return ret;
}
