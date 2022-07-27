#include "ft_ls.h"
#include "libft.h"

// Colors should be defined by the $LS_COLORS environment variable which takes
// its values from the dircolors(1) command.

char **colors;
int colors_len;
char *reset;

static void init_colors(void) __attribute__((constructor));
static void fini_colors(void) __attribute__((destructor));

static void init_colors(void)
{
	char *color_str = getenv("LS_COLORS");

	if (!color_str)
		return;
	colors = ft_split(color_str, ':');

	reset = colors[0];

	for (colors_len = 0; colors[colors_len]; colors_len++)
	{
		if (ft_strchr(colors[colors_len], '*') != NULL)
			continue;
	}
}

static void fini_colors(void)
{
	for (int i = 0; i < colors_len; i++)
		free(colors[i]);
	free(colors);
}

static char *lookup_name(const char *name, const char *type)
{
	for (int i = colors_len - 1; i >= 0; i--)
	{
		char *c = ft_strdup(colors[i]);

		char *e = ft_strchr(c, '=');
		e[0] = '\0';
		char *color = e + 1;
		char *ent = c;
		e = ft_strchr(c, '.');
		e = e ? e : c;
		if ((name && ft_strcmp(name, e) == 0) || ft_strcmp(type, ent) == 0)
		{
			char *ret = ft_strdup(color);
			free(c);
			return ret;
		}
		free(c);
	}
	return NULL;
}

static char *get_type(const struct stat *s)
{
	char *type = "no";

	if (S_ISDIR(s->st_mode))
		type = "di";
	else if (S_ISLNK(s->st_mode))
		type = "ln";
	else if (S_ISREG(s->st_mode) && s->st_nlink > 1)
		type = "mh";
	else if (S_ISFIFO(s->st_mode))
		type = "pi";
	else if (S_ISSOCK(s->st_mode))
		type = "so";
	// Should be do (doors)
	else if (S_ISBLK(s->st_mode))
		type = "bd";
	else if (S_ISCHR(s->st_mode))
		type = "cd";
	else if (S_ISUID & s->st_mode)
		type = "su";
	else if (S_ISGID & s->st_mode)
		type = "sg";
	// Should be ca (file with capabilities)
	else if (s->st_mode & S_IWOTH)
		type = (S_ISVTX & s->st_mode) ? "tw" : "ow";
	else if (S_ISVTX & s->st_mode)
		type = "st";
	else if (S_IXUSR & s->st_mode)
		type = "ex";
	else if (S_ISREG(s->st_mode))
		type = "fi";

	return type;
}

void print_name(const char *name, const struct stat *s, int forced)
{
	char *ext = ft_strrchr(name, '.');

	char *type;
	if (forced == 1)
		type = "or";
	else if (forced == 2)
		type = "mi";
	else
		type = get_type(s);
	char *col = lookup_name(ext, type);

	if (col)
		printf("\033[%sm%s\033[0m", col, name);
	else
		printf("%s", name);
	free(col);
}
