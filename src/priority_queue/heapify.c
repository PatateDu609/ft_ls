#include "internal.h"

static void swap(char **a, char **b)
{
	char *tmp = *a;
	*a = *b;
	*b = tmp;
}

void __pq_heapify(priority_queue_t *pq, int i)
{
	if (pq->nb < 2)
		return;
	int largest = i;
	int left = 2 * i + 1;
	int right = 2 * i + 2;

	if (left < pq->nb && pq->cmp(pq->el[largest], pq->el[left]) > 0)
		largest = left;
	if (right < pq->nb && pq->cmp(pq->el[largest], pq->el[right]) > 0)
		largest = right;

	if (largest != i)
	{
		swap(&pq->el[i], &pq->el[largest]);
		__pq_heapify(pq, largest);
	}
}
