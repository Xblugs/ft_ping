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

/*
	put prototypes in a header somewhere later
	and make a struct to handle the data

	todo: think about the structs needed
	req: flags, socket_id, message structure, misc values such as icmp_seq
		timestamp and so on
*/

int	main(int ac, char** av)
{
	if (ac == 1)
	{
		printf("%s", MSG_MISS_HOST);
		return (64);
	}
	t_ping data = (t_ping){0};
	ping_init(ac, av, &data);

	return (0);
}

int	ping_init(int ac, char **av, t_ping *data)
{
	arg_parse(ac, av, data);
	// data->header.code = 0;
	data->header.type = ECHO_REQUEST;
	return (0);
}

/*
	[-?, -h, --help] don't need to be stored, execute directly
*/
int arg_parse(int ac, char** av, t_ping *data)
{
	(void) data;

	for (int i = 1; i < ac; i++)
	{
		if (av[i][0] != '-')
		{
			data->host_count++;
			continue;
		}
		printf("%s\n", av[i]);
	}
	printf("%d\n", data->host_count);
	printf("%d\n", data->header.type);
	
	return (0);
}
