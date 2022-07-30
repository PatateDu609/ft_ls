
#define PQ_T char *
#define PQ_NAME str
#include "priority_queue.h"

#include "ft_ls.h"
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#include "sort_funcs.h"

#include <fcntl.h>

union cols_u cols;

static inline void merge_path(char *target, size_t buf_size, const char *path, const char *name)
{
	if (path[0] == '\0' || ft_strcmp(path, "/") == 0)
		snprintf(target, buf_size, "/%s", name);
	else if (path[ft_strlen(path) - 1] != '/')
		snprintf(target, buf_size, "%s/%s", path, name);
	else
		snprintf(target, buf_size, "%s%s", path, name);
}

void free_entry(entry_t entry)
{
	free(entry.s);
}

void free_str(char *str)
{
	free(str);
}

static int ft_stat_dispatcher(const conf_t *conf, char *path, struct stat *s)
{
	if (!conf->dereference)
		return lstat(path, s);
	else
		return stat(path, s);
}

struct cols
{
	int cols;
	int col_width;
};

static void print_perms(mode_t mode, ACL_XATTR_t axt)
{
	char perms[12];

	if (S_ISDIR(mode))
		perms[0] = 'd';
	else if (S_ISLNK(mode))
		perms[0] = 'l';
	else if (S_ISCHR(mode))
		perms[0] = 'c';
	else if (S_ISBLK(mode))
		perms[0] = 'b';
	else if (S_ISFIFO(mode))
		perms[0] = 'p';
	else if (S_ISSOCK(mode))
		perms[0] = 's';
	else if (S_ISREG(mode))
		perms[0] = '-';
	else
		perms[0] = '?';

	perms[1] = (mode & S_IRUSR) ? 'r' : '-';
	perms[2] = (mode & S_IWUSR) ? 'w' : '-';
	if (mode & S_ISUID)
		perms[3] = (mode & S_IXUSR) ? 's' : 'S';
	else
		perms[3] = (mode & S_IXUSR) ? 'x' : '-';

	perms[4] = (mode & S_IRGRP) ? 'r' : '-';
	perms[5] = (mode & S_IWGRP) ? 'w' : '-';
	if (mode & S_ISGID)
		perms[6] = (mode & S_IXGRP) ? 's' : 'S';
	else
		perms[6] = (mode & S_IXGRP) ? 'x' : '-';

	perms[7] = (mode & S_IROTH) ? 'r' : '-';
	perms[8] = (mode & S_IWOTH) ? 'w' : '-';
	if (mode & S_ISVTX)
		perms[9] = (mode & S_IXOTH) ? 't' : 'T';
	else
		perms[9] = (mode & S_IXOTH) ? 'x' : '-';
	perms[10] = axt;
	perms[11] = '\0';

	printf("%*s ", cols.long_listing.perms, perms);
}

static void print_user_group(ug_t *ug, const conf_t *conf)
{
	if (!conf->no_owner)
		printf("%*s ", cols.long_listing.user, ug->user);
	if (!conf->no_group)
		printf("%*s ", cols.long_listing.group, ug->group);
}

// static char *get_link(const char *path)
// {
// 	char lnk[PATH_MAX];

// 	if (readlink(path, lnk, PATH_MAX) == -1)
// 		return NULL;
// }

static void print_colored(const char *basename, const char *path, const struct stat *s)
{
	if (S_ISLNK(s->st_mode))
	{
		char rel[2 * PATH_MAX];
		merge_path(rel, sizeof rel, path, basename);

		char link[PATH_MAX];
		ssize_t len = readlink(rel, link, sizeof link);
		if (len == -1)
		{
			printf("\n");
			fprintf(stderr, "ft_ls: %s: %s\n", rel, strerror(errno));
			return;
		}
		link[len] = '\0';

		int forced = 0;
		struct stat link_stat;
		merge_path(rel, sizeof rel, path, link);
		if (lstat(rel, &link_stat) == -1)
			forced = 1;
		print_name(basename, s, forced);
		printf(" -> ");
		print_name(link, &link_stat, forced == 1 ? 2 : 0);
	}
	else
		print_name(basename, s, 0);
}

static void print_not_colored(const char *basename, const char *path, const struct stat *s)
{
	if (S_ISLNK(s->st_mode))
	{
		char link[PATH_MAX];
		char target[2 * PATH_MAX];
		merge_path(target, PATH_MAX, path, basename);

		ssize_t lnk_size = readlink(target, link, PATH_MAX);
		if (lnk_size == -1)
		{
			printf("\n");
			fprintf(stderr, "ft_ls: %s: %s\n", target, strerror(errno));
			return;
		}
		link[lnk_size] = '\0';
		printf("%s -> %s", basename, link);
	}
	else
		printf("%s", basename);
}

static void print_entry(const entry_t *e, const conf_t *conf, struct cols col_conf, bool exact)
{
	const char *name = e->name;

	const char *basename = name;
	char *path = ft_strrchr(e->name, '/');
	if (path && !exact)
	{
		basename = path + 1;
		path = ft_strndup(e->name, path - e->name + 1);
	}
	else if (path)
		path = ft_strdup(path);

	if (conf->inode)
	{
		if (conf->long_listing)
			printf("%*lu ", cols.long_listing.inode, e->s->st_ino);
		else
			printf("%lu ", e->s->st_ino);
	}
	if (conf->size)
	{
		if (conf->long_listing)
			printf("%*ld ", cols.long_listing.blocks, e->s->st_blocks >> 1);
		else
			printf("%ld ", e->s->st_blocks >> 1);
	}

	if (!conf->long_listing)
	{
		if (conf->tty)
		{
			if (conf->color != COLOR_NEVER)
			{
				static int col = 0;
				printf("%-*s", col_conf.col_width, name);
				col++;
				if (col == col_conf.cols)
				{
					printf("\n");
					col = 0;
				}
			}
			else
				printf("%-*s\n", col_conf.col_width, name);
		}
		else if (conf->color == COLOR_ALWAYS)
		{
			int forced = 0;
			if (S_ISLNK(e->s->st_mode))
			{
				char buf[PATH_MAX];
				if (readlink(name, buf, PATH_MAX) == -1)
					forced = 1;
			}
			print_name(basename, e->s, forced);
			printf("\n");
		}
		else
			printf("%s\n", basename);

		free(path);
		return;
	}

	print_perms(e->s->st_mode, e->acl_xattr);
	printf("%*ld ", cols.long_listing.links, e->s->st_nlink);

	if ((!conf->no_group && !conf->no_owner) || conf->no_group != conf->no_owner)
		print_user_group(e->ug, conf);
	printf("%*ld ", cols.long_listing.size, e->s->st_size);
	printf("%*s ", cols.long_listing.date, e->date);

	if (conf->color == COLOR_NEVER || (conf->color == COLOR_AUTO && !conf->tty))
		print_not_colored(basename, path, e->s);
	else
		print_colored(basename, path, e->s);
	free(path);
}

static void print_queue(char *path, pq_entry_t *pq, const conf_t *conf)
{
	struct cols col_conf;
	char **dates = NULL;
	ug_t *ug = NULL;
	ACL_XATTR_t *axts = NULL;

	if (conf->long_listing)
	{
		ug = ft_calloc(pq->size, sizeof *ug);
		if (!ug)
		{
			perror("ft_calloc");
			exit(EXIT_FAILURE);
		}
		dates = ft_calloc(pq->size, sizeof *dates);
		if (!dates)
		{
			perror("ft_calloc");
			exit(EXIT_FAILURE);
		}
		axts = ft_calloc(pq->size, sizeof *axts);
	}

	pq_entry_t *dup = pq_entry_new(pq->cmp);
	if (!dup)
	{
		perror("pq_entry_new");
		exit(EXIT_FAILURE);
	}

	dup->free_data = pq->free_data;
	dup->size = pq->size;
	dup->data = ft_calloc(dup->size, sizeof *dup->data);
	if (!dup->data)
	{
		perror("ft_calloc");
		exit(EXIT_FAILURE);
	}
	ft_memcpy(dup->data, pq->data, dup->size * sizeof *dup->data);

	entry_t entry;
	setup_cols(conf, dup, ug, dates, axts);
	int i = 0;
	pq_entry_free(dup);
	for (bool ret; (ret = pq_entry_pop(pq, &entry)); i++)
	{
		char p[2 * PATH_MAX] = {0};
		merge_path(p, sizeof p, path, entry.name);
		ft_memcpy(entry.name, p, sizeof p);

		if (conf->long_listing)
		{
			entry.date = dates[i];
			entry.ug = ug + i;
		}
		print_entry(&entry, conf, col_conf, false);

		if (conf->long_listing || !pq->size)
			printf("\n");

		if (conf->long_listing)
		{
			free(dates[i]);
			free_entry(entry);
		}
	}
	free(dates);
	free(ug);
	free(axts);
}

// 7:57:00

static void print_dir_queue(char *path, pq_str_t *pq, const conf_t *conf)
{
	char *name;
	for (bool ret; (ret = pq_str_pop(pq, &name));)
	{
		char p[2 * PATH_MAX] = {0};
		merge_path(p, sizeof p, path, name);

		ft_dive_in(p, conf);
		free(name);
	}
}

static pq_entry_cmp_fun *get_cmp_entry_fun(const conf_t *conf)
{
	if (conf->reverse)
		return conf->time_sort ? reverse_time_sort : reverse_name_sort;

	return conf->time_sort ? time_sort : name_sort;
}

int print_dir(const conf_t *conf, entry_t *entry)
{
	if (conf->recursive)
		printf("\n%s:\n", entry->name);
	DIR *d = opendir(entry->name);

	if (!d)
	{
		fprintf(stderr, "ls: %s: %s\n", entry->name, strerror(errno));
		return 1;
	}

	pq_entry_cmp_fun *cmp = get_cmp_entry_fun(conf);
	pq_entry_t *pq = pq_entry_new(cmp);
	pq->free_data = &free_entry;
	size_t blks = 0;

	pq_str_t *pq_dir = NULL;
	if (conf->recursive)
	{
		pq_str_cmp_fun *cmp_dir = conf->reverse ? reverse_str_sort : ft_strcmp;
		pq_dir = pq_str_new(cmp_dir);
		pq_dir->free_data = free_str;
	}
	for (struct dirent *ent; (ent = readdir(d));)
	{
		if (ent->d_name[0] == '.' && !conf->all)
		{
			if (!conf->almost_all || (conf->almost_all &&
				(ft_strcmp(".", ent->d_name) == 0 ||
				ft_strcmp("..", ent->d_name) == 0)))
				continue;
		}
		if (conf->recursive && ent->d_type == DT_DIR)
		{
			if (ft_strcmp(".", ent->d_name) && ft_strcmp("..", ent->d_name))
			{
				char *name = ft_strdup(ent->d_name);
				pq_str_push(pq_dir, name);
			}
		}

		entry_t new_entry;
		ft_memset(&new_entry, 0, sizeof(entry_t));
		ft_memcpy(new_entry.name, ent->d_name, sizeof(ent->d_name));
		new_entry.s = ft_calloc(1, sizeof *new_entry.s);
		if (!new_entry.s)
		{
			perror("malloc");
			exit(EXIT_FAILURE);
		}

		char path[PATH_MAX] = {0};
		int ret = snprintf(path, PATH_MAX, "%s", entry->name);
		if (entry->name[ft_strlen(entry->name) - 1] != '/')
			ret = snprintf(path + ret, PATH_MAX - ret, "/%s", ent->d_name);
		else
			ret = snprintf(path + ret, PATH_MAX - ret, "%s", ent->d_name);
		if (ft_stat_dispatcher(conf, path, new_entry.s) == -1)
		{
			perror(path);
			free(new_entry.s);
			continue;
		}

		if (conf->long_listing)
			blks += new_entry.s->st_blocks;

		pq_entry_push(pq, new_entry);
	}
	closedir(d);

	if (conf->long_listing)
		printf("total %ld\n", blks >> 1);

	print_queue(entry->name, pq, conf);
	pq_entry_free(pq);

	if (conf->recursive)
		print_dir_queue(entry->name, pq_dir, conf);
	pq_str_free(pq_dir);
	return 0;
}

int print_file(const conf_t *conf, entry_t *entry)
{
	struct stat s;

	if (ft_stat_dispatcher(conf, entry->name, &s) == -1)
	{
		perror(entry->name);
		return 1;
	}
	entry->ug = ft_calloc(1, sizeof *entry->ug);
	set_ug(entry->ug, &s, conf);
	set_date(&entry->date, s.st_mtim);
	set_axt(&entry->acl_xattr, entry->name);

	print_entry(entry, conf, (struct cols){0, 0}, true);
	printf("\n");

	free(entry->ug);
	entry->ug = NULL;
	free(entry->date);
	entry->date = NULL;
	return 0;
}
