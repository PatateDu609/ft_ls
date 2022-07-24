#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdbool.h>
#include <stdint.h>
#include <sys/stat.h>
#include <dirent.h>

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
	struct stat *s;
	char *name;
} entry_t;

typedef struct
{
	uint16_t inode;	 // Displayed only if -i is passed.
	uint16_t blocks; // Displayed only if -s is passed (not implemented yet).

	// Displayed only if -l is passed.
	uint16_t perms;
	uint16_t links;
	uint16_t user;
	uint16_t group;
	uint16_t size;
	uint16_t date;
	uint16_t name;
} tab_long_listing_t;

#endif
