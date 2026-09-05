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
static void reset_stats(t_stat *stats);



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
	struct addrinfo *res	= NULL, *rp = NULL;

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

		// reset values before next round
		unsigned short ping_left = data->flag_bag.count_val;
		reset_stats(&data->stat_bag);
		data->header.un.echo.sequence = htons(-1);
		int	wait = 1000;
		if (FLAG(INTERVAL_FLAG))
			wait = data->flag_bag.interval;
		DEBUG_PRINT("wait for %u", wait);

		while (ping_left || !FLAG(COUNT_FLAG))
		{
			// send
			if (ping_send(data, rp))
				break ;

			// receive
			ping_receive(data, rp);
			
			usleep(wait * 1000);	// default interval value is 1 second
			ping_left--;
		}
		print_stats(&data->stat_bag, data->av[i]);
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

/*
	Setup the structure
*/
static void reset_stats(t_stat *stats)
{
    stats->tx_count = 0;
	stats->rx_count = 0;
    stats->min = 999999999.0; 
    stats->max = 0.0;
    stats->avg = 0.0;
	stats->stddev = 0.0;
    stats->S = 0.0;
}
