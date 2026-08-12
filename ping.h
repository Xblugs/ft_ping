#pragma once

// echo "#include <netinet/ip_icmp.h>" | gcc -x c -E - | grep "netinet/ip_icmp.h"
// libc equivalent of <linux/icmp.h>, gives the icmphdr struct
#include <netinet/ip_icmp.h>
#include <stdio.h>

#define MSG_INVAL_OPT	"ping: invalid option -- "	// + "'option'\n"
#define MSG_MISS_HOST	"ping: missing host operand\n"
#define MSG_MORE_INFO	"Try 'ping --help' or 'ping --usage' for more information.\n"

#define ECHO_REPLY		0
#define ECHO_REQUEST	8

// Forward declaration to keep things ordered
typedef struct s_ping t_ping;

typedef struct s_ping
{
	short			flags;
	short			host_count;
	char			**hostname;		// book argv here and navigate
	struct icmphdr	header;
}	t_ping;

int	ping_init(int ac, char **av, t_ping *data) __asm__("Init boi");
int arg_parse(int ac, char** av, t_ping *data) __asm__("Parsi boi");
