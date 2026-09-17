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

#include "ping.h"
#include <unistd.h>

// Static section
static int	arg_parse(t_ping *data);
static int	socket_init(t_ping *data);
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

	if(arg_parse(data))
		return (1);

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
	
	// set timeout: default 1s
	data->time = (struct timeval) {1, 0};
	setsockopt(data->sockfd, SOL_SOCKET, SO_RCVTIMEO, &data->time, sizeof(struct timeval));
	setsockopt(data->sockfd, IPPROTO_IP, IP_TTL, &data->flag_bag.ttl, sizeof(data->flag_bag.ttl));

	return (0);
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
	data->header.type 				= ECHO_REQUEST;
	data->header.checksum			= 0;
	data->header.un.echo.id 		= htons((uint16_t) getpid());
	// data->header.un.echo.sequence	= htons(-1); // reset in the loop later on

	// define the payload reserved space after the header
	// don't ever exceed HEADER_SIZE or kaboom
	data->payload = data->packet + HEADER_SIZE; 
}
