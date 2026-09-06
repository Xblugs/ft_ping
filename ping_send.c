/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping_send.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cczerwin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/21 17:28:32 by cczerwin          #+#    #+#             */
/*   Updated: 2026/08/21 17:28:33 by cczerwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"

// Static section
static void assemble_packet(t_ping *data);
static void	get_sock_id(t_ping *data);

int	ping_send(t_ping *data, struct addrinfo *rp)
{
    DEBUG_PRINT("family=%d socktype=%d proto=%d addrlen=%d",
		       rp->ai_family, rp->ai_socktype, rp->ai_protocol, (int)rp->ai_addrlen);

    assemble_packet(data);
	ssize_t bytes_sent = sendto(data->sockfd, (void *) data->packet
		, sizeof(data->packet), NO_FLAG, rp->ai_addr, rp->ai_addrlen);

    DEBUG_PRINT("bytes sent: %lu\t(-1 = error)", bytes_sent);
	if (bytes_sent >= 0)
	{
		get_sock_id(data);
		data->stat_bag.tx_count++;
		return (0);
	}
	else if (bytes_sent == -1)
		perror("sendto");
    return (1);
}

/*
    Create the payload, assemble the packet and compute the checksum
*/
static void assemble_packet(t_ping *data)
{
	data->header.un.echo.sequence = ntohs(data->header.un.echo.sequence);
	data->header.un.echo.sequence++;
	data->header.un.echo.sequence = htons(data->header.un.echo.sequence);

	// preload the pattern (0x00 if none)
	for (size_t i = 0; i < PAYLOAD_SIZE; i++)
		data->payload[i] = data->flag_bag.pattern_val[i % data->flag_bag.pattern_len];

    gettimeofday(&data->time, NULL);
	memcpy(data->packet, (void *) &data->header, sizeof(struct icmphdr));
	memcpy(data->payload, (void *) &data->time, sizeof(data->time));
	data->header.checksum = checksum((void *) data->packet, sizeof(data->packet));
}

/*
	Kernels erase the ICMP id and use the socket name instead
	We need it to check if the packet was intended for us
	(kernel manage the receiving/filtering part but it's better to check)
*/
static void	get_sock_id(t_ping *data)
{
	struct sockaddr_in sin;
	socklen_t len = sizeof(sin);
	if (getsockname(data->sockfd, (struct sockaddr *)&sin, &len) == 0)
	    data->sockid = ntohs(sin.sin_port);
	else
		perror("getsockname");
	DEBUG_PRINT("sockname is [%u]", data->sockid);
}
