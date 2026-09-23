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
static int	verify_packet(t_ping *data, struct iphdr *ip, struct icmphdr *icmp, struct addrinfo *rp);
static int	manage_verbose(t_ping *data, struct iphdr *ip, struct icmphdr *icmp);
static void print_ping_reply(struct sockaddr_in *from, struct icmphdr *icmp, int ttl, double rtt_ms, int bytes_received);

/*
	Placeholder for the reception part
*/
int	ping_receive(t_ping *data, struct addrinfo *rp)
{
	struct iphdr 	*ip		= NULL;
	struct icmphdr 	*icmp	= NULL;
	char 	buffer[MAX_IP_SIZE];
	ssize_t	process_status 	= 1;

	while (process_status) 
	{
		/* 
			Receive packet
			--> Verify the checksum validity
			--> Verify the type (ECHO_REPLY if everything went OK)
		*/

		ip 	 = NULL;
		icmp = NULL;
		// if (FLAG(ROOT_FLAG))
			process_status = recvfrom(data->sockfd, buffer, MAX_IP_SIZE, NO_FLAG, rp->ai_addr, &rp->ai_addrlen);
		// else
		// {
		// 	// i/o vector
		// 	struct iovec iov[1];
		// 	iov[0].iov_base = buffer;
		// 	iov[0].iov_len  = sizeof(buffer);
		
		// 	// ttl goes here
		// 	char ctrl_buf[CMSG_SPACE(sizeof(int))];

		// 	struct msghdr msg  = {0};
		// 	msg.msg_name       = rp->ai_addr;
		// 	msg.msg_namelen    = sizeof(*rp->ai_addr);
		// 	msg.msg_iov        = iov;
		// 	msg.msg_iovlen     = 1;
		// 	msg.msg_control    = ctrl_buf;
		// 	msg.msg_controllen = sizeof(ctrl_buf);

		// 	process_status = recvmsg(data->sockfd, &msg, 0);

		// 	int ttl = -1;

		// 	for (struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg); 
		// 	     cmsg != NULL; 
		// 	     cmsg = CMSG_NXTHDR(&msg, cmsg)) {
				
		// 	    if (cmsg->cmsg_level == SOL_IP && cmsg->cmsg_type == IP_TTL) {
		// 	        ttl = *(int *)CMSG_DATA(cmsg);
		// 	        break;
		// 	    }
		// 	}
		// }

		// recv timeout or recv error
		if (process_status == -1)
			return (1);

		ip = (struct iphdr *) buffer;
		icmp = (struct icmphdr *) (buffer + ip->ihl * 4);

		process_status = verify_packet(data, ip, icmp, rp);
	}

	char *payload = (void *)icmp + HEADER_SIZE;
	memcpy(&data->time, payload, sizeof(struct timeval));

	double rtt = timespan_ms(&data->time);
	update_stats(&data->stat_bag, rtt);

	if (FLAG(ROOT_FLAG))
		print_ping_reply((struct sockaddr_in *)rp->ai_addr, icmp, ip->ttl, rtt, sizeof(data->packet));
	else
		print_ping_reply((struct sockaddr_in *)rp->ai_addr, icmp, -1, rtt, sizeof(data->packet));
	if (FLAG(VERBOSE_FLAG))
		manage_verbose(data, ip, icmp);

	return (0);
}

/*
	Verify the packet integrity to ensure this is our ping reply

	Returns (0) only if we need to stop polling with recv
	Returns (1) in case of wrong request type 
		i.e. localhost test receive echo request before the reply
*/
static int	verify_packet(t_ping *data, struct iphdr *ip, struct icmphdr *icmp, struct addrinfo *rp)
{
	(void) ip;
	(void) rp;

	if (DEBUG)
		icmp_type_debug(icmp->type);

	// kernel may already verify the chksum
	if (verify_checksum(icmp))
		return (0);

	// verify the ICMP id or socket id (depends on socket mode)
	if ((icmp->un.echo.id != data->header.un.echo.id)
			&& ntohs(icmp->un.echo.id) != data->sockid)
	{
		DEBUG_PRINT("packet not for us");
		return (1);
	}

	// pinging localhost
	if (icmp->type == ECHO_REQUEST)
		return (1);

	DEBUG_PRINT("Got pid=%u seq=%u\n",
			ntohs(icmp->un.echo.id), icmp->un.echo.sequence);
	return (0);
}

/*
	Reply form:
		64 bytes from 172.217.22.78: icmp_seq=0 ttl=113 time=54.525 ms
*/
static void print_ping_reply(struct sockaddr_in *from, struct icmphdr *icmp, int ttl, double rtt_ms, int bytes_received)
{
    char ip_str[INET_ADDRSTRLEN];
    
    inet_ntop(AF_INET, &(from->sin_addr), ip_str, INET_ADDRSTRLEN);
    
    uint16_t seq = icmp->un.echo.sequence;

    printf("%d bytes from %s: icmp_seq=%u ttl=%d time=%.1f ms\n",
           bytes_received, ip_str, seq, ttl, rtt_ms);
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
	ICMP codes can be found here:

	https://www.iana.org/assignments/icmp-parameters#icmp-parameters-codes-0
*/
static int	manage_verbose(t_ping *data, struct iphdr *ip, struct icmphdr *icmp)
{
	/*
		Type 3: Destination Unreachable (e.g., Port Unreachable, Network Unreachable).
		Type 4: Source Quench (flow control).
		Type 5: Redirect (tells the host to use a alternative router).
		Type 11: Time Exceeded (TTL expired in transit)
	*/

	(void) data;
	(void) ip;
	(void) icmp;

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
        printf("ping: no packet received\n");
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
