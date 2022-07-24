#include <unistd.h>
#include <stdio.h>

#include "defines.h"

#include "ft_getopt.h"
#include "ft_ls.h"

void print_help(t_args *args)
{
	printf("Usage: %s [OPTION]... ADDRESS\n", program_invocation_short_name);
	for (int i = 0; i < args->nb_opt; i++)
	{
		printf("\n");
		if (args->options[i].short_name)
			printf("\033[1;31m-%c\033[0m", args->options[i].short_name);
		if (args->options[i].name)
			printf("%s--%s", (args->options[i].short_name) ? ", " : "", args->options[i].name);
		if (args->options[i].need_value && args->options[i].small_print)
		{
			printf("%c", args->options[i].name ? '=' : '\t');
			printf("\033[38;5;149m%s\033[0m", args->options[i].arg_help);
		}
		printf("\n\t%s\n", args->options[i].description);
	}
}
int main(int ac, char **av)
{
	t_args *args = parse_args(ac, av);
	if (!args || args == (void *)-1)
		return 1;

	if (args->flags & OPT_help)
	{
		print_help(args);
		free_args(args);
		return 0;
	}

	int ret = ft_ls(args, isatty(STDOUT_FILENO));
	free_args(args);
	return ret;
}
