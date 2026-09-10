/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping_receive.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cczerwin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/31 19:10:50 by cczerwin          #+#    #+#             */
/*   Updated: 2026/08/31 19:10:52 by cczerwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"

// Static section
static void update_stats(t_stat *stats, double rtt);

/*
	Placeholder for the reception part
*/
int	ping_receive(t_ping *data, struct addrinfo *rp)
{
	struct iphdr 	*ip		= NULL;
	struct icmphdr 	*icmp	= NULL;
	char 	buffer[MAX_IP_SIZE];

	// wait for answer or timeout
	if (recvfrom(data->sockfd, buffer, MAX_IP_SIZE, NO_FLAG, rp->ai_addr, &rp->ai_addrlen) > 0)
	{
		ip = (struct iphdr *) buffer;
		icmp = (struct icmphdr *) (buffer + ip->ihl * 4);
  		if (DEBUG)
			icmp_type_debug(icmp->type);
	}
	else
		return (1);

	if (verify_checksum(icmp))
		return (1);

	// verify the ICMP id or socket id (depends on socket mode)
	if ((icmp->un.echo.id == data->header.un.echo.id)
			|| ntohs(icmp->un.echo.id) == data->sockid)
	{
		printf("Got pid=%u seq=%d\n",
			ntohs(icmp->un.echo.id), ntohs(icmp->un.echo.sequence));
	}
	else
	{
		DEBUG_PRINT("packet not for us");
		return (1);
	}

	char *payload = (void *)icmp + HEADER_SIZE;
	memcpy(&data->time, payload, sizeof(struct timeval));
	update_stats(&data->stat_bag, timespan_ms(&data->time));

	return (0);
}

/*
	Verify the received packet integrity
*/
int	verify_checksum(struct icmphdr *icmp)
{
	uint16_t old;

	old = icmp->checksum;
	icmp->checksum = 0;
	if (checksum(icmp, HEADER_SIZE + PAYLOAD_SIZE) != old)
	{
		if (DEBUG)
			DEBUG_PRINT("checksum not matching, packet dropped");
		return (1);
	}
	return (0);
}

/*
	Welford algorithm used for variance calculation (stddev)
	https://alchemmist.xyz/articles/welford-algorithm/
*/
static void update_stats(t_stat *stats, double rtt)
{
    stats->rx_count++;

    if (rtt < stats->min) stats->min = rtt;
    if (rtt > stats->max) stats->max = rtt;

    // Welford algorithm
    double delta = rtt - stats->avg;
    stats->avg += delta / stats->rx_count;
    stats->S += delta * (rtt - stats->avg);
}

void print_stats(t_stat *stats, char *host)
{
    if (stats->rx_count == 0)
	{
        printf("no packet received\n");
        return ;
    }

    // Variance = S / n
    double variance = stats->S / stats->rx_count;
    stats->stddev = sqrt(variance);

    printf("--- %s ping statistics ---\n", host);
	printf("%d packets transmitted, %d packets received, %.0f%% packet loss\n",
		stats->tx_count, stats->rx_count, 1.0 - ((float) stats->rx_count / (float) stats->tx_count));
    printf("rtt min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n",
           stats->min, stats->avg, stats->max, stats->stddev);
}
