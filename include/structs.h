#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdbool.h>
#include <stdint.h>
#include <sys/stat.h>
#include <dirent.h>

#include "priority_queue.h"

typedef struct
{
	bool tty : 1;
	bool almost_all : 1;
	bool all : 1;
	bool long_listing : 1;
	bool recursive : 1;
	bool reverse : 1;
	bool time_sort : 1;
	bool directory : 1;

	bool no_group : 1;
	bool no_owner : 1;
	bool dereference : 1;
	bool inode : 1;
	bool comma : 1; // Not implemented
	bool size : 1;
	enum
	{
		COLOR_ALWAYS = 0b00,
		COLOR_AUTO = 0b01,
		COLOR_NEVER = 0b10,
	} color : 2;

	char **paths;
	size_t tty_width;
} conf_t;

typedef struct
{
	char *user;
	uid_t uid;

	char *group;
	gid_t gid;
} ug_t;


typedef struct
{
	struct stat *s;
	char name[1024];

	ug_t *ug;
	char *date;
} entry_t;

typedef struct
{
	uint16_t inode;	 // Displayed only if -i is passed.
	uint16_t blocks; // Displayed only if -s is passed.

	// Displayed only if -l is passed.
	uint16_t perms;
	uint16_t links;
	uint16_t user;
	uint16_t group;
	uint16_t size;
	uint16_t date;
	uint16_t name;
} __attribute__((packed)) tab_long_listing_t;

union cols_u
{
	uint16_t *width;
	tab_long_listing_t long_listing;
};

make_priority_queue(entry_t, entry)

#endif
