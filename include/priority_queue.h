#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

typedef struct
{
	char **el;
	int nb;
	int (*cmp)(const char *, const char *);
} priority_queue_t;

priority_queue_t *priority_queue_new(int (*cmp)(const char *, const char *));
void priority_queue_free(priority_queue_t *pq);

void priority_queue_push(priority_queue_t *pq, const char *str);
char *priority_queue_pop(priority_queue_t *pq);

#endif
