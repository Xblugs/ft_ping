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
	if(arg_parse(data))
		return (1);

	ICMP_header_init(data);

	if (socket_init(data))
		return (1);

	if (data->ret)
	{
		fprintf(stderr, "%s", MSG_MISS_HOST);
		return (1);
	}
	return (0);
}

/*
	Retrieve flags and their values
	[-?, -h, --help] don't need to be stored, execute directly
*/
static int arg_parse(t_ping *data)
{
	// '-' and '--' are valid inputs
	for (int i = 1; i < *data->ac; i++)
	{
		if (data->av[i][0] != '-' || strlen(data->av[i]) == 1)
			continue;
		flag_wrapper(data->av[i], data);

		DEBUG_PRINT("Found %s\tflag = %#x", data->av[i], data->flag_bag.flags);
		if (data->ret)
			break;
	}
	
	return (0);
}

/*
	Try to use RAW sockets (legacy)
	Use DGRAM as fallback
*/
static int	socket_init(t_ping *data)
{
	// set timeout: default 1s
	struct timeval tv = {1, 0};
	setsockopt(data->sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	data->sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP); // require root
	if (data->sockfd >= 0)
		data->flag_bag.flags |= ROOT_FLAG;
	else
		data->sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP);
	if (data->sockfd < 0)
	{
		fprintf(stderr, "%s", MSG_MISS_HOST);
		return (1);
	}
	DEBUG_PRINT("root status: %d", (data->flag_bag.flags & ROOT_FLAG) == ROOT_FLAG);
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
	data->header.un.echo.id 		= htons((uint16_t) getpid());
	data->header.un.echo.sequence	= htons(1);

	// define the payload reserved space after the header
	// don't ever exceed HEADER_SIZE or kaboom
	data->payload = data->packet + HEADER_SIZE; 
}
