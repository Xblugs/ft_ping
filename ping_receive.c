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

#include <arpa/inet.h>
#include <math.h>

#include "ping.h"

// Static section
static void update_stats(t_stat *stats, double rtt);
static int	verify_packet(t_ping *data, struct iphdr *ip, struct icmphdr *icmp, struct addrinfo *rp);
static int	manage_verbose(t_ping *data, struct iphdr *ip, struct icmphdr *icmp);
static void print_ping_reply(struct sockaddr_in *from, struct icmphdr *icmp, int ttl, double rtt_ms, int bytes_received);
static void print_ping_ttl_exceed(t_ping *data, struct iphdr *ip, struct icmphdr *icmp);

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
		icmp = (struct icmphdr *) (buffer + (ip->ihl << 2));

		process_status = verify_packet(data, ip, icmp, rp);
	}

	double rtt = 0;

	if (icmp->type == ICMP_ECHOREPLY)
	{
		char *payload = (void *)icmp + HEADER_SIZE;
		memcpy(&data->time, payload, sizeof(struct timeval));
		
		rtt = timespan_ms(&data->time);
		update_stats(&data->stat_bag, rtt);
	}

	if (FLAG(ROOT_FLAG))
	{
		if (icmp->type == ICMP_ECHOREPLY)
			print_ping_reply((struct sockaddr_in *)rp->ai_addr, icmp, ip->ttl, rtt, sizeof(data->packet));
		else if (icmp->type == ICMP_TIME_EXCEEDED)
			print_ping_ttl_exceed(data, ip, icmp);
	}
	else // UNUSED, DGRAM SOCKET MODE
		print_ping_reply((struct sockaddr_in *)rp->ai_addr, icmp, -1, rtt, sizeof(data->packet));

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

	// pinging localhost
	if (icmp->type == ICMP_ECHO)
		return (1);

	// verify the ICMP id or socket id (depends on socket mode)
	if ((icmp->un.echo.id != data->header.un.echo.id)
			&& ntohs(icmp->un.echo.id) != data->sockid
			&& icmp->type == ICMP_ECHOREPLY)
	{
		DEBUG_PRINT("packet not for us: id=%d hdr=%d ntohs_id=%d sockid=%d",
			icmp->un.echo.id, data->header.un.echo.id, ntohs(icmp->un.echo.id), data->sockid);
		return (1);
	}

	DEBUG_PRINT("Got pid=%u seq=%u\n",
			ntohs(icmp->un.echo.id), icmp->un.echo.sequence);
	return (0);
}

/*
	Reply form:	(ICMP type 0)
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
	Reply form: (ICMP type 11)
		92 bytes from 72.14.219.248: Time to live exceeded
*/
static void print_ping_ttl_exceed(t_ping *data, struct iphdr *ip, struct icmphdr *icmp)
{
    printf("%d bytes from %s: Time to live exceeded\n",
           ntohs(ip->tot_len) - (ip->ihl << 2), inet_ntoa(*((struct in_addr *) &ip->saddr)));
	
	/*
		The entire original IP request is sent back inside the current request
		This is why this it becomes 92 bytes (with default size)
	*/
	if (FLAG(VERBOSE_FLAG))
	{
		struct iphdr *inner_ip = (struct iphdr *) ((char *)icmp + 8);
		struct icmphdr *inner_icmp = (struct icmphdr *) ((char *)inner_ip + inner_ip->ihl * 4);
		manage_verbose(data, inner_ip, inner_icmp);
	}
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
	This function prints more information during transmission issues

	ICMP codes can be found here:
	https://www.iana.org/assignments/icmp-parameters#icmp-parameters-codes-0
*/
static int	manage_verbose(t_ping *data, struct iphdr *ip, struct icmphdr *icmp)
{
	(void) data;

	size_t j, hlen = ip->ihl << 2;
	unsigned char *cp = (unsigned char *) ip + sizeof (*ip);	/* point to options */

	/*
		Expected output:

		IP Hdr Dump:
		 4500 0054 4c56 4000 0101 fdaf 0a0b c8af 8efa cdee 
		Vr HL TOS  Len   ID Flg  off TTL Pro  cks      Src	Dst	Data
		 4  5  00 0054 4c56   2 0000  01  01 fdaf 10.11.200.175  142.250.205.238 
		ICMP: type 8, code 0, size 64, id 0x20aa, seq 0x0000
	*/

	// Raw hexa bytes for iphdr size (20 bytes)
	printf("IP Hdr Dump:\n ");
    for (j = 0; j < sizeof (*ip); ++j)
		printf("%02x%s", *((unsigned char *) ip + j),
			(j % 2) ? " " : "");	/* Group bytes two by two.  */
    printf("\n");

	/*
		Vr  = IP Version = 4 ; We only manage IPv4
		HL  = IP Header length
		TOS = Type of Service, see above for details
	*/
  	printf("Vr HL TOS  Len   ID Flg  off TTL Pro  cks      Src\tDst\tData\n");
  	printf(" %1x  %1x  %02x", ip->version, ip->ihl, ip->tos);

	/*
		Len = total IP length (needed in the case of additional data)
		ID  = Identifier in case of fragmented packet
	*/
	printf(" %04x %04x", ntohs(ip->tot_len), ntohs(ip->id));

	/*
		Flg = Fragmentation flags
		off = Fragment Offset (0000 if not fragmented)
	*/
  	printf("   %1x %04x", (ntohs(ip->frag_off) & 0xe000) >> 13,
		ntohs(ip->frag_off) & 0x1fff);

	/*
		TTL = Time to live
		Pro = Protocol used	inside the packet (01 for ICMP)
		cks = Checksum value
		Src = Source adress (From where was the answer sent from)
		Dst = Destination adress

	*/
  	printf("  %02x  %02x %04x", ip->ttl, ip->protocol, ntohs(ip->check));
  	printf(" %s ", inet_ntoa(*((struct in_addr *) &ip->saddr)));
  	printf(" %s ", inet_ntoa(*((struct in_addr *) &ip->daddr)));

  	while (hlen-- > sizeof (*ip))
  	  printf("%02x", *cp++);

	hlen = ip->ihl << 2;
  	printf("\n");
	printf("ICMP: type %u, code %u, size %lu", icmp->type, icmp->code,
	      ntohs(ip->tot_len) - hlen);
	if (icmp->type == ICMP_ECHOREPLY || icmp->type == ICMP_ECHO)
		printf (", id 0x%04x, seq 0x%04x",
			icmp->un.echo.id, icmp->un.echo.sequence);
	printf("\n");

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
    printf("--- %s ping statistics ---\n", host);
	printf("%d packets transmitted, %d packets received, %.0f%% packet loss\n",
		stats->tx_count, stats->rx_count, 100 - ((float) stats->rx_count / (float) stats->tx_count * 100));
    
	if (stats->rx_count > 0)
	{
		// Variance = S / n
		double variance = stats->S / stats->rx_count;
		stats->stddev = sqrt(variance);
		printf("rtt min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n",
           stats->min, stats->avg, stats->max, stats->stddev);
	}
}
