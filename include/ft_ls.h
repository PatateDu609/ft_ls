#ifndef FT_LS_H
#define FT_LS_H

#include "ft_getopt.h"
#include "structs.h"
#include <stdbool.h>

int ft_ls(t_args *args, bool tty);
struct stat *ft_stat(char *path);

int ft_dive_in(char *path, const conf_t *conf);
int print_dir(const conf_t *conf, entry_t *entry);
int print_file(const conf_t *conf, entry_t *entry);
void print_name(const char *name, const struct stat *s, int forced);

void setup_cols(const conf_t *conf, pq_entry_t *pq, ug_t *ug, char **dates, ACL_XATTR_t *axt);

void set_ug(ug_t *ug, const struct stat *s, const conf_t *conf);
void set_date(char **date, struct timespec spec);
void set_axt(ACL_XATTR_t *axt, const char *name);

extern union cols_u cols;

#endif
