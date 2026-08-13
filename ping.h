#pragma once

// echo "#include <netinet/ip_icmp.h>" | gcc -x c -E - | grep "netinet/ip_icmp.h"
// libc equivalent of <linux/icmp.h>, gives the icmphdr struct
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <string.h>

#define DEBUG			1

#define MSG_INVAL_OPT	"ping: invalid option -- "	// + "'option'\n"
#define MSG_MISS_HOST	"ping: missing host operand\n"
#define MSG_MORE_INFO	"Try 'ping --help' or 'ping --usage' for more information.\n"
#define MSG_VERSION		"ft_ping 1.0\nWritten by cczerwin.\n"

#define ECHO_REPLY		0
#define ECHO_REQUEST	8

#define FLOOD_FLAG		1
#define LOAD_FLAG		2
#define NUMERIC_FLAG	4
#define PATTERN_FLAG	8
#define IGN_ROUTE_FLAG	16
#define SIZE_FLAG		32
#define STOP_FLAG		64
#define WAIT_FLAG		128
#define TTL_FLAG		256
#define TYPE_FLAG		512
#define TIMESTAMP_FLAG	1024
#define VERBOSE_FLAG	2048
#define COUNT_FLAG		4096

// Forward declaration to keep things ordered
typedef struct s_ping t_ping;

typedef struct s_ping
{
	short			flags;
	short			count_val;
	short			linger_val;
	short			pattern_val;
	short			preload_val;
	short			size_val;
	short			ttl;
	short			timeout_val;
	char			timestamp_val;
	char			type_val;
	int				*ac;
	char			**av;
	struct icmphdr	header;
	char			ret;		// prevent allocating in each func
}	t_ping;

int		ping_init(int *ac, char **av, t_ping *data) __asm__("Init boi");
int 	arg_parse(t_ping *data) 					__asm__("Parsi boi");
void	flag_wrapper(char *flag, t_ping *data)		__asm__("Flaggi Wrappi");
