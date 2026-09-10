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
		if (!FLAG(ROOT_FLAG))
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
	printf("SEQ=%u\n", data->header.un.echo.sequence);

	// add header and timestamp
    gettimeofday(&data->time, NULL);
	memcpy(data->packet, (void *) &data->header, sizeof(struct icmphdr));
	memcpy(data->payload, (void *) &data->time, sizeof(data->time));

	// preload the pattern (0x00 if none) after the timestamp
	for (size_t i = 0; (i < PAYLOAD_SIZE - sizeof(data->time)) & FLAG(PATTERN_FLAG); i++)
		data->payload[i + sizeof(data->time)] = data->flag_bag.pattern_val[i % data->flag_bag.pattern_len];

	// checksum needs to be added after computation
	struct icmphdr *packet_hdr = (struct icmphdr *)data->packet;
	packet_hdr->un.echo.sequence = htons(data->header.un.echo.sequence);
    packet_hdr->checksum = checksum((void *) data->packet, sizeof(data->packet));

	// set for next packet
	data->header.un.echo.sequence++;
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
