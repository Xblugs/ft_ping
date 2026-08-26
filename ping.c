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
		while (1)
		{
			// send
			if (ping_send(data, rp))
				break ;

			// search/wait for answer

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
