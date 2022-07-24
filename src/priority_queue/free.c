#include "priority_queue.h"
#include <stdlib.h>

void priority_queue_free(priority_queue_t *pq)
{
	if (!pq)
		return;

	for (int i = 0; i < pq->nb; i++)
		free(pq->el[i]);
	free(pq->el);
	free(pq);
}
