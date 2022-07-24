#include "structs.h"
#include "ft_ls.h"

struct stat *ft_stat(char *path)
{
	struct stat *s;

	s = malloc(sizeof(struct stat));
	if (!s)
		return NULL;
	if (stat(path, s) == -1)
	{
		free(s);
		return NULL;
	}
	return s;
}
