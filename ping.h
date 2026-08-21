#pragma once

// command to check if a header is part of the standard lib
// echo "#include <netinet/ip_icmp.h>" | gcc -x c -E - | grep "netinet/ip_icmp.h"
// libc equivalent of <linux/icmp.h>, gives the icmphdr struct
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

// Debug mode & colorful prints
// This is not the verbose flag
#define DEBUG			1
#define PURPLE			"\e[0;35m"
#define END				"\e[0m"

#define MSG_INVAL_OPT	"ping: invalid option -- "	// + "'option'\n"
#define MSG_MISS_HOST	"ping: missing host operand\n"
#define MSG_MORE_INFO	"Try 'ping --help' or 'ping --usage' for more information.\n"
#define MSG_VERSION		"ft_ping 1.0\nWritten by cczerwin.\n"

/*
	The max packet size for IPv4 is 65535 bytes
		20 bytes are needed for the IP header
		8  bytes are needed for the ICMP header

	The payload size can be up to 65507 bytes
*/
#define HEADER_SIZE		8	// ICMP Header is 8 bytes
#define PAYLOAD_SIZE	32	// 32 is standard for windows, 56 for Unix

#define ECHO_REPLY		0
#define ECHO_REQUEST	8

// 16 flags <==> short
// upgrade to int if needed
#define FLOOD_FLAG		1		// -f, --flood
#define LOAD_FLAG		2		// -l, --preload
#define NUMERIC_FLAG	4		// -n, --numeric
#define PATTERN_FLAG	8		// -p, --pattern
#define IGN_ROUTE_FLAG	16		// -r, --ignore-routing
#define SIZE_FLAG		32		// -s, --size
#define STOP_FLAG		64		// -w, --timeout
#define WAIT_FLAG		128		// -W, --linger
#define TTL_FLAG		256		// --ttl
#define TYPE_FLAG		512		// -T, --tos
#define TIMESTAMP_FLAG	1024	// --ip-timestamp
#define VERBOSE_FLAG	2048	// -v, --verbose
#define COUNT_FLAG		4096	// -c, --count
#define ROOT_FLAG		8192	// Internal flag, are we root or not
#define PHOLD1_FLAG		16384	// currently placeholder
#define PHOLD2_FLAG		32768	// currently placeholder

// Forward declaration to keep things ordered
typedef struct s_flag t_flag;
typedef struct s_ping t_ping;

/*
	Tidy separations for all the possibles values
*/
typedef struct s_flag
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
}	t_flag;

typedef struct s_ping
{
	t_flag			flag_bag;
	int				sockfd;
	int				*ac;
	char			**av;
	char			packet[HEADER_SIZE + PAYLOAD_SIZE];
	struct icmphdr	header;
	char			*payload;
	struct timeval	time;
	char			ret;		// return values
}	t_ping;

// Pasted here for convenience
// struct icmphdr
// {
//   uint8_t type;		/* message type */
//   uint8_t code;		/* type sub-code */
//   uint16_t checksum;
//   union
//   {
//     struct
//     {
//       uint16_t	id;
//       uint16_t	sequence;
//     } echo;			/* echo datagram */
//     uint32_t	gateway;	/* gateway address */
//     struct
//     {
//       uint16_t	__glibc_reserved;
//       uint16_t	mtu;
//     } frag;			/* path mtu discovery */
//   } un;
// };

//	ping.c

void			do_ping(t_ping *data)						__asm__("Pinggi");

//	ping_init.c
int				ping_init(int *ac, char **av, t_ping *data);

//	ping_flags.c
void			flag_wrapper(char *flag, t_ping *data)		__asm__("Flaggi Wrappi");

// ping_utils.c
unsigned long	timespan_us(struct timeval *t0)				__asm__("Smoll Time boi");
unsigned long	timespan_ms(struct timeval *t0)				__asm__("Time boi");
unsigned long	timespan(struct timeval *t0)				__asm__("Big Time boi");
