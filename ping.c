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

		// find the right address from the getaddrinfo return
		// for (rp = res; rp != NULL; rp = rp->ai_next)
		// {
		// }
		// if (rp == NULL)
		// 	data->ret = 1;
		rp = res;
		char buffer[MAX_IP_SIZE];
		int j = 0, rec = 0;
		struct iphdr *ip = NULL;
		struct icmphdr *icmp = NULL;
		while (1)
		{
			// send
			if (ping_send(data, rp))
				break ;

			// wait for answer or timeout
			rec = recvfrom(data->sockfd, buffer, MAX_IP_SIZE, NO_FLAG, rp->ai_addr, &rp->ai_addrlen);
			if (rec > 0)
			{
				ip = (struct iphdr *) buffer;
				icmp = (struct icmphdr *) (buffer + ip->ihl * 4);
  				if (DEBUG)
					icmp_type_debug(icmp->type);
			}
			else
				continue ;

			if (verify_checksum(icmp))
			{
				usleep(1000000); // spam prevention while WIP
				continue ;
			}

			printf("Got pid=%u seq=%d j=%d\n", ntohs(icmp->un.echo.id), icmp->un.echo.sequence, i);
			printf("sent id=%u reply id=%u\n",
       					ntohs(data->header.un.echo.id),
       					ntohs(icmp->un.echo.id));

			char *payload = (void *)icmp + HEADER_SIZE;
			memcpy(&data->time, payload, sizeof(struct timeval));
			printf("since ping %lu ms = %lu us\n", timespan_ms(&data->time), timespan_us(&data->time));

			j++;
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
		return (NULL);
	}
	if (DEBUG)
		address_format_debug((struct sockaddr_in *)res->ai_addr, av);

	return (res);
}
