#include "priority_queue.h"
#include "ft_ls.h"
#include <pwd.h>
#include <grp.h>
#include <time.h>

static inline int reverse_sort(const char *a, const char *b)
{
	return (ft_strcmp(b, a));
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

static size_t get_longest(priority_queue_t *pq)
{
	size_t max = 0;
	for (int i = 0; i < pq->nb; i++)
	{
		size_t len = ft_strlen(pq->el[i]);
		if (len > max)
			max = len;
	}
	return max;
}

static void print_perms(mode_t mode)
{
	char perms[12];
	int i = 0;

	if (S_ISDIR(mode))
		perms[i] = 'd';
	else if (S_ISLNK(mode))
		perms[i] = 'l';
	else if (S_ISCHR(mode))
		perms[i] = 'c';
	else if (S_ISBLK(mode))
		perms[i] = 'b';
	else if (S_ISFIFO(mode))
		perms[i] = 'p';
	else if (S_ISSOCK(mode))
		perms[i] = 's';
	else if (S_ISREG(mode))
		perms[i] = '-';
	else
		perms[i] = '?';

	i++;

	perms[i++] = (mode & S_IRUSR) ? 'r' : '-';
	perms[i++] = (mode & S_IWUSR) ? 'w' : '-';
	if (mode & S_ISUID)
		perms[i++] = (mode & S_IXUSR) ? 's' : 'S';
	else
		perms[i++] = (mode & S_IXUSR) ? 'x' : '-';

	perms[i++] = (mode & S_IRGRP) ? 'r' : '-';
	perms[i++] = (mode & S_IWGRP) ? 'w' : '-';
	if (mode & S_ISGID)
		perms[i++] = (mode & S_IXGRP) ? 's' : 'S';
	else
		perms[i++] = (mode & S_IXGRP) ? 'x' : '-';

	perms[i++] = (mode & S_IROTH) ? 'r' : '-';
	perms[i++] = (mode & S_IWOTH) ? 'w' : '-';
	if (mode & S_ISVTX)
		perms[i++] = (mode & S_IXOTH) ? 't' : 'T';
	else
		perms[i++] = (mode & S_IXOTH) ? 'x' : '-';
	perms[i] = '\0';

	printf("%s ", perms);
}

static void print_user_group(const conf_t *conf, const struct stat *s)
{
	struct passwd *pw = getpwuid(s->st_uid);
	struct group *gr = getgrgid(s->st_gid);
	if (!conf->no_owner)
		printf("%s ", pw->pw_name);
	if (!conf->no_group)
		printf("%s ", gr->gr_name);
}

static void print_date(struct timespec ts)
{
	char *date = ctime(&ts.tv_sec);
	date[ft_strlen(date) - 1] = '\0';

	ft_strrchr(date, ':')[0] = '\0';
	printf("%s ", ft_strchr(date, ' ') + 1);
}

static void get_lnk_path(char *buf, const char *initial_path, const char *link)
{
	if (link[0] == '/')
		sprintf(buf, "%s", link);
	else
	{
		const char *first;
		char tmp;

		char *basename = ft_strrchr(initial_path, '/');
		if (!basename)
			first = ".";
		else
		{
			tmp = basename[0];
			basename[0] = '\0';
			first = initial_path;
		}
		sprintf(buf, "%s/%s", first, link);
		if (basename)
			basename[0] = tmp;
	}
}

static void print_colored(const char *name, const char *initial_name, const struct stat *s)
{
	if (S_ISLNK(s->st_mode))
	{
		int forced = 0;
		char link[PATH_MAX];

		ssize_t lnk_size = readlink(initial_name, link, PATH_MAX);
		if (lnk_size == -1)
		{
			printf("\n");
			perror("readlink");
			return;
		}
		else
		{
			link[lnk_size] = '\0';

			char buf[PATH_MAX];
			get_lnk_path(buf, initial_name, link);
			struct stat lnk_stat;
			if (stat(buf, &lnk_stat) == -1)
				forced = 1;
			print_name(name, &lnk_stat, forced);
		}
		printf(" -> ");
		forced = (forced == 1) ? 2 : 0;
		print_name(link, s, forced);
	}
	else
		print_name(name, s, 0);
}

static void print_not_colored(const char *name, const char *initial_name, const struct stat *s)
{
	if (S_ISLNK(s->st_mode))
	{
		char link[PATH_MAX];
		ssize_t lnk_size = readlink(initial_name, link, PATH_MAX);
		if (lnk_size == -1)
		{
			printf("\n");
			perror("readlink");
			return;
		}
		link[lnk_size] = '\0';
		printf("%s -> %s", name, link);
	}
	else
		printf("%s", name);
}

static void print_entry(const char *name, const struct stat *s, const conf_t *conf, struct cols col_conf, bool exact)
{
	const char *initial_name = name;
	if (!exact)
		name = ft_strrchr(name, '/') + 1;
	if (conf->inode)
		printf("%ld ", s->st_ino);

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
			if (S_ISLNK(s->st_mode))
			{
				char buf[PATH_MAX];
				if (readlink(initial_name, buf, PATH_MAX) == -1)
					forced = 1;
			}
			print_name(name, s, forced);
			printf("\n");
		}
		else
			printf("%s\n", name);
		return;
	}

	print_perms(s->st_mode);
	printf("%ld ", s->st_nlink);

	if ((!conf->no_group && !conf->no_owner) || conf->no_group != conf->no_owner)
		print_user_group(conf, s);
	printf("%ld ", s->st_size);
	print_date(s->st_mtim);

	if (conf->color == COLOR_NEVER || (conf->color == COLOR_AUTO && !conf->tty))
		print_not_colored(name, initial_name, s);
	else
		print_colored(name, initial_name, s);
}

static void print_queue(char *path, priority_queue_t *pq, const conf_t *conf)
{
	struct cols col_conf;
	struct stat s;

	if (!conf->long_listing && conf->tty)
	{
		size_t longest = get_longest(pq);
		col_conf.col_width = longest + 1;
		col_conf.cols = conf->tty_width / col_conf.col_width;
	}
	for (char *name; (name = priority_queue_pop(pq));)
	{
		char *arr[] = {path, name, NULL};
		char *path = ft_strjoin_arr(arr, '/');
		if (ft_stat_dispatcher(conf, path, &s) == -1)
		{
			perror(path);
			free(path);
			continue;
		}
		print_entry(path, &s, conf, col_conf, false);

		if (conf->long_listing || !pq->nb)
			printf("\n");

		free(path);
		free(name);
	}
}

static void print_dir_queue(char *path, priority_queue_t *pq, const conf_t *conf)
{
	for (char *name; (name = priority_queue_pop(pq));)
	{
		char *arr[] = {path, name, NULL};
		char *path = ft_strjoin_arr(arr, '/');
		if (!path)
		{
			perror(path);
			free(name);
			exit(EXIT_FAILURE);
		}

		ft_dive_in(path, conf);
		free(path);
		free(name);
	}
}

int print_dir(const conf_t *conf, entry_t *entry)
{
	if (conf->recursive)
		printf("\n%s:\n", entry->name);
	DIR *d = opendir(entry->name);

	if (!d)
	{
		char *err = ft_strjoin("ft_ls: ", entry->name);
		perror(err);
		free(err);

		return 1;
	}
	if (conf->long_listing)
		printf("Total %ld\n", entry->s->st_blocks);

	int (*cmp)(const char *, const char *) = conf->reverse ? reverse_sort : ft_strcmp;
	priority_queue_t *pq = priority_queue_new(cmp);

	priority_queue_t *pq_dir = NULL;
	if (conf->recursive)
		pq_dir = priority_queue_new(cmp);
	for (struct dirent *ent; (ent = readdir(d));)
	{
		if (ent->d_name[0] == '.' && !conf->all)
			continue;
		if (conf->recursive && ent->d_type == DT_DIR)
			priority_queue_push(pq_dir, ent->d_name);

		priority_queue_push(pq, ent->d_name);
	}
	print_queue(entry->name, pq, conf);
	if (conf->recursive)
		print_dir_queue(entry->name, pq_dir, conf);

	priority_queue_free(pq);
	priority_queue_free(pq_dir);

	closedir(d);
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
	print_entry(entry->name, &s, conf, (struct cols){0, 0}, true);
	printf("\n");
	return 0;
}
