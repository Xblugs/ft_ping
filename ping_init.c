/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping_init.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cczerwin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/21 17:48:49 by cczerwin          #+#    #+#             */
/*   Updated: 2026/08/21 17:48:50 by cczerwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>

#include "ping.h"

// Static section
static int	arg_parse(t_ping *data);
static int	socket_init(t_ping *data);
static void	get_sock_id(t_ping *data);
static void	ICMP_header_init(t_ping *data);

/*
	Initialize then start the ping pipeline
*/
int	ping_init(int *ac, char **av, t_ping *data)
{
	data->ac = ac;
	data->av = av;
	signal(SIGINT, &signal_handler);

	if (!getuid())
		SETFLAG(ROOT_FLAG);
	DEBUG_PRINT("root status: %d (id=%u)", FLAG(ROOT_FLAG), getuid());

	// killswitch for unprivileged mode
	if (!FLAG(ROOT_FLAG))
	{
		fprintf(stderr, "ping: socket: Operation not permitted\n");
		return (1);
	}

	if(arg_parse(data))
		return (1);

	if (FLAG(STOP_FLAG)) // -w or --timeout
	{
		signal(SIGALRM, &signal_handler);
		alarm(data->flag_bag.timeout_val);
	}

	ICMP_header_init(data);

	if (socket_init(data))
		return (1);

	if (data->ret)
		return (1);
	return (0);
}

/*
	Retrieve flags and their values
	[-?, -h, --help] don't need to be stored, execute directly
*/
static int arg_parse(t_ping *data)
{
	short host = 0;

	// '-' and '--' are valid inputs
	for (int i = 1; i < *data->ac; i++)
	{
		if (data->av[i][0] != '-' || strlen(data->av[i]) == 1)
		{
			host++;
			continue;
		}
		flag_wrapper(data->av[i], data);

		if (data->ret)
			break;
	}
	if (DEBUG)
		flag_type_debug(data);
	if (!host && !data->ret)
	{
		data->ret = 1;
		fprintf(stderr, "%s", MSG_MISS_HOST);
	}
	return (data->ret);
}

/*
	Try to use RAW sockets (legacy)
	Use DGRAM as fallback
*/
static int	socket_init(t_ping *data)
{
	// data->sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW); // require ip header
	data->sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP); // require root
	if (data->sockfd < 0)
		data->sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP);
	if (data->sockfd < 0)
	{
		fprintf(stderr, "ping: failed to open socket\n");
		return (1);
	}
	else
	{
		int	opt = 1;	// ask to receive the ttl for DGRAM sockets
		setsockopt(data->sockfd, SOL_IP, IP_RECVTTL, &opt, sizeof(opt));
		get_sock_id(data);
	}
	
	// set timeout: default 1s
	data->time = (struct timeval) {1, 0};
	setsockopt(data->sockfd, SOL_SOCKET, SO_RCVTIMEO, &data->time, sizeof(struct timeval));
	setsockopt(data->sockfd, IPPROTO_IP, IP_TTL, &data->flag_bag.ttl, sizeof(data->flag_bag.ttl));

	return (0);
}

/*
	Kernels erase the ICMP id and use the socket name instead
	We need it to check if the packet was intended for us
	(kernel manage the receiving/filtering part but it's better to check)
*/
static void	get_sock_id(t_ping *data)
{
	struct sockaddr_in sin = {0};
	socklen_t len = sizeof(sin);
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = 0; // 0 tells the kernel to assign an ID immediately

	if (bind(data->sockfd, (struct sockaddr *)&sin, len))
	{
		perror("ping: bind failed\n");
		data->ret = 1;
		return ;
	}

	if (getsockname(data->sockfd, (struct sockaddr *)&sin, &len) == 0)
	    data->sockid = ntohs(sin.sin_port);
	else
		perror("getsockname");
	DEBUG_PRINT("sockname is [%u]", data->sockid);
}

/*
	Set fields in the icmphdr structure
		un.echo is used for pings (ICMP_ECHO, ICMP_ECHOREPLY)
		un.gateway is used for redirections (ICMP_REDIRECT)
		un.frag is used when fragmentation is required but blocked (ICMP_DEST_UNREACH)
*/
static void	ICMP_header_init(t_ping *data)
{
	data->header.code 				= 0;
	data->header.type 				= ICMP_ECHO;
	data->header.checksum			= 0;
	data->header.un.echo.id 		= htons((uint16_t) (getpid() & 0xFFFF));
	// data->header.un.echo.sequence	= htons(-1); // reset in the loop later on

	// define the payload reserved space after the header
	// don't ever exceed HEADER_SIZE or kaboom
	data->payload = data->packet + HEADER_SIZE; 
}
