#include "ft_ls.h"

#include "defines.h"

#define PQ_T char *
#define PQ_NAME str
#include "priority_queue.h"

#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int ft_dive_in(char *path, const conf_t *conf)
{
	entry_t entry;
	ft_memset(&entry, 0, sizeof(entry_t));
	ft_memcpy(entry.name, path, ft_strlen(path));

	entry.s = ft_stat(path);

	if (!entry.s)
	{
		char *err = ft_strjoin("ft_ls: ", path);
		perror(err);
		free(err);
		return 1;
	}

	if (S_ISDIR(entry.s->st_mode) && !conf->directory)
		print_dir(conf, &entry);
	else
		print_file(conf, &entry);

	free(entry.s);
	return 0;
}

static int ft_list(const conf_t *conf)
{
	int ret = 0;

	if (conf->paths[0])
		for (char **c = conf->paths; *c; c++)
			ret |= ft_dive_in(*c, conf);
	else
		ret = ft_dive_in(".", conf);

	return ret;
}

static size_t tty_width(void)
{
	struct winsize ws;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
	return ws.ws_col;
}

int get_color(t_args *args)
{
	if (!(args->flags & OPT_color))
		return COLOR_AUTO;
	for (int i = 0; i < args->nb_opt; i++)
	{
		if (ft_strcmp(args->options[i].name, "color") == 0)
		{
			char *val = args->options[i].value;
			if (ft_strcmp(val, "always") == 0)
				return COLOR_ALWAYS;
			else if (ft_strcmp(val, "never") == 0)
				return COLOR_NEVER;
			else if (ft_strcmp(val, "auto") == 0)
				return COLOR_AUTO;
		}
	}
	return COLOR_AUTO;
}

int ft_ls(t_args *args, bool tty)
{
	int ret = 0;
	conf_t conf = {0};

	conf.all = args->flags & OPT_a;
	conf.long_listing = args->flags & OPT_l;
	conf.reverse = args->flags & OPT_r;
	conf.time_sort = args->flags & OPT_t;
	conf.recursive = args->flags & OPT_R;
	conf.paths = args->args;

	conf.tty = conf.long_listing ? tty : tty & 0; // Used to avoid printing columns...
	conf.tty_width = tty ? tty_width() : 0;

	conf.almost_all = args->flags & OPT_A;
	conf.directory = args->flags & OPT_d;
	conf.no_owner = args->flags & OPT_g;
	conf.no_group = args->flags & OPT_G;
	conf.dereference = args->flags & OPT_L;
	conf.inode = args->flags & OPT_i;
	conf.comma = args->flags & OPT_m;
	conf.size = args->flags & OPT_s;
	conf.color = get_color(args);

	ret = ft_list(&conf);

	return ret;
}
