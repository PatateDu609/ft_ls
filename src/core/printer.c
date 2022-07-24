#include "priority_queue.h"
#include "ft_ls.h"
#include <pwd.h>
#include <grp.h>
#include <time.h>

static int reverse_sort(const char *a, const char *b)
{
	return (ft_strcmp(b, a));
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
	if (S_ISDIR(mode))
		ft_putchar_fd('d', 1);
	else if (S_ISLNK(mode))
		ft_putchar_fd('l', 1);
	else if (S_ISCHR(mode))
		ft_putchar_fd('c', 1);
	else if (S_ISBLK(mode))
		ft_putchar_fd('b', 1);
	else if (S_ISFIFO(mode))
		ft_putchar_fd('p', 1);
	else if (S_ISSOCK(mode))
		ft_putchar_fd('s', 1);
	else if (S_ISREG(mode))
		ft_putchar_fd('-', 1);
	else
		ft_putchar_fd('?', 1);

	dprintf(1, "%c%c", (mode & S_IRUSR) ? 'r' : '-', (mode & S_IWUSR) ? 'w' : '-');
	if (mode & S_ISUID)
		ft_putchar_fd((mode & S_IXUSR) ? 's' : 'S', 1);
	else
		ft_putchar_fd((mode & S_IXUSR) ? 'x' : '-', 1);
	dprintf(1, "%c%c", (mode & S_IRGRP) ? 'r' : '-', (mode & S_IWGRP) ? 'w' : '-');
	if (mode & S_ISGID)
		ft_putchar_fd((mode & S_IXGRP) ? 's' : 'S', 1);
	else
		ft_putchar_fd((mode & S_IXGRP) ? 'x' : '-', 1);
	dprintf(1, "%c%c", (mode & S_IROTH) ? 'r' : '-', (mode & S_IWOTH) ? 'w' : '-');
	if (mode & S_ISVTX)
		ft_putchar_fd((mode & S_IXOTH) ? 't' : 'T', 1);
	else
		ft_putchar_fd((mode & S_IXOTH) ? 'x' : '-', 1);

	dprintf(1, " ");
}

static void print_user_group(const struct stat *s)
{
	struct passwd *pw = getpwuid(s->st_uid);
	struct group *gr = getgrgid(s->st_gid);
	printf("%s %s ", pw->pw_name, gr->gr_name);
}

static void print_date(struct timespec ts)
{
	char *date = ctime(&ts.tv_sec);
	date[ft_strlen(date) - 1] = '\0';

	ft_strrchr(date, ':')[0] = '\0';
	printf("%s ", ft_strchr(date, ' ') + 1);
}

static void print_entry(const char *name, const struct stat *s, const conf_t *conf, struct cols col_conf, bool exact)
{
	const char *initial_name = name;
	if (!exact)
		name = ft_strrchr(name, '/') + 1;
	if (!conf->long_listing)
	{
		if (conf->tty)
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
		return;
	}

	if (conf->inode)
		dprintf(1, "%ld ", s->st_ino);

	print_perms(s->st_mode);
	printf("%ld ", s->st_nlink);
	print_user_group(s);
	printf("%ld ", s->st_size);
	print_date(s->st_mtim);

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
			struct stat lnk_stat;
			if (stat(link, &lnk_stat) == -1)
				forced = 1;
		}
		link[lnk_size] = '\0';
		print_name(name, s, forced);
		printf(" -> ");
		forced = (forced == 1) ? 2 : 0;
		print_name(link, s, forced);
	}
	else
		print_name(name, s, 0);
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
		if (lstat(path, &s) == -1)
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
	entry->d = opendir(entry->name);

	if (!entry->d)
	{
		char *err = ft_strjoin("ft_ls: ", entry->name);
		perror(err);
		free(err);

		return 1;
	}

	int (*cmp)(const char *, const char *) = conf->reverse ? reverse_sort : ft_strcmp;
	priority_queue_t *pq = priority_queue_new(cmp);

	priority_queue_t *pq_dir = NULL;
	if (conf->recursive)
		pq_dir = priority_queue_new(cmp);
	for (struct dirent *ent; (ent = readdir(entry->d));)
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
	closedir(entry->d);

	return 0;
}

int print_file(const conf_t *conf, entry_t *entry)
{
	struct stat s;
	if (lstat(entry->name, &s) == -1)
	{
		perror(entry->name);
		return 1;
	}
	print_entry(entry->name, &s, conf, (struct cols){0, 0}, true);
	printf("\n");
	return 0;
}
