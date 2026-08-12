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

int	main(int ac, char** av)
{
	char ret = 0;

	if (ac == 1)
	{
		printf("%s", MSG_MISS_HOST);
		return (64);
	}
	t_ping data = (t_ping){0};
	ret = ping_init(ac, av, &data);

	return (ret);
}

int	ping_init(int ac, char **av, t_ping *data)
{
	if(arg_parse(ac, av, data))
		return (1);
	// data->header.code = 0;
	data->header.type = ECHO_REQUEST;
	return (0);
}

/*
	[-?, -h, --help] don't need to be stored, execute directly
*/
int arg_parse(int ac, char** av, t_ping *data)
{
	(void)	data;
	char	ret;

	// '-' and '--' are valid inputs
	for (int i = 1; i < ac; i++)
	{
		if (av[i][0] != '-')
		{
			data->host_count++;
			continue;
		}
		if (strlen(av[i]) == 1)
			continue;
		ret = flag_wrapper(av[i], data);

		printf("%s\n", av[i]);
		printf("flag = %#x\n", data->flags);
		if (ret)
			break;
	}
	printf("%d\n", data->host_count);
	printf("%d\n", data->header.type);
	
	return (0);
}
