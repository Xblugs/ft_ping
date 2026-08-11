#pragma once

// echo "#include <netinet/ip_icmp.h>" | gcc -x c -E - | grep "netinet/ip_icmp.h"
// libc equivalent of <linux/icmp.h>, gives the 
#include <netinet/ip_icmp.h>
#include <stdio.h>

#define MSG_INVAL_OPT	"ping: invalid option -- "
#define MSG_MISS_HOST	"ping: missing host operand"
#define MSG_MORE_INFO	"Try 'ping --help' or 'ping --usage' for more information."

// Forward declaration to keep things ordered
typedef struct s_ping t_ping;

typedef struct s_ping
{
	short			flags;
	short			hostname_count;
	char			**hostname;		// book argv here and navigate
	struct icmphdr	header;
}	t_ping;

int recover_flags(int ac, char** av, t_ping data) __asm__("Flaggi boi");
