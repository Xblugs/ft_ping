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
static void	resolve_host(t_ping *data, char *av, struct addrinfo *hint, struct addrinfo **res);
static 

int	main(int ac, char** av)
{
	if (ac == 1)
	{
		printf("%s", MSG_MISS_HOST);
		return (64);
	}
	t_ping data = (t_ping){0};
	data.ret = ping_init(&ac, av, &data);
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

static void	do_ping(t_ping *data)
{
	// alarm()
	struct addrinfo hint	= (struct addrinfo){0}, *res = NULL, *rp = NULL;
	hint.ai_family 			= AF_INET;
	hint.ai_flags 			|= AI_CANONNAME;

	for (int i = 1; data->av[i] != NULL; i++)
	{
		if (data->av[i][0] == '-')
			continue ;

		resolve_host(data, data->av[i], &hint, &res);
		if (data->ret)
			return ;

		if (DEBUG)
		{
			struct sockaddr_in *ip = (struct sockaddr_in *)res->ai_addr;
			if (inet_pton(AF_INET, data->av[i], &ip->sin_addr.s_addr))
				printf(PURPLE"IP! (%s)\n"END, data->av[i]);
			else
				printf(PURPLE"Hostname! (%s)\n"END, data->av[i]);
		}

		for (rp = res; rp != NULL; rp = rp->ai_next)
		{
			printf("rp=%p next=%p canonname=%s\n", rp, rp->ai_next, rp->ai_canonname);

			ssize_t bytes_sent = sendto(data->sockfd, 
                            data->packet, HEADER_SIZE + PAYLOAD_SIZE, 
                            0, 
                            rp->ai_addr, rp->ai_addrlen);
							
			if (bytes_sent >= 0)
				break ;
		}
		if (rp == NULL)
		{
			printf("Not found :c\n");
			data->ret = 1;
		}
		else
			printf("We in boss\n");
		freeaddrinfo(res);
	}
}

static void	resolve_host(t_ping *data, char *av, struct addrinfo *hint, struct addrinfo **res)
{
	data->ret = getaddrinfo(av, NULL, hint, res);
	if (data->ret)
	{
		if (DEBUG)
			printf(PURPLE"getaddr error: %s\n"END, gai_strerror(data->ret));
		data->ret = 1;
	}
}
