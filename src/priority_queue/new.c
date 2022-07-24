#include "priority_queue.h"
#include "libft.h"

priority_queue_t *priority_queue_new(int (*cmp)(const char *, const char *))
{
	priority_queue_t *pq = malloc(sizeof(priority_queue_t));

	if (!pq)
		return NULL;
	pq->el = NULL;
	pq->nb = 0;
	pq->cmp = cmp;
	return pq;
}
