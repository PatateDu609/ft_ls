#include "ft_getopt.h"
#include "defines.h"
#include "libft.h"

static int check_color(char *str)
{
	if (ft_strcmp(str, "never") == 0)
		return 1;
	if (ft_strcmp(str, "always") == 0)
		return 1;
	if (ft_strcmp(str, "auto") == 0)
		return 1;
	return 0;
}

static void init_t(t_option *option)
{
	option->name = NULL;
	option->short_name = 't';
	option->description = "Sort by time.";
	option->need_value = 0;
	option->value = NULL;
	option->flag = OPT_t;
}

void init_a(t_option *option)
{
	option->name = "all";
	option->short_name = 'a';
	option->description = "Do not ignore entries starting with '.'.";
	option->need_value = 0;
	option->value = NULL;
	option->flag = OPT_a;
}

void init_l(t_option *option)
{
	option->name = NULL;
	option->short_name = 'l';
	option->description = "List in long format.";
	option->need_value = 0;
	option->value = NULL;
	option->flag = OPT_l;
}

void init_r(t_option *option)
{
	option->name = "reverse";
	option->short_name = 'r';
	option->description = "List in reverse order.";
	option->need_value = 0;
	option->value = NULL;
	option->flag = OPT_r;
}

void init_R(t_option *option)
{
	option->name = "recursive";
	option->short_name = 'R';
	option->description = "List in recursive mode.";
	option->need_value = 0;
	option->value = NULL;
	option->flag = OPT_R;
}

void init_d(t_option *option)
{
	option->name = "directory";
	option->short_name = 'd';
	option->description = "List directory contents, not just the directory.";
	option->need_value = 0;
	option->value = NULL;
	option->flag = OPT_d;
}

void init_G(t_option *option)
{
	option->name = "group";
	option->short_name = 'G';
	option->description = "Do not list group information in long format.";
	option->need_value = 0;
	option->value = NULL;
	option->flag = OPT_G;
}

void init_g(t_option *option)
{
	option->name = "owner";
	option->short_name = 'g';
	option->description = "Do not list owner information in long format.";
	option->need_value = 0;
	option->value = NULL;
	option->flag = OPT_g;
}

void init_i(t_option *option)
{
	option->name = "inode";
	option->short_name = 'i';
	option->description = "List inode information.";
	option->need_value = 0;
	option->value = NULL;
	option->flag = OPT_i;
}

void init_m(t_option *option)
{
	option->name = "modified";
	option->short_name = 'm';
	option->description = "Fill width with a comma separated list of entries.";
	option->need_value = 0;
	option->value = NULL;
	option->flag = OPT_m;
}

void init_A(t_option *option)
{
	option->name = "all";
	option->short_name = 'A';
	option->description = "Do not list implied '.' and '..'.";
	option->need_value = 0;
	option->value = NULL;
	option->flag = OPT_A;
}

void init_L(t_option *option)
{
	option->name = "dereference";
	option->short_name = 'L';
	option->description = "Print information about the link itself, not the file referenced by it.";
	option->need_value = 0;
	option->value = NULL;
	option->flag = OPT_L;
}

void init_help(t_option *option)
{
	option->name = "help";
	option->short_name = 0;
	option->description = "Display this help and exit.";
	option->need_value = 0;
	option->value = NULL;
	option->flag = OPT_help;
}

void init_color(t_option *option)
{
	option->name = "color";
	option->short_name = 0;
	option->description = "Colorize the output.";
	option->need_value = 1;
	option->value = NULL;
	option->check = check_color;
	option->flag = OPT_color;
	option->arg_help = "WHEN";
	option->small_print = "Must be either 'never', 'always' or 'auto'.";
}

void init_s(t_option *option)
{
	option->name = "size";
	option->short_name = 's';
	option->description = "Print the allocated size of each file, in blocks.";
	option->need_value = 0;
	option->value = NULL;
	option->flag = OPT_s;
}

t_option *init_options(int *nb)
{
	void (*fct[])(t_option * option) = {
		init_help,
		init_color,
		init_a,
		init_A,
		init_d,
		init_g,
		init_G,
		init_i,
		init_l,
		init_L,
		init_m,
		init_r,
		init_R,
		init_t,
		init_s,
	};

	*nb = sizeof(fct) / sizeof(fct[0]);
	if (!*nb)
		return NULL;

	t_option *options = ft_calloc(*nb, sizeof(t_option));
	if (!options)
		return NULL;
	for (int i = 0; i < *nb; i++)
		fct[i](&options[i]);

	return options;
}
