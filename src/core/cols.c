#include "ft_ls.h"
#include <pwd.h>
#include <grp.h>
#include <time.h>

enum {
	ENTRY_UID,
	ENTRY_GID,
};

#define MAX(a, b) ((a) > (b) ? (a) : (b))

__attribute__((pure))
__attribute__((optimize("O3")))
static uint16_t uint64_len(uint64_t n)
{
	if (n < 10)
		return 1;
	if (n < 100)
		return 2;
	if (n < 1000)
		return 3;
	if (n < 10000)
		return 4;
	if (n < 100000)
		return 5;
	if (n < 1000000)
		return 6;
	if (n < 10000000)
		return 7;
	if (n < 100000000)
		return 8;
	if (n < 1000000000)
		return 9;
	if (n < 10000000000)
		return 10;
	if (n < 100000000000)
		return 11;
	if (n < 1000000000000)
		return 12;
	if (n < 10000000000000)
		return 13;
	if (n < 100000000000000)
		return 14;
	if (n < 1000000000000000)
		return 15;
	if (n < 10000000000000000)
		return 16;
	if (n < 100000000000000000)
		return 17;
	if (n < 1000000000000000000)
		return 18;
	return 19;
}

void set_ug(ug_t *ug, const struct stat *s, const conf_t *conf)
{
	if (!conf->no_owner)
	{
		struct passwd *pw = getpwuid(s->st_uid);
		ug->uid = s->st_uid;
		ug->user = pw->pw_name;
	}
	if (!conf->no_group)
	{
		struct group *gr = getgrgid(s->st_gid);
		ug->gid = s->st_gid;
		ug->group = gr->gr_name;
	}
}

void set_date(char **date, struct timespec spec)
{
	time_t t;
	time(&t);
	char *d = ctime(&spec.tv_sec);
	size_t len = ft_strlen(d);
	d[len - 1] = '\0';


	char *dot = ft_strrchr(d, ':');
	if (dot)
		dot[0] = '\0';
	char *space = ft_strchr(d, ' ');
	if (space)
		d = space + 1;
	*date = ft_strdup(d);
}

void setup_cols(const conf_t *conf, pq_entry_t *pq, ug_t *ug, char **dates)
{
	if (conf->long_listing)
	{
		tab_long_listing_t len = {0};

		entry_t e;
		for (int i = 0; pq_entry_pop(pq, &e); i++)
		{
			set_ug(ug + i, e.s, conf);
			set_date(dates + i, e.s->st_mtim);

			len.inode = MAX(len.inode, uint64_len(e.s->st_ino));
			len.blocks = MAX(len.blocks, uint64_len(e.s->st_blocks >> 1));
			len.perms = 10; // Management of ACL and extended attributes is not implemented yet.
			len.links = MAX(len.links, uint64_len(e.s->st_nlink));
			len.user = MAX(len.user, ft_strlen((ug + i)->user));
			len.group = MAX(len.group, ft_strlen((ug + i)->group));
			len.size = MAX(len.size, uint64_len(e.s->st_size));
			len.date = MAX(len.date, ft_strlen(dates[i]));
			len.name = 0;
		}

		cols.long_listing = len;
	}
}
