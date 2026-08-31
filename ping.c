/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cczerwin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/16 19:56:30 by cczerwin          #+#    #+#             */
/*   Updated: 2026/07/16 19:56:33 by cczerwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"

// Static section
static void	clean_close(t_ping *data);
static void	*resolve_host(t_ping *data, char *av);

int	main(int ac, char** av)
{
	if (ac == 1)
	{
		fprintf(stderr, "%s", MSG_MISS_HOST);
		return (64);
	}
	t_ping data = (t_ping){0};
	data.ret = ping_init(&ac, av, &data);
	if (!data.ret)
		do_ping(&data);
	clean_close(&data);

	return (data.ret);
}

/*
	Properly close the socket before returning
*/
static void	clean_close(t_ping *data)
{
	// if closing a fd fail, pray your OS will do the cleanup
	shutdown(data->sockfd, SHUT_RDWR);
	close(data->sockfd);
}

void	do_ping(t_ping *data)
{
	// alarm()
	struct addrinfo *res = NULL, *rp = NULL;

	for (int i = 1; data->av[i] != NULL; i++)
	{
		if (data->av[i][0] == '-')
			continue ;

		res = resolve_host(data, data->av[i]);
		if (data->ret)
			return ;

		if (DEBUG)
		{
			struct sockaddr_in *ip = (struct sockaddr_in *)res->ai_addr;
			if (inet_pton(AF_INET, data->av[i], &ip->sin_addr.s_addr))
				DEBUG_PRINT("found IP! (%s)", data->av[i]);
			else
				DEBUG_PRINT("found Hostname! (%s = %s)", data->av[i], inet_ntoa(ip->sin_addr));
		}

		// find the right address from the getaddrinfo return
		// for (rp = res; rp != NULL; rp = rp->ai_next)
		// {
		// }
		// if (rp == NULL)
		// 	data->ret = 1;
		rp = res;
		char buffer[100];
		buffer[99] = 0;
		int i = 0, rec = 0;
		long time;
		uint16_t old;
		struct timeval tv = {0, 0};
		struct iphdr ip;
		while (1)
		{
			// send
			if (ping_send(data, rp))
				break ;

			// search/wait for answer
			rec = recvfrom(data->sockfd, buffer, 99, NO_FLAG, rp->ai_addr, &rp->ai_addrlen);
			if (rec > 0)
			{
  				// printf("ICMP packet received from %s\n", inet_ntoa(peerAddr.sin_addr));
  				struct icmphdr *icmp = (struct icmphdr *) (buffer + sizeof(struct iphdr));
  				switch (icmp->type)
				{
  					case ICMP_ECHOREPLY:      printf("Type: ICMP_ECHOREPLY\n"); 		break;
  					case ICMP_DEST_UNREACH:   printf("Type: ICMP_DEST_UNREACH\n"); 		break;
  					case ICMP_SOURCE_QUENCH:  printf("Type: ICMP_SOURCE_QUENCH\n"); 	break;
  					case ICMP_REDIRECT:       printf("Type: ICMP_REDIRECT\n"); 			break;
  					case ICMP_ECHO:           printf("Type: ICMP_ECHO\n"); 				break;
  					case ICMP_TIME_EXCEEDED:  printf("Type: ICMP_TIME_EXCEEDED\n"); 	break;
  					case ICMP_PARAMETERPROB:  printf("Type: ICMP_PARAMETERPROB\n"); 	break;
  					case ICMP_TIMESTAMP:      printf("Type: ICMP_TIMESTAMP\n"); 		break;
  					case ICMP_TIMESTAMPREPLY: printf("Type: ICMP_TIMESTAMPREPLY\n"); 	break;
  					case ICMP_INFO_REQUEST:   printf("Type: ICMP_INFO_REQUEST\n"); 		break;
  					case ICMP_INFO_REPLY:     printf("Type: ICMP_INFO_REPLY\n"); 		break;
  					case ICMP_ADDRESS:        printf("Type: ICMP_ADDRESS\n"); 			break;
  					case ICMP_ADDRESSREPLY:   printf("Type: ICMP_ADDRESSREPLY\n"); 		break;
  					default:                  printf("Type: <0x%02x>\n", icmp->type); 	break;
  				}
			}

			// struct iphdr *ip = (struct iphdr *)buffer;
			struct icmphdr *icmp = (struct icmphdr *)(buffer + ip.ihl * 4);
			char *payload = (void *)icmp + HEADER_SIZE;
			memcpy(&tv, payload, sizeof(struct timeval));
			time = time_since(&tv, &data->time);
			printf("si on est roleplay on a 0 ici [%ld]\n", time);
			printf("since ping %lu ms = %lu us\n", timespan_ms(&tv), timespan_us(&tv));

			old = icmp->checksum;
			icmp->checksum = 0;
			if (checksum(icmp, HEADER_SIZE + PAYLOAD_SIZE) != old)
			{
				printf("checksum not matching, packet dropped\n");
				continue ;
			}
			printf("Got ty=%d co=%d pid=%u %d %d\n", icmp->type, icmp->code, ntohs(icmp->un.echo.id), icmp->un.echo.sequence, i);
			printf("sent id=%u reply id=%u\n",
       			ntohs(data->header.un.echo.id),
       			ntohs(icmp->un.echo.id));

			// timespan_us(&tv);
			i++;
			usleep(1000000);
			// print shit
		}

		freeaddrinfo(res);
	}
}

static void	*resolve_host(t_ping *data, char *av)
{
	struct addrinfo hint	= (struct addrinfo){0}, *res = NULL;
	hint.ai_family 			= AF_INET;
	hint.ai_flags 			|= AI_CANONNAME;

	data->ret = getaddrinfo(av, NULL, &hint, &res);
	if (data->ret)
	{
		DEBUG_PRINT("getaddr error: %s", gai_strerror(data->ret));
		data->ret = 1;
	}
	return (res);
}
