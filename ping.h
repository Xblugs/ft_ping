#pragma once

// echo "#include <netinet/ip_icmp.h>" | gcc -x c -E - | grep "netinet/ip_icmp.h"
// libc equivalent of <linux/icmp.h>, gives the icmphdr struct
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define DEBUG			1

#define MSG_INVAL_OPT	"ping: invalid option -- "	// + "'option'\n"
#define MSG_MISS_HOST	"ping: missing host operand\n"
#define MSG_MORE_INFO	"Try 'ping --help' or 'ping --usage' for more information.\n"
#define MSG_VERSION		"ft_ping 1.0\nWritten by cczerwin.\n"

#define ECHO_REPLY		0
#define ECHO_REQUEST	8

// 16 flags <==> short
// upgrade to int if needed
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
#define PLACEHOLDER0_FLAG		8192
#define PLACEHOLDER1_FLAG		16384
#define PLACEHOLDER2_FLAG		32768

// Forward declaration to keep things ordered
typedef struct s_ping t_ping;

typedef struct s_ping
{
	unsigned short	flags;
	unsigned short	count_val;
	unsigned short	linger_val;
	unsigned short	pattern_val;
	unsigned short	preload_val;
	unsigned short	size_val;
	unsigned short	ttl;
	unsigned short	timeout_val;
	unsigned char	timestamp_val;
	unsigned char	type_val;
	int				*ac;
	char			**av;
	struct icmphdr	header;
	char			ret;		// prevent allocating in each func
}	t_ping;

int		ping_init(int *ac, char **av, t_ping *data) __asm__("Init boi");
int 	arg_parse(t_ping *data) 					__asm__("Parsi boi");
void	flag_wrapper(char *flag, t_ping *data)		__asm__("Flaggi Wrappi");
