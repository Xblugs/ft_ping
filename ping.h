#pragma once

// command to check if a header is part of the standard lib
// echo "#include <netinet/ip_icmp.h>" | gcc -x c -E - | grep "netinet/ip_icmp.h"
// libc equivalent of <linux/icmp.h>, gives the icmphdr struct
#include <netdb.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <string.h>

// Debug mode & colorful prints
// This is not the verbose flag
#ifndef DEBUG
	#define DEBUG		0
#endif

// this macro was vibe coded
#if DEBUG == 1
    #define DEBUG_PRINT(fmt, ...) fprintf(stderr, PURPLE "[DEBUG]\t[" fmt "]\n" END, ##__VA_ARGS__)
	#define PURPLE			"\e[1;95m"
	#define END				"\e[0m"
#else
    #define DEBUG_PRINT(fmt, ...) do { } while ( 0 )	// prevent empty conditions errors
#endif

// TODO: Make macro for some of thoses?
#define MSG_REQ_ARG		"ping: option requires an argument -- "
#define MSG_REQ_ARG_W(arg)	printf("ping: option '%s' requires an argument\n", arg)

#define UNKNOWN_HOST	"ping: unknown host"
#define MSG_INVAL_ARG	"ping: invalid argument: "				// + "'arg'\n"
#define MSG_INVAL_OPT	"ping: invalid option -- "				// + "'option'\n"
#define MSG_INVAL_OPT2	"ping: unrecognized option -- "			// same but used for -- options
#define MSG_MISS_HOST	"ping: missing host operand\n"
#define MSG_MORE_INFO	"Try 'ping --help' or 'ping --usage' for more information.\n"
#define MSG_VERSION		"ft_ping version 1.0\nmade by cczerwin, based on inetutils-ping 2.6\n"
#define MSG_CNT_FLOOD	"ping: option value too small: " 		// + "'value'\n"
#define MSG_PATTERN_ERR "ping: error in pattern near "			// + "'value'\n"
#define MSG_VAL_TOO_BIG	"ping: option value too big: "			// + "'value'\n"
#define MSG_VAL_TOO_SMALL "ping: option value too small: "		// + "'value'\n"
#define TTL_EXCEED		"Time to live exceeded\n"

/*
	The max packet size for IPv4 is 65535 bytes
		20 bytes are needed for the IP header
		 8 bytes are needed for the ICMP header

	The payload size can be up to 65507 bytes (for a default ping)
*/
#define MAX_IP_SIZE		65535
#define HEADER_SIZE		8	// ICMP Header is 8 bytes
#define PAYLOAD_SIZE	56	// 32 is standard for windows, 56 for Unix

#define MAX_PATTERN_BYTES 16

// 16 flags <==> short
// upgrade to int if needed
#define FLAG(arg) 		((data->flag_bag.flags & arg) == arg)
#define SETFLAG(arg) 	(data->flag_bag.flags = data->flag_bag.flags | arg)

#define NO_FLAG			0
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
#define INTERVAL_FLAG	16384	// -i, --interval
#define PHOLD2_FLAG		32768	// currently placeholder

// Forward declaration to keep things ordered
typedef struct s_flag t_flag;
typedef struct s_stat t_stat;
typedef struct s_ping t_ping;

/*
	Tidy separations for all the possibles values
*/
typedef struct s_flag
{
	unsigned short	flags;
	unsigned short	count_val;
	unsigned short	interval;		// -i, time between pings in ms
	// unsigned short	linger_val;		// -W, timeout per response
	unsigned char	pattern_val[16];
	unsigned char	pattern_len;
	// unsigned short	preload_val;
	// unsigned int	size_val;
	unsigned int	ttl;
	unsigned int	timeout_val;	// -w, global timeout
	// unsigned char	timestamp_val;
	// unsigned char	type_val;
}	t_flag;

typedef struct s_stat
{
	int		tx_count;
	int		rx_count;
	double	min;
	double	avg;
	double	max;
	double	stddev; // standard deviation
	double 	S;		// used for calc
}	t_stat;

typedef struct s_ping
{
	t_flag			flag_bag;
	t_stat			stat_bag;
	int				sockfd;
	uint16_t		sockid;
	int				*ac;
	char			**av;
	struct icmphdr	header;
	struct timeval	time;
	char			packet[HEADER_SIZE + PAYLOAD_SIZE];
	char			*payload;
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

// ping_debug.c
void			address_format_debug(struct sockaddr_in *ip, char *av);
void			icmp_type_debug(uint8_t type);
void 			flag_type_debug(t_ping *data);

//	ping_flags_parse.c
int				parse_value_int(char *flag, t_ping *data, char type);
float 			parse_value_float(char *flag, t_ping *data, char type);
void			parse_value_hex(char *flag, t_ping *data, char type);

//	ping_flags_single.c
void			single_flag(char *flag, t_ping *data);

//	ping_flags_word.c
void			word_flag(char *flag, t_ping *data);

//	ping_flags.c
void			flag_wrapper(char *flag, t_ping *data)		__asm__("Flaggi_Wrappi");

int				unknown_flag(char *flag, char f);
int				invalid_argument(char *flag, char f);
int				invalid_pattern(char *flag, char f);
int 			print_usage(void);
int 			print_help(void);
int 			print_version(void);
int 			check_timeout_val(t_ping *data);

//	ping_init.c
int				ping_init(int *ac, char **av, t_ping *data);

//	ping_receive.c
int				ping_receive(t_ping *data, struct addrinfo *rp);
int				verify_checksum(struct icmphdr *icmp);
void			print_stats(t_stat *stats, char *host);

//	ping_send.c
int				ping_send(t_ping *data, struct addrinfo *rp) __asm__("Pinggi_Sendi");

// ping_utils.c
uint16_t 		checksum(void *data, int len)				__asm__("Checki");
double			timespan_ms(struct timeval *t0);
unsigned long	time_since(struct timeval *t0, struct timeval *t1);

//	ping.c
void			signal_handler(int sig);
void			do_ping(t_ping *data)						__asm__("Pinggi");
