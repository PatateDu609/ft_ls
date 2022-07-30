#ifndef SORT_FUNCS_H
#define SORT_FUNCS_H

#include "structs.h"

#define __unused __attribute__((unused))

static inline int name_sort(const entry_t a, const entry_t b)
{
	return ft_strcmp(a.name, b.name);
}

static inline int reverse_name_sort(const entry_t a, const entry_t b)
{
	return -name_sort(a, b);
}

static inline int reverse_str_sort(const char *a, const char *b)
{
	return ft_strcmp(b, a);
}

static inline int time_sort(const entry_t a, const entry_t b)
{
	if (a.s->st_mtime < b.s->st_mtime)
		return 1;
	if (a.s->st_mtime > b.s->st_mtime)
		return -1;
	// If the files have the same st_mtime, we sort them by nanoseconds.
	if (a.s->st_mtim.tv_nsec < b.s->st_mtim.tv_nsec)
		return 1;
	if (a.s->st_mtim.tv_nsec > b.s->st_mtim.tv_nsec)
		return -1;

	return name_sort(a, b);
}

static inline int reverse_time_sort(const entry_t a, const entry_t b)
{
	return -time_sort(a, b);
}

static inline int no_sort(__unused const entry_t a, __unused const entry_t b)
{
	return 0;
}

#undef __unused

#endif
