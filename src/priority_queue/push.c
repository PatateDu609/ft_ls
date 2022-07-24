#include "priority_queue.h"
#include "libft.h"
#include "internal.h"

void priority_queue_push(priority_queue_t *pq, const char *str)
{
	char *s = ft_strdup(str);
	if (!s)
		return;

	if (!pq->nb)
	{
		pq->el = ft_calloc(1, sizeof(char *));
		if (!pq->el)
		{
			free(s);
			return;
		}

		pq->el[0] = s;
		pq->nb++;
		return;
	}
	char **new_el = ft_calloc(pq->nb + 1, sizeof(char *));
	if (!new_el)
	{
		free(s);
		return;
	}

	ft_memcpy(new_el, pq->el, pq->nb * sizeof(char *));
	new_el[pq->nb] = s;
	free(pq->el);

	pq->el = new_el;
	pq->nb++;
	for (int i = pq->nb / 2 - 1; i >= 0; i--)
		__pq_heapify(pq, i);
}
