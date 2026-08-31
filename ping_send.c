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

int	ping_send(t_ping *data, struct addrinfo *rp)
{
    DEBUG_PRINT("family=%d socktype=%d proto=%d addrlen=%d",
		       rp->ai_family, rp->ai_socktype, rp->ai_protocol, (int)rp->ai_addrlen);

    assemble_packet(data);
	ssize_t bytes_sent = sendto(data->sockfd, (void *) data->packet
		, sizeof(data->packet), NO_FLAG, rp->ai_addr, rp->ai_addrlen);

    DEBUG_PRINT("bytes sent: %lu\t(-1 = error)", bytes_sent);
	if (bytes_sent >= 0)
		return (0);
	else if (bytes_sent == -1)
		perror("sendto");
    return (1);
}

/*
    Create the payload, assemble the packet and compute the checksum
*/
static void assemble_packet(t_ping *data)
{
    gettimeofday(&data->time, NULL);
	memcpy(data->packet, (void *) &data->header, sizeof(struct icmphdr));
	memcpy(data->payload, (void *) &data->time, sizeof(data->time));
	data->header.checksum = checksum((void *) data->packet, sizeof(data->packet));
}
