#ifndef DEFINES_H
#define DEFINES_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <errno.h>

#define OPT_t 0x01
#define OPT_a 0x02
#define OPT_l 0x04
#define OPT_r 0x08
#define OPT_R 0x10
#define OPT_d 0x20
#define OPT_G 0x40
#define OPT_g 0x80
#define OPT_i 0x100
#define OPT_m 0x200
#define OPT_A 0x800
#define OPT_L 0x1000
#define OPT_help 0x2000
#define OPT_color 0x4000

#define program_name program_invocation_short_name

#endif
